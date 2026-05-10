#include <windows.h>
#include "Settings.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <MinHook.h>
// Misc
#include "CDataFileMgr.h"
#include "Events.h"
// Patches
#include "GroupMap.h"
#include "DecalLimits.h"
#include "RadioStations.h"
#include "PopZones.h"
#include "RockstarEditor.h"
#include "Water.h"
#include "Paths.h"
#include "mojito_wt_api.h"

static void LoggingEnabled()
{
	spdlog::set_level(spdlog::level::info);
	spdlog::set_default_logger(spdlog::basic_logger_mt("WorldTravelPatches", "WorldTravelPatches.log"));
	spdlog::flush_every(std::chrono::seconds(5));
}

DWORD WINAPI Main()
{
	Settings::Load();
	
	if (Settings::EnableLogging)
	{
		LoggingEnabled();
	}
	else 
	{
		spdlog::set_level(spdlog::level::off);
	}

	MH_Initialize();

	Events::Install();
	spdlog::info("Added Game Skeleton Event Hooks.");

	GroupMap::Init();
	spdlog::info("Added Group Map Hooks.");

	RadioStations::Patch();
	spdlog::info("Patched Radio Stations.");

	DecalLimits::PatchDecalLimits();
	spdlog::info("Patched Decal Limits.");

	DecalLimits::fwBoxStreamerVariablePatch();
	spdlog::info("Patched fwBoxStreamerVariable.");

	PopZones::Patch();
	spdlog::info("Patched PopZone / PopCycle.");

	RockstarEditor::Patch();
	spdlog::info("Patched Rockstar Editor.");

	Water::Init();
	spdlog::info("Patched Water.");

	mojito::Init();
	spdlog::info("Init Mojito FW");

	MH_EnableHook(MH_ALL_HOOKS);
	return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		Main();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		spdlog::shutdown();
	}

	return TRUE;
}
