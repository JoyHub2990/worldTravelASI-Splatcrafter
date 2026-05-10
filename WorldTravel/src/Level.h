#include <string>
#include "../dependencies/include/natives.h"
#include <fstream>  
#include <windows.h>
#include <string>
#include <ctime>
#include <iostream>
#include <vector>
#include "HelpText.h"
#include "Menu.h"
#include "MpMap.h"
#include "PlayerLocation.h"
#include "Farlods.h"
#include "PathNodeState.h"
#include "RestrictedAreaClearance.h"
#include "WaterState.h"
#include <unordered_map>
#include <functional>
#include "AmbientZone.h"
#include "AmbientZoneGroup.h"
#include "Ipl.h"
#include "IplGroup.h"
#include "RadioStation.h"
#include "RadioStationGroup.h"
#include "Scenario.h"
#include "ScenarioGroup.h"
#include "WeatherTypes.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZonedModel.h"
#include "ZonedModelGroup.h"
#include "Settings.h"

using namespace std;
#ifndef LEVEL
#define LEVEL
class Level
{
public:
	std::string levelName;
	bool defaultMap;
	int levelLocationId;


	AmbientZoneGroup ambientZoneGroup;
	IplGroup baseIplGroup;
	IplGroup spIplGroup;
	IplGroup mpIplGroup;
	IplGroup mpForcedIplGroup;
	IplGroup modIplGroup;
	ZonedModelGroup pedGroup;
	RadioStationGroup radioGroup;
	ScenarioGroup scenarioGroup;
	ZonedModelGroup vehicleGroup;
	WeatherTypes weatherTypes;
	ZoneGroup zoneGroup;


	bool hasAmbientZoneGroup;
	bool hasBaseIplGroup;
	bool hasSpIplGroup;
	bool hasMpIplGroup;
	bool hasMpForcedIplGroup;
	bool hasModIplGroup;
	bool hasPedGroup;
	bool hasRadioGroup;
	bool hasScenarioGroup;
	bool hasVehicleGroup;
	bool hasWeatherTypes;
	bool hasZoneGroup;

	int pathNodeState;
	int globalWaterFile;
	bool useIslandHopper;
	int minimapState;


	void ReadFiles(const std::string& name);

	void SetProperties(const std::string& name, int p_levelLocationId,
		int p_pathNodeState, int p_globalWaterFile, int p_minimapState,
		bool p_defaultMap, bool p_useIslandHopper);

	bool IsMapActive();

	void RequestScript(PCHAR scriptName, int stackSize);

	void LoadLevel();

	void UnloadLevel();

	void RemoveForcedIpls();

	void DisableActiveFiles();
	
	void EnableRadios();

	void DisableZoneGroup();
};
#endif