#include "Paths.h"
#include "Events.h"
#include <MinHook.h>
#include "Hooking.Patterns.h"
#include "CPathFind.h"
#include "CDataFileMgr.h"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <windows.h>

// Unused Currently

CDataFileMgr::DataFile* Paths::GetRpfFile(const char* filename)
{
    CDataFileMgr* mgr = CDataFileMgr::Instance();

    for (int i = 0; i < mgr->m_DataFiles.Count; i++)
    {
        CDataFileMgr::DataFile* file = &mgr->m_DataFiles.Items[i];

        int cmp = strncmp(file->filename, filename, sizeof(file->filename));
        if (file->fileType == CDataFileMgr::RPF_FILE && cmp == 0)
        {
            return file;
        }
    }

    return nullptr;
}

void Paths::CheckForSwitch()
{
    if (!wantsToSwitchPaths)
        return;

    constexpr const char* vanillaFilename = "platform:/levels/gta5/paths.rpf";
    constexpr const char* customFilename = "platform:/levels/gta5/mypaths.rpf";

    CDataFileMgr::DataFile* vanillaFile = GetRpfFile(vanillaFilename);
    CDataFileMgr::DataFile* customFile = GetRpfFile(customFilename);

    spdlog::info("VanillaFile: {}", (void*)vanillaFile);
    spdlog::info("CustomFile: {}", (void*)customFile);

    if (vanillaFile && customFile)
    {
        CPathFind* pf = CPathFind::Instance();

        static void(*strStreaming_ClearRequiredFlag)(void*, uint32_t, uint32_t) =
            (decltype(strStreaming_ClearRequiredFlag))hook::get_pattern("89 54 24 10 48 83 EC 28 4C 8B C9 8B CA 4D 8B 11");

        static bool(*strStreaming_RemoveObject)(void*, uint32_t) =
            (decltype(strStreaming_RemoveObject))hook::get_pattern("4C 8B 31 8B F2 4C 8B F9 41 F6 44 F6 ? ? 75 07", -0x23);

        static void* strStreaming = []()
            {
                auto addr = hook::get_pattern<uint8_t>("48 8B 05 ? ? ? ? 41 B1 01 48 8D 1C C8", 3);
                return addr + *(int*)addr + 4;
            }();

        for (int i = 0; i < 1024; i++)
        {
            strStreaming_ClearRequiredFlag(strStreaming, pf->GetBaseIndex() + i, 0x11);
            strStreaming_RemoveObject(strStreaming, pf->GetBaseIndex() + i);
            pf->Remove(i);
        }

        if (vanillaPaths)
        {
            CFileLoader::DisableRpfFile(vanillaFile);
            CFileLoader::EnableRpfFile(customFile);
        }
        else
        {
            CFileLoader::DisableRpfFile(customFile);
            CFileLoader::EnableRpfFile(vanillaFile);
        }

        spdlog::info("SWITCHED PATHS");
        spdlog::info("VanillaFile: disabled:{}", vanillaFile->disabled);
        spdlog::info("CustomFile: disabled:{}", customFile->disabled);

        vanillaPaths = !vanillaPaths;
        spdlog::info("VanillaPaths:{}", vanillaPaths);
    }

    wantsToSwitchPaths = false;
}

template<int FramesToSkip>
void Paths::LogStackTrace()
{
    void* stack[32];
    USHORT frames = CaptureStackBackTrace(FramesToSkip, 32, stack, NULL);

    spdlog::warn("\tStack Trace:");
    for (int i = 0; i < frames; i++)
    {
        void* address = stack[i];
        HMODULE module = NULL;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR)address, &module);
        char moduleName[256];
        GetModuleFileName(module, moduleName, 256);
        // spdlog::warn("\t\t{:16X} - {}+{:08X}", (uintptr_t)address, std::filesystem::path(moduleName).filename().string().c_str(), ((uintptr_t)address - (uintptr_t)module));
    }
}

uint32_t* Paths::CPathFind_Register_detour(CPathFind* pf, uint32_t* outIndex, const char* name)
{
    CPathFind_Register_orig(pf, outIndex, name);
    spdlog::info("CPathFind::Register: index:{}, name:{}", *outIndex, name);
    LogStackTrace<2>();
    return outIndex;
}

bool Paths::CPathFind_PlaceResource_detour(CPathFind* pf, uint32_t index, void* resourceMap, void* resourceInfo)
{
    spdlog::info("CPathFind::PlaceResource: index:{}", index);
    LogStackTrace<2>();
    return CPathFind_PlaceResource_orig(pf, index, resourceMap, resourceInfo);
}

void Paths::Init()
{
    Events::OnCorePreUpdate.Add(CheckForSwitch);

    MH_CreateHook(
        hook::get_pattern("40 53 48 83 EC 20 48 8B DA 49 8D 48 05 4C 8D 44 24 ?"),
        CPathFind_Register_detour,
        reinterpret_cast<void**>(&CPathFind_Register_orig)
    );

    MH_CreateHook(
        hook::get_pattern("85 D2 78 59 48 89 5C 24 ? 89 54 24 10"),
        CPathFind_PlaceResource_detour,
        reinterpret_cast<void**>(&CPathFind_PlaceResource_orig)
    );
}

void Paths::SwitchPaths()
{
    wantsToSwitchPaths = true;
}
