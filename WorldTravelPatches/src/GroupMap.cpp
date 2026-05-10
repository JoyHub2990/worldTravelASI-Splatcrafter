#include "GroupMap.h"
#include "Hooking.Patterns.h"
#include "Hashing.h"
#include "Settings.h"
#include <windows.h>

void GroupMap::Init()
{
    if (joaat(Settings::DefaultGroupMap) != joaat("GROUP_MAP_SP"))
    {
        void* loc = hook::get_pattern("75 0D BA ? ? ? ? 48 8B CE E8 ? ? ? ? 41 B0 01 BA", 3); // CExtraContentManager::Init
        DWORD oldProtect;
        VirtualProtect(loc, 4, PAGE_READWRITE, &oldProtect);
        *(unsigned int*)loc = joaat(Settings::DefaultGroupMap);
        VirtualProtect(loc, 4, oldProtect, &oldProtect);
    }
}
