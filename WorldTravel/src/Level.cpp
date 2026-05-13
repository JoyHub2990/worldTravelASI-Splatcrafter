#include "Level.h"

using namespace std;





void Level::ReadFiles(const std::string& name)
{
	levelName = name;
	std::string mainLevelFolder = "./Levels/";
	std::string extension = ".txt";
	std::string ambientZonePath = mainLevelFolder + name + "/AmbientZones" + extension;
	std::string baseIplPath = mainLevelFolder + name + "/IPLs" + extension;
	std::string spIplPath = mainLevelFolder + name + "/IPLsSP" + extension;
	std::string mpIplPath = mainLevelFolder + name + "/IPLsMP" + extension;
	std::string mpForcedIplPath = mainLevelFolder + name + "/IPLsMPForced" + extension;
	std::string modIplPath = mainLevelFolder + name + "/ModIPLs" + extension;
	std::string lodLightIplPath = mainLevelFolder + name + "/IPLsLODLights" + extension;
	std::string pedPath = mainLevelFolder + name + "/ZonedPeds" + extension;
	std::string radioPath = mainLevelFolder + name + "/ZonedRadioStations" + extension;
	std::string scenarioPath = mainLevelFolder + name + "/Scenarios" + extension;
	std::string vehiclePath = mainLevelFolder + name + "/ZonedVehicles" + extension;
	std::string weatherTypesPath = mainLevelFolder + name + "/WeatherTypes" + extension;
	std::string zonePath = mainLevelFolder + name + "/Zones" + extension;
	
	hasAmbientZoneGroup = worldtravel::file::doesFileExist(ambientZonePath);
	if (hasAmbientZoneGroup)
		ambientZoneGroup.ReadFile(ambientZonePath);

	hasBaseIplGroup = worldtravel::file::doesFileExist(baseIplPath);
	if (hasBaseIplGroup)
		baseIplGroup.ReadFile(baseIplPath);
	
	hasSpIplGroup = worldtravel::file::doesFileExist(spIplPath);
	if (hasSpIplGroup)
		spIplGroup.ReadFile(spIplPath);
	
	hasMpIplGroup = worldtravel::file::doesFileExist(mpIplPath);
	if (hasMpIplGroup)
		mpIplGroup.ReadFile(mpIplPath);
		
	hasMpForcedIplGroup = worldtravel::file::doesFileExist(mpForcedIplPath);
	if (hasMpForcedIplGroup)
		mpForcedIplGroup.ReadFile(mpForcedIplPath);

	hasModIplGroup = worldtravel::file::doesFileExist(modIplPath);
	if (hasModIplGroup)
		modIplGroup.ReadFile(modIplPath);

	hasLodLightIplGroup = worldtravel::file::doesFileExist(lodLightIplPath);
	if (hasLodLightIplGroup)
		lodLightIplGroup.ReadFile(lodLightIplPath);
	
	hasPedGroup = worldtravel::file::doesFileExist(pedPath);
	if (hasPedGroup)
		pedGroup.ReadFile(pedPath);
	
	hasRadioGroup = worldtravel::file::doesFileExist(radioPath);
	if (hasRadioGroup)
		radioGroup.ReadFile(radioPath);
		
	hasScenarioGroup = worldtravel::file::doesFileExist(scenarioPath);
	if (hasScenarioGroup)
		scenarioGroup.ReadFile(scenarioPath);
		
	hasVehicleGroup = worldtravel::file::doesFileExist(vehiclePath);
	if (hasVehicleGroup)
		vehicleGroup.ReadFile(vehiclePath);
		
	hasWeatherTypes = worldtravel::file::doesFileExist(weatherTypesPath);
	if (hasWeatherTypes)
		weatherTypes.ReadFile(weatherTypesPath);
		
	hasZoneGroup = worldtravel::file::doesFileExist(zonePath);
	if (hasZoneGroup)
		zoneGroup.ReadFile(zonePath);
		
}

void Level::SetProperties(const std::string& name, int p_levelLocationId,
	int p_pathNodeState, int p_globalWaterFile, int p_minimapState,
	bool p_defaultMap, bool p_useIslandHopper)
{
	Level::ReadFiles(name);
	levelLocationId = p_levelLocationId;
	pathNodeState = p_pathNodeState;
	globalWaterFile = p_globalWaterFile;
	minimapState = p_minimapState;
	defaultMap = p_defaultMap;
	useIslandHopper = p_useIslandHopper;
}

bool Level::IsMapActive()
{
	return worldtravel::GetPlayerLocationID() == levelLocationId;
}

void Level::RequestScript(PCHAR scriptName, int stackSize)
{
	if (SCRIPT::_GET_NUMBER_OF_INSTANCES_OF_SCRIPT_WITH_NAME_HASH(GAMEPLAY::GET_HASH_KEY(scriptName)) == 0 && SCRIPT::DOES_SCRIPT_EXIST(scriptName))
	{
		SCRIPT::REQUEST_SCRIPT(scriptName);

		for (ULONGLONG timeOut = GetTickCount64() + 5000; GetTickCount64() < timeOut;)
		{
			if (SCRIPT::HAS_SCRIPT_LOADED(scriptName))
				break;
			WAIT(0);
		}

		SYSTEM::START_NEW_SCRIPT(scriptName, stackSize); // 1024 on console
		SCRIPT::SET_SCRIPT_AS_NO_LONGER_NEEDED(scriptName);
	}
}

void Level::LoadLevel()
{
	if (useIslandHopper)
	{
		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			STREAMING::SET_ISLAND_ENABLED(const_cast<char*>("HeistIsland"), true);
			WAIT(1000);
		}
	}
	if (hasAmbientZoneGroup)
		ambientZoneGroup.RequestAmbientZoneGroup(defaultMap);
	if (hasBaseIplGroup)
		baseIplGroup.RequestIplGroup(defaultMap);
	if (worldtravel::MpMap::IsMPMapActive())
	{
		if (hasMpIplGroup)
			mpIplGroup.RequestIplGroup(defaultMap);
	}
	else
	{
		if (hasSpIplGroup)
			spIplGroup.RequestIplGroup(defaultMap);
	}
	if (hasModIplGroup)
		modIplGroup.RequestIplGroup(defaultMap);
	if (hasLodLightIplGroup && loadLodLights)
		lodLightIplGroup.RequestIplGroup(defaultMap);
	if (hasPedGroup)
		pedGroup.SuppressZonedModels(false, false);
	if (hasRadioGroup)
		radioGroup.EnableRadioStationGroup();
	if (hasVehicleGroup)
		vehicleGroup.SuppressZonedModels(false, true);
	if (hasWeatherTypes)
	{
		weatherTypes.SetWeatherType();
		if (defaultMap)
			weatherTypes.ResetWeatherType(true);
	}
	if (hasZoneGroup)
		zoneGroup.EnableZoneGroup();
	if (worldtravel::PathNodeState::GetPathNodeState() != pathNodeState)
	{
		worldtravel::PathNodeState::SetPathNodeState(pathNodeState);
	}
	if (pathNodeState == 1)
	{
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(5526.24f, -5137.23f, 61.78925f, 3679.327f, -4973.879f, 125.0828f, 192, false, true, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3691.211f, -4941.24f, 94.59368f, 3511.115f, -4869.191f, 126.7621f, 16, false, true, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3510.004f, -4865.81f, 94.69557f, 3204.424f, -4833.817f, 126.8152f, 16, false, true, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3186.534f, -4832.798f, 109.8148f, 3202.187f, -4833.993f, 114.815f, 16, false, true, true);
	}
	
	if (worldtravel::WaterState::GetWaterState() != globalWaterFile)
	{
		worldtravel::WaterState::SetWaterState(globalWaterFile);
	}
	
	switch (minimapState)
	{
	case 1:
		UI::SET_MINIMAP_IN_PROLOGUE(true);
		break;
	case 2:
		UI::SET_USE_ISLAND_MAP(true);
		break;
	case 3:
		UI::SET_MINIMAP_COMPONENT(14, true, -1);
		break;
	}
	if (defaultMap)
	{
		patches::farlods::SetVisible(true);

		GRAPHICS::DISABLE_VEHICLE_DISTANTLIGHTS(false);

		STREAMING::_SET_MAPDATACULLBOX_ENABLED((char*)"HeistIsland", false);

		// if in singleplayer
		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			UI::_SET_MINIMAP_REVEALED(false);
			for (int i = 0; i < 5; i++)
			{
				GAMEPLAY::DISABLE_HOSPITAL_RESTART(i, false);
			}
			for (int i = 0; i < 7; i++)
			{
				GAMEPLAY::DISABLE_POLICE_RESTART(i, false);
			}

			// enable the following scripts in singleplayer that we disabled when not in LS
			RequestScript("blip_controller", 1424);        // responsible for blips
			RequestScript("forsalesigns", 1424);           // responsible for property for sale signs
			RequestScript("respawn_controller", 1424);     // responsible for respawning the player
			if (!worldtravel::RestrictedAreaClearance::HasRestrictedAreaClearance())
				RequestScript("restrictedareas", 1424);        // responsible for restricted areas
			RequestScript("vehicle_gen_controller", 1424); // responsible for player businesses / vehicle blips
		}
	}
}

void Level::UnloadLevel()
{

	if (useIslandHopper)
	{
		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			STREAMING::SET_ISLAND_ENABLED(const_cast<char*>("HeistIsland"), false);
			WAIT(1000);
			STREAMING::_SET_MAPDATACULLBOX_ENABLED((char*)"HeistIsland", true);
		}
	}
	if (hasAmbientZoneGroup)
		ambientZoneGroup.RemoveAmbientZoneGroup(defaultMap);
	if (hasBaseIplGroup)
		baseIplGroup.RemoveIplGroup(defaultMap);
	if (worldtravel::MpMap::IsMPMapActive())
	{
		if (hasMpIplGroup)
			mpIplGroup.RemoveIplGroup(defaultMap);
	}
	else
	{
		if (hasSpIplGroup)
			spIplGroup.RemoveIplGroup(defaultMap);
	}
	if (hasModIplGroup)
		modIplGroup.RemoveIplGroup(defaultMap);
	if (hasLodLightIplGroup && loadLodLights)
		lodLightIplGroup.RemoveIplGroup(defaultMap);
	if (hasPedGroup)
		pedGroup.SuppressZonedModels(true, false);
	if (hasRadioGroup)
		radioGroup.DisableRadioStationGroup();
	if (hasVehicleGroup)
		vehicleGroup.SuppressZonedModels(true, true);
	if (hasWeatherTypes)
	{
		if (!defaultMap)
			weatherTypes.ResetWeatherType(false);
	}
	if (hasZoneGroup)
		zoneGroup.DisableZoneGroup();
	if (pathNodeState == 1)
	{
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(5526.24f, -5137.23f, 61.78925f, 3679.327f, -4973.879f, 125.0828f, 192, false, false, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3691.211f, -4941.24f, 94.59368f, 3511.115f, -4869.191f, 126.7621f, 16, false, false, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3510.004f, -4865.81f, 94.69557f, 3204.424f, -4833.817f, 126.8152f, 16, false, false, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3186.534f, -4832.798f, 109.8148f, 3202.187f, -4833.993f, 114.815f, 16, false, false, true);
	}

	switch (minimapState)
	{
	case 1:
		UI::SET_MINIMAP_IN_PROLOGUE(false);
		break;
	case 2:
		UI::SET_USE_ISLAND_MAP(false);
		break;
	case 3:
		UI::SET_MINIMAP_COMPONENT(14, false, -1);
		break;
	}
	if (defaultMap)
	{
		patches::farlods::SetVisible(false);

		GRAPHICS::DISABLE_VEHICLE_DISTANTLIGHTS(true);

		STREAMING::_SET_MAPDATACULLBOX_ENABLED((char*)"HeistIsland", true);

		// if in singleplayer
		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			UI::_SET_MINIMAP_REVEALED(true);
			for (int i = 0; i < 5; i++)
			{
				GAMEPLAY::DISABLE_HOSPITAL_RESTART(i, true);
			}
			for (int i = 0; i < 7; i++)
			{
				GAMEPLAY::DISABLE_POLICE_RESTART(i, true);
			}

			// disable the following scripts in singleplayer to remove blips and respawn points when not in LS
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"blip_controller");          // responsible for blips
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"forsalesigns");			   // responsible for property for sale signs
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"respawn_controller");	   // responsible for respawning the player
			if (!worldtravel::RestrictedAreaClearance::HasRestrictedAreaClearance())
				GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"restrictedareas");		   // responsible for restricted areas
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"vehicle_gen_controller");   // responsible for player businesses / vehicle blips
		}
	}
}

void Level::RemoveForcedIpls()
{
	mpForcedIplGroup.RemoveIplGroupIfActive();
}

void Level::DisableActiveFiles()
{

	if (hasAmbientZoneGroup)
		ambientZoneGroup.RemoveAmbientZoneGroupIfActive();
	if (hasBaseIplGroup)
		baseIplGroup.RemoveIplGroup(defaultMap);
	if (worldtravel::MpMap::IsMPMapActive())
	{
		if (hasMpIplGroup)
			mpIplGroup.RemoveIplGroupIfActive();
	}
	else
	{
		if (hasSpIplGroup)
			spIplGroup.RemoveIplGroupIfActive();
	}
	if (hasModIplGroup)
		modIplGroup.RemoveIplGroupIfActive();
	if (hasLodLightIplGroup)
		lodLightIplGroup.RemoveIplGroupIfActive();
	if (hasPedGroup)
		pedGroup.SuppressZonedModels(true, false);
	if (hasRadioGroup)
		radioGroup.DisableRadioStationGroup();
	if (hasVehicleGroup)
		vehicleGroup.SuppressZonedModels(true, true);
	if (hasZoneGroup)
		zoneGroup.DisableZoneGroup();
}

void Level::EnableRadios()
{
	if (hasRadioGroup)
		radioGroup.EnableRadioStationGroup();
}

void Level::DisableZoneGroup()
{
	if (hasZoneGroup)
		zoneGroup.DisableZoneGroup();
}