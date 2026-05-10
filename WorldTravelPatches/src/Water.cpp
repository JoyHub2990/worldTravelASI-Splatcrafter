#include "Water.h"
#include "UsingAllocator.h"
#include "Hooking.h"
#include "Hashing.h"
#include <MinHook.h>
#include "spdlog/spdlog.h"
#include "CDataFileMgr.h"
#include "CPathFind.h"
#include <filesystem>
#include "Events.h"
#include "CWaterData.h"

namespace
{
    // Sanity-check a derived pointer falls inside the GTA5.exe image range
    // *as mapped at runtime*. The hardcoded 0x140000000..0x146000000 range
    // refers to the static PE base; ASLR relocates it to a different runtime
    // base each launch (typically 0x7FF7_xxxx_xxxx on Windows x64), so we
    // bracket the live image with hook::getRVA which already accounts for
    // the relocation delta. A target outside this range means the RIP-
    // relative operand we just read is from the wrong instruction — usually
    // because Rockstar shifted bytes inside the function body and our
    // hardcoded offset is stale.
    inline bool InExeRange(uintptr_t p)
    {
        const uintptr_t lo = hook::getRVA(0);
        const uintptr_t hi = hook::getRVA(hook::exe_end() - 0x140000000);
        return p >= lo && p <= hi;
    }
}

void Water::CheckForLoading()
{
    if (wantToLoadWaterData)
    {
        fCWaterData_Unload();
        fCWaterData_Load(waterDataFileToLoad);

        wantToLoadWaterData = false;
        waterDataFileToLoad[0] = 0;
    }
}

void Water::Init()
{
    char msg[256];

    spdlog::info("Water::Init: scanning for switchWater call-site pattern");
    auto p = hook::pattern("E8 ? ? ? ? 83 0D ? ? ? ? ? 40 88 2D");
    size_t n = p.size();
    sprintf_s(msg, sizeof(msg), "Water::Init: call-site pattern found %zu matches", n);
    spdlog::info(msg);
    if (n != 1)
    {
        spdlog::error("Water::Init: BROKEN, expected exactly 1 match - skipping water hook");
        return;
    }

    uintptr_t callingAddress = reinterpret_cast<uintptr_t>(p.get(0).get<uint8_t>(0));
    uintptr_t origSwitchWaterAddr = reinterpret_cast<uintptr_t>(hook::get_call(callingAddress));
    sprintf_s(msg, sizeof(msg),
              "Water::Init: callSite=0x%p origSwitchWater=0x%p",
              reinterpret_cast<void*>(callingAddress),
              reinterpret_cast<void*>(origSwitchWaterAddr));
    spdlog::info(msg);

    if (!InExeRange(origSwitchWaterAddr))
    {
        spdlog::error("Water::Init: origSwitchWater target outside EXE range - skipping");
        return;
    }

    // Each (offsetTo4ByteAddr, numBytesInLine) pair below describes one
    // RIP-relative operand inside the switchWater function body. These
    // offsets are build-specific: when Rockstar inserts/reorders bytes
    // (as they did between the original LCPP-target build and 3751+),
    // one or more reads will land on the wrong instruction's operand and
    // the resulting pointer falls outside the EXE range.
    CDataFileMgr** sm_instance_tmp  = nullptr;
    eWaterArea*    curLoadedArea_tmp = nullptr;
    int32_t*       bd_min_x_tmp      = nullptr;
    int32_t*       bd_min_y_tmp      = nullptr;
    int32_t*       bd_max_x_tmp      = nullptr;
    int32_t*       bd_max_y_tmp      = nullptr;

    bool ok = true;
    {
        auto raw = hook::get_address<CDataFileMgr**>(origSwitchWaterAddr + 0x97, 3, 7);
        if (!InExeRange(reinterpret_cast<uintptr_t>(raw))) { spdlog::error("Water::Init: sm_Instance out of range (+0x97) - stale offset"); ok = false; }
        else { sprintf_s(msg, sizeof(msg), "Water::Init: sm_Instance -> 0x%p", static_cast<void*>(raw)); spdlog::info(msg); sm_instance_tmp = raw; }
    }
    {
        auto raw = hook::get_address<eWaterArea*>(origSwitchWaterAddr + 0x0E, 2, 6);
        if (!InExeRange(reinterpret_cast<uintptr_t>(raw))) { spdlog::error("Water::Init: curLoadedArea out of range (+0x0E) - stale offset"); ok = false; }
        else { sprintf_s(msg, sizeof(msg), "Water::Init: curLoadedArea -> 0x%p", static_cast<void*>(raw)); spdlog::info(msg); curLoadedArea_tmp = raw; }
    }
    {
        auto raw = hook::get_address<int32_t*>(origSwitchWaterAddr + 0x29, 2, 6);
        if (!InExeRange(reinterpret_cast<uintptr_t>(raw))) { spdlog::error("Water::Init: bd_min_x out of range (+0x29) - stale offset"); ok = false; }
        else { sprintf_s(msg, sizeof(msg), "Water::Init: bd_min_x -> 0x%p", static_cast<void*>(raw)); spdlog::info(msg); bd_min_x_tmp = raw; }
    }
    {
        auto raw = hook::get_address<int32_t*>(origSwitchWaterAddr + 0x2F, 2, 6);
        if (!InExeRange(reinterpret_cast<uintptr_t>(raw))) { spdlog::error("Water::Init: bd_min_y out of range (+0x2F) - stale offset"); ok = false; }
        else { sprintf_s(msg, sizeof(msg), "Water::Init: bd_min_y -> 0x%p", static_cast<void*>(raw)); spdlog::info(msg); bd_min_y_tmp = raw; }
    }
    {
        auto raw = hook::get_address<int32_t*>(origSwitchWaterAddr + 0x1F, 2, 10);
        if (!InExeRange(reinterpret_cast<uintptr_t>(raw))) { spdlog::error("Water::Init: bd_max_x out of range (+0x1F) - stale offset"); ok = false; }
        else { sprintf_s(msg, sizeof(msg), "Water::Init: bd_max_x -> 0x%p", static_cast<void*>(raw)); spdlog::info(msg); bd_max_x_tmp = raw; }
    }
    {
        auto raw = hook::get_address<int32_t*>(origSwitchWaterAddr + 0x35, 2, 10);
        if (!InExeRange(reinterpret_cast<uintptr_t>(raw))) { spdlog::error("Water::Init: bd_max_y out of range (+0x35) - stale offset"); ok = false; }
        else { sprintf_s(msg, sizeof(msg), "Water::Init: bd_max_y -> 0x%p", static_cast<void*>(raw)); spdlog::info(msg); bd_max_y_tmp = raw; }
    }

    if (!ok)
    {
        spdlog::error("Water::Init: one or more derived pointers are bogus, skipping hook install. "
                      "Send WorldTravelPatches.log to fix the offsets for this build.");
        return;
    }

    // Commit only after every read passed the range check. Installing the
    // hook with one bad pointer would scribble into random memory on the
    // first level switch.
    CDataFileMgr::sm_Instance = sm_instance_tmp;
    CWaterData::curLoadedArea = curLoadedArea_tmp;
    CWaterData::bd_min_x      = bd_min_x_tmp;
    CWaterData::bd_min_y      = bd_min_y_tmp;
    CWaterData::bd_max_x      = bd_max_x_tmp;
    CWaterData::bd_max_y      = bd_max_y_tmp;

    intptr_t allocatedFunctionAddress =
        reinterpret_cast<intptr_t>(hook::AllocateFunctionStub(
            reinterpret_cast<void*>(origSwitchWaterAddr),
            reinterpret_cast<void*>(CWaterData::switchWater),
            0));
    int32_t offset = static_cast<int32_t>(allocatedFunctionAddress - (callingAddress + 5));
    hook::put<int32_t>(callingAddress + 1, offset);

    sprintf_s(msg, sizeof(msg),
              "Water::Init: hook installed at 0x%p (stub=0x%p)",
              reinterpret_cast<void*>(callingAddress),
              reinterpret_cast<void*>(allocatedFunctionAddress));
    spdlog::info(msg);
}

void Water::Load(const char* filePath)
{
    strcpy_s(waterDataFileToLoad, filePath);
    wantToLoadWaterData = true;
}

void Water::BeginLoadScreen()
{
    prevGameState = *gameState;
    *gameState = 2;
}

void Water::EndLoadScreen()
{
    *gameState = prevGameState;
    prevGameState = 0xFFFFFFFF;
}
