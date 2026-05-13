#include "LevelSwitch.h"
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
#include "Level.h"
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

namespace levelSwitch
{

	// Streaming Data Index
	int LibertyCityScenariosIndex;
	int LibertyCityZonesIndex;
	int LibertyCityIPLsIndex;
	int LosSantosScenariosIndex;
	int LosSantosZonesIndex;
	int LosSantosIPLsIndex;
	int ModIPLsIndex;

	// Streaming Data References
	bool loadLibertyCity;
	bool loadLibertyLODLights;
	bool loadNorthYankton;
	bool loadCayoPerico;
	bool loadSantaFuegos;
	bool fastTravel;
	bool restrictedAreaClearance;
	int tpKey;
	//std::vector<std::string> libertyScenarios;
	ScenarioGroup libertyScenarioGroup;
	//std::vector<std::string> libertyZones;
	ZoneGroup libertyZoneGroup;
	//std::vector<std::string> libertyAmbientZones;
	AmbientZoneGroup libertyAmbientZoneGroup;
	//std::vector<std::string> libertyIpls;
	//std::vector<std::string> libertySpIpls;
	//std::vector<std::string> libertyMpIpls;
	//std::vector<std::string> libertyLODLightIpls;
	IplGroup libertyIplGroup;
	IplGroup libertySpIplGroup;
	IplGroup libertyMpIplGroup;
	IplGroup libertyLODLightIplGroup;
	//std::vector<std::string> libertyPeds;
	//std::vector<std::string> libertyVehicles;
	ZonedModelGroup libertyPedGroup;
	ZonedModelGroup libertyVehicleGroup;
	//std::vector<std::string> libertyRadio;
	RadioStationGroup libertyRadioGroup;
	//std::vector<std::string> santosScenarios;
	ScenarioGroup santosScenarioGroup;
	//std::vector<std::string> santosZones;
	ZoneGroup santosZoneGroup;
	//std::vector<std::string> santosAmbientZones;
	AmbientZoneGroup santosAmbientZoneGroup;
	//std::vector<std::string> santosIpls;
	//std::vector<std::string> santosSpIpls;
	//std::vector<std::string> santosMpIpls;
	//std::vector<std::string> santosMpForcedIpls;
	IplGroup santosIplGroup;
	IplGroup santosSpIplGroup;
	IplGroup santosMpIplGroup;
	IplGroup santosMpForcedIplGroup;
	//std::vector<std::string> santosPeds;
	//std::vector<std::string> santosVehicles;
	ZonedModelGroup santosPedGroup;
	ZonedModelGroup santosVehicleGroup;
	//std::vector<std::string> santosRadio;
	RadioStationGroup santosRadioGroup;
	//std::vector<std::string> yanktonZones;
	ZoneGroup yanktonZoneGroup;
	//std::vector<std::string> yanktonAmbientZones;
	AmbientZoneGroup yanktonAmbientZoneGroup;
	//std::vector<std::string> yanktonIpls;
	IplGroup yanktonIplGroup;
	//std::vector<std::string> cayoScenarios;
	ScenarioGroup cayoScenarioGroup;
	//std::vector<std::string> cayoZones;
	ZoneGroup cayoZoneGroup;
	//std::vector<std::string> cayoAmbientZones;
	AmbientZoneGroup cayoAmbientZoneGroup;
	//std::vector<std::string> cayoMpIpls;
	IplGroup cayoMpIplGroup;
	//std::vector<std::string> modIpls;
	IplGroup modIplGroup;
	//std::vector<bool> santosIplState;
	//std::vector<bool> santosSpIplState;
	//std::vector<bool> santosMpIplState;
	//std::vector<bool> santosScenariosState;
	//std::vector<bool> santosAmbientZonesState;

	//WeatherTypes
	/*std::vector<std::string> santosWeatherTypes;
	std::vector<std::string> libertyWeatherTypes;
	std::vector<std::string> yanktonWeatherTypes;
	std::vector<std::string> cayoWeatherTypes;*/
	WeatherTypes santosWeatherTypes;
	WeatherTypes libertyWeatherTypes;
	WeatherTypes yanktonWeatherTypes;
	WeatherTypes cayoWeatherTypes;
	/*std::vector<std::string> weatherTypes =
	{
		"CLEAR",
		"EXTRASUNNY",
		"CLOUDS",
		"OVERCAST",
		"RAIN",
		"CLEARING",
		"THUNDER",
		"SMOG",
		"CLEAR",
		"EXTRASUNNY",
		"CLOUDS",
		"SMOG"
	};
	std::vector<std::string> yanktonWeatherTypes =
	{
		"CLEAR",
		"EXTRASUNNY",
		"CLOUDS",
		"OVERCAST",
		"EXTRASUNNY",
		"XMAS",
		"SNOW",
		"SNOWLIGHT",
		"BLIZZARD",
		"XMAS",
		"SNOW",
		"SNOWLIGHT",
		"BLIZZARD"
	};*/

	// Level Objects
	Level losSantos;
	Level northYankton;
	Level cayoPerico;
	Level libertyCity;
	Level santaFuegos;

	int LCEnabled = 0;
	//int playerLocationID = 0; // 0 = LS    1 = LC    2 = NY    3 = CP

	// Player Objects
	Player player;
	Ped playerPed;
	Vehicle playerVehicle;
	Vehicle rentalVehicle;
	Hash rentalVehicleHash;
	BOOL bPlayerExists;
	Vector3 PlayerPosition;

	// Blip Objects
	Blip AirportBlip;
	Blip DocksBlip;
	Blip FlyToLCBlip;
	Blip FlyToLSBlip;
	Blip FlyToCPBlip;
	bool airportBlipVisible = false;
	bool docksBlipVisible = false;
	bool flyToLCBlipVisible = false;
	bool flyToLSBlipVisible = false;
	bool flyToCPBlipVisible = false;
	bool airportBlipFarDist = false;

	// Marker Cords
	std::vector<float> LibertyCityOffsetPos =		{ 5188.185f, -3256.298f, 0.0f };
	std::vector<float> LosSantosIntAirport =		{ -1050.53f, -2741.48f, 14.60f };
	std::vector<float> LosSantosIntAirportArrival = { -1037.446f, -2737.821f, 20.168f };
	std::vector<float> LosSantosDocks =				{ 1170.40f, -2973.52f, 5.13f };
	std::vector<float> LosSantosDocksArrival =		{ 1143.96f, -2953.18f, 5.13f };
	std::vector<float> FrancisIntAirport =			{ 7553.86f, -2879.39f, 6.08f };
	std::vector<float> FrancisIntAirportArrival =	{ 7552.72f, -2851.3f, 6.08f };
	std::vector<float> LibertyCityDocks =			{ 5935.99f, -3531.89f, 5.05f };
	std::vector<float> LibertyCityDocksArrival =	{ 5941.92f, -3481.14f, 5.05f };
	std::vector<float> NorthYanktonAirport =		{ 15873.73f, -5158.65f, 83.85f };
	std::vector<float> NorthYanktonAirportArrival = { 5550.18f, -5131.62f, 78.55f };
	std::vector<float> CayoPericoAirport =			{ 4496.84f, -4521.04f, 4.41f };
	std::vector<float> TempLocation =				{ 0.0f, 0.0f, 100.0f };

	// Bounds
	float LibertyCityNorthBound = 1200.0f;
	float LibertyCitySouthBound = -8000.0f;
	float LibertyCityEastBound = 9900.0f;
	float LibertyCityWestBound = 1600.0f;

	float CayoPericoNorthBound = -3150.0f;
	float CayoPericoSouthBound = -7150.0f;
	float CayoPericoEastBound = 6700.0f;
	float CayoPericoWestBound = 2700.0f;
	float CayoPericoMPDestHeading = 35.0f;

	// Flight Trigger Cords
	std::vector<float> FlyToLCFromLSCoords =	{ 9500.0f, 9500.0f, 0.0f };
	std::vector<float> FlyToLSFromLCCoords =	{ -9500.0f, -9500.0f, 0.0f };
	std::vector<float> FlyToCPFromLSCoords =	{ 9500.0f, -9500.0f, 0.0f };
	std::vector<float> FlyToLCFromCPCoords =	{ -4000.0f, 9500.0f, 0.0f };
	std::vector<float> FlyToCPFromLCCoordsMP =	{ 3700.0f, -3200.0f, 0.0f };
	std::vector<float> FlyToLSFromCPCoords =	{ -9500.0f, 9500.0f, 0.0f };
	std::vector<float> FlyToCPFromLSCoordsMP =	{ 2750.0f, -3200.0f, 0.0f };
	std::vector<float> FlyToCPFromLCCoords =	{ 10500.0f, -13500.0f, 0.0f };

	// Menu Options
	std::vector<string> santosAirportOptions = { "(FIA)Liberty City, Liberty State [ON TIME]",
													"(NYA)Ludendorff, North Yankton [ON TIME]",
													"(VCI)Vice City, Leonida [DELAYED]",
													"(EBA)San Fierro, San Andreas [CANCELED]",
													"(LVA)Las Venturas, San Andreas [DELAYED]",
													"(CIA)Carcer City, Dolmen State [CANCELED]",
													"(PIA)Cottonmouth, Aloisia [CANCELED]",
													"(LIA)Windy Bay, Prairie [CANCELED]" };
	std::vector<bool> santosAirportOptionsAvalible = { true,
														true,
														false,
														false,
														false,
														false,
														false,
														false };

	std::vector<string> libertyAirportOptions = { "(LSX)Los Santos, San Andreas [ON TIME]",
													"(NYA)Ludendorff, North Yankton [ON TIME]",
													"(VCI)Vice City, Leonida [DELAYED]",
													"(EBA)San Fierro, San Andreas [CANCELED]",
													"(LVA)Las Venturas, San Andreas [DELAYED]",
													"(CIA)Carcer City, Dolmen State [CANCELED]",
													"(PIA)Cottonmouth, Aloisia [CANCELED]",
													"(LIA)Windy Bay, Prairie [CANCELED]" };
	std::vector<bool> libertyAirportOptionsAvalible = { true,
														true,
														false,
														false,
														false,
														false,
														false,
														false };

	std::vector<string> yanktonAirportOptions = { "(LSX)Los Santos, San Andreas [ON TIME]",
													"(FIA)Liberty City, Liberty State [ON TIME]",
													"(VCI)Vice City, Leonida [DELAYED]",
													"(EBA)San Fierro, San Andreas [CANCELED]",
													"(LVA)Las Venturas, San Andreas [DELAYED]",
													"(CIA)Carcer City, Dolmen State [CANCELED]",
													"(PIA)Cottonmouth, Aloisia [CANCELED]",
													"(LIA)Windy Bay, Prairie [CANCELED]" };
	std::vector<bool> yanktonAirportOptionsAvalible = { true,
														true,
														false,
														false,
														false,
														false,
														false,
														false };
	
	std::vector<string> teleportOptions = { "Los Santos",
											"North Yankton",
											"Cayo Perico",
											"Liberty City",
											"Santa Fuegos"};
	std::vector<bool> teleportOptionsAvalible = { true,
													true,
													true,
													true,
													true};

	void readFileToVector(const std::string& filePath, std::vector<std::string>& targetVector) {
		std::ifstream file(filePath);
		std::string line;

		if (file.is_open()) {
			while (std::getline(file, line)) {
				if (!line.empty()) {
					targetVector.push_back(line);
				}
			}
			file.close();
		}
	}

	// Generalized function for all locations
	void readCityFiles(const std::string& city, const std::unordered_map<std::string, std::vector<std::string>*>& fileMap) {
		for (const auto& [fileSuffix, targetVector] : fileMap) {
			std::string filePath = "./Levels/" + city + "/" + fileSuffix + ".txt";
			readFileToVector(filePath, *targetVector);
		}
	}

	// Location specific readers
	void readLSFiles() {
		/*std::unordered_map<std::string, std::vector<std::string>*> lsFiles = {
			{"Scenarios", &santosScenarios},
			{"Zones", &santosZones},
			{"AmbientZones", &santosAmbientZones},
			{"IPLs", &santosIpls},
			{"IPLsSP", &santosSpIpls},
			{"IPLsMP", &santosMpIpls},
			{"IPLsMPForced", &santosMpForcedIpls},
			{"ZonedPeds", &santosPeds},
			{"ZonedVehicles", &santosVehicles},
			{"ZonedRadioStations", &santosRadio},
			{"WeatherTypes", &santosWeatherTypes},
			{"Mods/ModIPLs", &modIpls}
		};
		readCityFiles("Los Santos", lsFiles);*/
		santosScenarioGroup.ReadFile("./Levels/Los Santos/Scenarios.txt");
		santosZoneGroup.ReadFile("./Levels/Los Santos/Zones.txt");
		santosAmbientZoneGroup.ReadFile("./Levels/Los Santos/AmbientZones.txt");
		santosIplGroup.ReadFile("./Levels/Los Santos/IPLs.txt");
		santosSpIplGroup.ReadFile("./Levels/Los Santos/IPLsSP.txt");
		santosMpIplGroup.ReadFile("./Levels/Los Santos/IPLsMP.txt");
		santosMpForcedIplGroup.ReadFile("./Levels/Los Santos/IPLsMPForced.txt");
		santosPedGroup.ReadFile("./Levels/Los Santos/ZonedPeds.txt");
		santosVehicleGroup.ReadFile("./Levels/Los Santos/ZonedVehicles.txt");
		santosRadioGroup.ReadFile("./Levels/Los Santos/ZonedRadioStations.txt");
		santosWeatherTypes.ReadFile("./Levels/Los Santos/WeatherTypes.txt");
		modIplGroup.ReadFile("./Levels/Los Santos/ModIPLs.txt");
	}

	void readNYFiles() {
		/*std::unordered_map<std::string, std::vector<std::string>*> nyFiles = {
			{"Zones", &yanktonZones},
			{"AmbientZones", &yanktonAmbientZones},
			{"IPLs", &yanktonIpls},
			{"WeatherTypes", &yanktonWeatherTypes}
		};
		readCityFiles("North Yankton", nyFiles);*/
		yanktonZoneGroup.ReadFile("./Levels/North Yankton/Zones.txt");
		yanktonAmbientZoneGroup.ReadFile("./Levels/North Yankton/AmbientZones.txt");
		yanktonIplGroup.ReadFile("./Levels/North Yankton/IPLs.txt");
		yanktonWeatherTypes.ReadFile("./Levels/North Yankton/WeatherTypes.txt");
	}

	void readCPFiles() {
		/*std::unordered_map<std::string, std::vector<std::string>*> cpFiles = {
			{"Scenarios", &cayoScenarios},
			{"Zones", &cayoZones},
			{"AmbientZones", &cayoAmbientZones},
			{"IPLsMP", &cayoMpIpls},
			{"WeatherTypes", &cayoWeatherTypes}
		};
		readCityFiles("Cayo Perico", cpFiles);*/
		cayoScenarioGroup.ReadFile("./Levels/Cayo Perico/Scenarios.txt");
		cayoZoneGroup.ReadFile("./Levels/Cayo Perico/Zones.txt");
		cayoAmbientZoneGroup.ReadFile("./Levels/Cayo Perico/AmbientZones.txt");
		cayoMpIplGroup.ReadFile("./Levels/Cayo Perico/IPLsMP.txt");
		cayoWeatherTypes.ReadFile("./Levels/Cayo Perico/WeatherTypes.txt");
	}

	void readLCFiles() {
		/*std::unordered_map<std::string, std::vector<std::string>*> lcFiles = {
			{"Scenarios", &libertyScenarios},
			{"Zones", &libertyZones},
			{"AmbientZones", &libertyAmbientZones},
			{"IPLs", &libertyIpls},
			{"IPLsSP", &libertySpIpls},
			{"IPLsMP", &libertyMpIpls},
			{"IPLsLODLights", &libertyLODLightIpls},
			{"ZonedPeds", &libertyPeds},
			{"ZonedVehicles", &libertyVehicles},
			{"ZonedRadioStations", &libertyRadio},
			{"WeatherTypes", &libertyWeatherTypes}
		};
		readCityFiles("Liberty City", lcFiles);*/
		libertyScenarioGroup.ReadFile("./Levels/Liberty City/Scenarios.txt");
		libertyZoneGroup.ReadFile("./Levels/Liberty City/Zones.txt");
		libertyAmbientZoneGroup.ReadFile("./Levels/Liberty City/AmbientZones.txt");
		libertyIplGroup.ReadFile("./Levels/Liberty City/IPLs.txt");
		libertySpIplGroup.ReadFile("./Levels/Liberty City/IPLsSP.txt");
		libertyMpIplGroup.ReadFile("./Levels/Liberty City/IPLsMP.txt");
		libertyLODLightIplGroup.ReadFile("./Levels/Liberty City/IPLsLODLights.txt");
		libertyPedGroup.ReadFile("./Levels/Liberty City/ZonedPeds.txt");
		libertyVehicleGroup.ReadFile("./Levels/Liberty City/ZonedVehicles.txt");
		libertyRadioGroup.ReadFile("./Levels/Liberty City/ZonedRadioStations.txt");
		libertyWeatherTypes.ReadFile("./Levels/Liberty City/WeatherTypes.txt");
	}


	void RequestScript(PCHAR scriptName, int stackSize)
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

	// Initialize Liberty City
	void initialize()
	{
		//readLSFiles();
		//readLCFiles();
		//readNYFiles();
		//readCPFiles();
		losSantos.SetProperties("Los Santos", 0, 0, 0, 0, true, false);
		northYankton.SetProperties("North Yankton", 2, 1, 0, 1, false, false);
		cayoPerico.SetProperties("Cayo Perico", 3, 2, 1, 2, false, true);
		libertyCity.SetProperties("Liberty City", 1, 1, 2, 3, false, false);
		libertyCity.loadLodLights = loadLibertyLODLights;
		santaFuegos.SetProperties("Santa Fuegos", 4, 0, 3, 0, false, false);

		libertyCity.DisableActiveFiles();
		santaFuegos.DisableActiveFiles();
		losSantos.EnableRadios();
		cayoPerico.DisableZoneGroup();
		/*for (int i = 0; i < libertyScenarios.size(); i++)
		{
			AI::SET_SCENARIO_GROUP_ENABLED((char*)libertyScenarios[i].c_str(), false);
		}*/
		//libertyScenarioGroup.RemoveScenarioGroupIfActive();

		/*for (int i = 0; i < libertyZones.size(); i++)
		{
			int zoneId = ZONE::GET_ZONE_FROM_NAME_ID((char*)libertyZones[i].c_str());
			ZONE::SET_ZONE_ENABLED(zoneId, false);
		}*/
		//libertyZoneGroup.DisableZoneGroup();

		/*for (int i = 0; i < libertyAmbientZones.size(); i++)
		{
			AUDIO::SET_AMBIENT_ZONE_STATE_PERSISTENT(const_cast<char*>(libertyAmbientZones[i].c_str()), false, true);
		}*/
		//libertyAmbientZoneGroup.RemoveAmbientZoneGroupIfActive();

		/*for (int i = 0; i < libertyPeds.size(); i++)
		{
			PED::SET_PED_MODEL_IS_SUPPRESSED(GAMEPLAY::GET_HASH_KEY((char*)libertyPeds[i].c_str()), true);
		}

		for (int i = 0; i < libertyVehicles.size(); i++)
		{
			VEHICLE::SET_VEHICLE_MODEL_IS_SUPPRESSED(GAMEPLAY::GET_HASH_KEY((char*)libertyVehicles[i].c_str()), true);
		}*/

		//libertyPedGroup.SuppressZonedModels(true, false);
		//libertyVehicleGroup.SuppressZonedModels(true, true);

		/*for (int i = 0; i < libertyRadio.size(); i++)
		{
			AUDIO::LOCK_RADIO_STATION((char*)libertyRadio[i].c_str(), true);
		}

		for (int i = 0; i < santosRadio.size(); i++)
		{
			AUDIO::LOCK_RADIO_STATION((char*)santosRadio[i].c_str(), false);
		}*/
		//libertyRadioGroup.DisableRadioStationGroup();
		//santosRadioGroup.EnableRadioStationGroup();

		/*for (int i = 0; i < cayoZones.size(); i++)
		{
			int zoneId = ZONE::GET_ZONE_FROM_NAME_ID((char*)cayoZones[i].c_str());
			ZONE::SET_ZONE_ENABLED(zoneId, false);
		}*/
		//cayoZoneGroup.DisableZoneGroup();

		if (restrictedAreaClearance)
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"restrictedareas");		   // responsible for restricted areas
	}

	void CreateBlips()
	{
		if (!UI::DOES_BLIP_EXIST(AirportBlip))
		{
			if (worldtravel::IsLosSantos())
				AirportBlip = UI::ADD_BLIP_FOR_COORD(LosSantosIntAirport[0], LosSantosIntAirport[1], LosSantosIntAirport[2]);

			else if (worldtravel::IsLibertyCity())
				AirportBlip = UI::ADD_BLIP_FOR_COORD(FrancisIntAirport[0], FrancisIntAirport[1], FrancisIntAirport[2]);

			else if (worldtravel::IsNorthYankton())
				AirportBlip = UI::ADD_BLIP_FOR_COORD(NorthYanktonAirport[0], NorthYanktonAirport[1], NorthYanktonAirport[2]);

			UI::SET_BLIP_SCALE(AirportBlip, 1);
			UI::SET_BLIP_SPRITE(AirportBlip, 90);
			UI::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)"Airport");
			UI::END_TEXT_COMMAND_SET_BLIP_NAME(AirportBlip);
			UI::SET_BLIP_COLOUR(AirportBlip, 0);
			UI::SET_BLIP_FLASHES(AirportBlip, false);

			if (!worldtravel::IsCayoPerico())
				UI::SET_BLIP_DISPLAY(AirportBlip, 2);
			else
				UI::SET_BLIP_DISPLAY(AirportBlip, 0);
			UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, true);
			UI::SET_BLIP_CATEGORY(AirportBlip, 1);
		}

		if (!UI::DOES_BLIP_EXIST(DocksBlip))
		{
			if (worldtravel::IsLosSantos())
				DocksBlip = UI::ADD_BLIP_FOR_COORD(LosSantosDocks[0], LosSantosDocks[1], LosSantosDocks[2]);

			else if (worldtravel::IsLibertyCity())
				DocksBlip = UI::ADD_BLIP_FOR_COORD(LibertyCityDocks[0], LibertyCityDocks[1], LibertyCityDocks[2]);

			UI::SET_BLIP_SCALE(DocksBlip, 1);
			UI::SET_BLIP_SPRITE(DocksBlip, 677);
			UI::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)"Docks");
			UI::END_TEXT_COMMAND_SET_BLIP_NAME(DocksBlip);
			UI::SET_BLIP_COLOUR(DocksBlip, 0);
			UI::SET_BLIP_FLASHES(DocksBlip, false);

			if (worldtravel::IsLosSantos() || worldtravel::IsLibertyCity())
				UI::SET_BLIP_DISPLAY(DocksBlip, 2);
			else
				UI::SET_BLIP_DISPLAY(DocksBlip, 0);
			UI::SET_BLIP_AS_SHORT_RANGE(DocksBlip, true);
			UI::SET_BLIP_CATEGORY(DocksBlip, 1);
		}

		if (!UI::DOES_BLIP_EXIST(FlyToLCBlip))
		{
			if (worldtravel::IsLosSantos())
				FlyToLCBlip = UI::ADD_BLIP_FOR_COORD(FlyToLCFromLSCoords[0], FlyToLCFromLSCoords[1], 1200.0f);

			else if (worldtravel::IsCayoPerico())
				FlyToLCBlip = UI::ADD_BLIP_FOR_COORD(FlyToLCFromCPCoords[0], FlyToLCFromCPCoords[1], 1200.0f);

			else
				FlyToLCBlip = UI::ADD_BLIP_FOR_COORD(0.0f, 0.0f, 1200.0f);

			UI::SET_BLIP_SCALE(FlyToLCBlip, 1);
			UI::SET_BLIP_SPRITE(FlyToLCBlip, 307);
			UI::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)"Fly to Liberty City");
			UI::END_TEXT_COMMAND_SET_BLIP_NAME(FlyToLCBlip);
			UI::SET_BLIP_COLOUR(FlyToLCBlip, 0);
			UI::SET_BLIP_FLASHES(FlyToLCBlip, false);
			flyToLCBlipVisible = false;
			UI::SET_BLIP_DISPLAY(FlyToLCBlip, 0);
			UI::SET_BLIP_AS_SHORT_RANGE(FlyToLCBlip, false);
			UI::SET_BLIP_CATEGORY(FlyToLCBlip, 1);
		}

		if (!UI::DOES_BLIP_EXIST(FlyToLSBlip))
		{
			if (worldtravel::IsLibertyCity())
				FlyToLSBlip = UI::ADD_BLIP_FOR_COORD(FlyToLSFromLCCoords[0], FlyToLSFromLCCoords[1], 1200.0f);

			else if (worldtravel::IsCayoPerico())
				FlyToLSBlip = UI::ADD_BLIP_FOR_COORD(FlyToLSFromCPCoords[0], FlyToLSFromCPCoords[1], 1200.0f);

			else
				FlyToLSBlip = UI::ADD_BLIP_FOR_COORD(0.0f, 0.0f, 1200.0f);

			UI::SET_BLIP_SCALE(FlyToLSBlip, 1);
			UI::SET_BLIP_SPRITE(FlyToLSBlip, 307);
			UI::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)"Fly to Los Santos");
			UI::END_TEXT_COMMAND_SET_BLIP_NAME(FlyToLSBlip);
			UI::SET_BLIP_COLOUR(FlyToLSBlip, 0);
			UI::SET_BLIP_FLASHES(FlyToLSBlip, false);
			flyToLSBlipVisible = false;
			UI::SET_BLIP_DISPLAY(FlyToLSBlip, 0);
			UI::SET_BLIP_AS_SHORT_RANGE(FlyToLSBlip, false);
			UI::SET_BLIP_CATEGORY(FlyToLSBlip, 1);
		}

		if (!UI::DOES_BLIP_EXIST(FlyToCPBlip))
		{
			if (worldtravel::IsLosSantos())
				FlyToCPBlip = UI::ADD_BLIP_FOR_COORD(FlyToCPFromLSCoords[0], FlyToCPFromLSCoords[1], 1200.0f);

			else if (worldtravel::IsLibertyCity())
				FlyToCPBlip = UI::ADD_BLIP_FOR_COORD(FlyToCPFromLCCoords[0], FlyToCPFromLCCoords[1], 1200.0f);

			else
				FlyToCPBlip = UI::ADD_BLIP_FOR_COORD(0.0f, 0.0f, 1200.0f);

			UI::SET_BLIP_SCALE(FlyToCPBlip, 1);
			UI::SET_BLIP_SPRITE(FlyToCPBlip, 307);
			UI::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)"Fly to Cayo Perico");
			UI::END_TEXT_COMMAND_SET_BLIP_NAME(FlyToCPBlip);
			UI::SET_BLIP_COLOUR(FlyToCPBlip, 0);
			UI::SET_BLIP_FLASHES(FlyToCPBlip, false);
			flyToCPBlipVisible = false;
			UI::SET_BLIP_DISPLAY(FlyToCPBlip, 0);
			UI::SET_BLIP_AS_SHORT_RANGE(FlyToCPBlip, false);
			UI::SET_BLIP_CATEGORY(FlyToCPBlip, 1);
		}
	}

	// Fix to force disable IPLs in MP
	void KeepLosSantosIplsDisabled()
	{
		if (!worldtravel::IsLosSantos())
		{
			if (worldtravel::MpMap::IsMPMapActive() && NETWORK::NETWORK_IS_IN_SESSION())
			{
				/*for (int i = 0; i < santosMpForcedIpls.size(); i++)
				{
					if (STREAMING::IS_IPL_ACTIVE((char*)santosMpForcedIpls[i].c_str()))
						STREAMING::REMOVE_IPL((char*)santosMpForcedIpls[i].c_str());
				}*/
				//santosMpForcedIplGroup.RemoveIplGroupIfActive();
				losSantos.RemoveForcedIpls();
			}
		}
	}

	// Controls time of day
	void NetworkClockController(bool forward)
	{
		int hours = TIME::GET_CLOCK_HOURS();
		int minutes = TIME::GET_CLOCK_MINUTES();
		int seconds = TIME::GET_CLOCK_SECONDS();
		if (forward)
		{
			hours += 12;
			if (hours > 23)
			{
				hours -= 24;
			}
		}
		else
		{
			hours -= 12;
			if (hours < 0)
			{
				hours += 24;
			}
		}
		NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(hours, minutes, seconds);
	}


	// handles blip visibility for all maps
	void BlipVisibilityController()
	{
		Entity player = playerPed;
		if (worldtravel::IsLosSantos())
		{
			if (!airportBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(AirportBlip, 2);
				airportBlipVisible = true;
			}
			if (!docksBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(DocksBlip, 2);
				docksBlipVisible = true;
			}
			if (PED::IS_PED_IN_FLYING_VEHICLE(player))
			{
				if (!flyToLCBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLCBlip, 2);
					flyToLCBlipVisible = true;
				}
				if (!flyToCPBlipVisible)
				{
					if (!NETWORK::NETWORK_IS_IN_SESSION())
					{
						UI::SET_BLIP_DISPLAY(FlyToCPBlip, 2);
						flyToCPBlipVisible = true;
					}
				}
				if (!airportBlipFarDist)
				{
					UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, false);
					airportBlipFarDist = true;
				}
			}
			else
			{
				if (flyToLCBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLCBlip, 0);
					flyToLCBlipVisible = false;
				}
				if (flyToCPBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToCPBlip, 0);
					flyToCPBlipVisible = false;
				}
				if (airportBlipFarDist)
				{
					UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, true);
					airportBlipFarDist = false;
				}
			}
			if (flyToLSBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToLSBlip, 0);
				flyToLSBlipVisible = false;
			}
		}
		else if (worldtravel::IsLibertyCity())
		{
			if (!airportBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(AirportBlip, 2);
				airportBlipVisible = true;
			}
			if (!docksBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(DocksBlip, 2);
				docksBlipVisible = true;
			}
			if (PED::IS_PED_IN_FLYING_VEHICLE(player))
			{
				if (!flyToLSBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLSBlip, 2);
					flyToLSBlipVisible = true;
				}
				if (!flyToCPBlipVisible)
				{
					if (!NETWORK::NETWORK_IS_IN_SESSION())
					{
						UI::SET_BLIP_DISPLAY(FlyToCPBlip, 2);
						flyToCPBlipVisible = true;
					}
				}
				if (!airportBlipFarDist)
				{
					UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, false);
					airportBlipFarDist = true;
				}
			}
			else
			{
				if (flyToLSBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLSBlip, 0);
					flyToLSBlipVisible = false;
				}
				if (flyToCPBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToCPBlip, 0);
					flyToCPBlipVisible = false;
				}
				if (airportBlipFarDist)
				{
					UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, true);
					airportBlipFarDist = false;
				}
			}
			if (flyToLCBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToLCBlip, 0);
				flyToLCBlipVisible = false;
			}
		}
		else if (worldtravel::IsNorthYankton())
		{
			if (!airportBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(AirportBlip, 2);
				airportBlipVisible = true;
			}
			if (docksBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(DocksBlip, 0);
				docksBlipVisible = false;
			}
			if (flyToLSBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToLSBlip, 0);
				flyToLSBlipVisible = false;
			}
			if (flyToLCBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToLCBlip, 0);
				flyToLCBlipVisible = false;
			}
			if (flyToCPBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToCPBlip, 0);
				flyToCPBlipVisible = false;
			}
			if (!airportBlipFarDist)
			{
				UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, false);
				airportBlipFarDist = true;
			}
		}
		else if (worldtravel::IsCayoPerico())
		{
			if (!airportBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(AirportBlip, 2);
				airportBlipVisible = true;
			}
			if (docksBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(DocksBlip, 0);
				docksBlipVisible = false;
			}
			if (PED::IS_PED_IN_FLYING_VEHICLE(player))
			{
				if (!flyToLSBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLSBlip, 2);
					flyToLSBlipVisible = true;
				}
				if (!flyToLCBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLCBlip, 2);
					flyToLCBlipVisible = true;
				}
				if (!airportBlipFarDist)
				{
					UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, false);
					airportBlipFarDist = true;
				}
			}
			else
			{
				if (flyToLSBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLSBlip, 0);
					flyToLSBlipVisible = false;
				}
				if (flyToLCBlipVisible)
				{
					UI::SET_BLIP_DISPLAY(FlyToLCBlip, 0);
					flyToLCBlipVisible = false;
				}
				if (airportBlipFarDist)
				{
					UI::SET_BLIP_AS_SHORT_RANGE(AirportBlip, true);
					airportBlipFarDist = false;
				}
			}
			if (flyToCPBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToCPBlip, 0);
				flyToCPBlipVisible = false;
			}
		}
		else if (worldtravel::GetPlayerLocationID() == 4)
		{

			if (!airportBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(AirportBlip, 0);
				airportBlipVisible = false;
			}
			if (docksBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(DocksBlip, 0);
				docksBlipVisible = false;
			}
			if (flyToLSBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToLSBlip, 0);
				flyToLSBlipVisible = false;
			}
			if (flyToLCBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToLCBlip, 0);
				flyToLCBlipVisible = false;
			}
			if (flyToCPBlipVisible)
			{
				UI::SET_BLIP_DISPLAY(FlyToCPBlip, 0);
				flyToCPBlipVisible = false;
			}
		}
	}


	void SetBlipsLocation(int blipLocationID)
	{
		if (blipLocationID == 0)
		{
			UI::SET_BLIP_COORDS(AirportBlip, LosSantosIntAirport[0], LosSantosIntAirport[1], LosSantosIntAirport[2]);
			UI::SET_BLIP_COORDS(DocksBlip, LosSantosDocks[0], LosSantosDocks[1], LosSantosDocks[2]);
			UI::SET_BLIP_COORDS(FlyToLCBlip, FlyToLCFromLSCoords[0], FlyToLCFromLSCoords[1], 1200.0f);
			UI::SET_BLIP_COORDS(FlyToCPBlip, FlyToCPFromLSCoords[0], FlyToCPFromLSCoords[1], 1200.0f);
			UI::SET_BLIP_COORDS(FlyToLSBlip, 0.0f, 0.0f, 1200.0f);
		}
		else if (blipLocationID == 1)
		{
			UI::SET_BLIP_COORDS(AirportBlip, FrancisIntAirport[0], FrancisIntAirport[1], FrancisIntAirport[2]);
			UI::SET_BLIP_COORDS(DocksBlip, LibertyCityDocks[0], LibertyCityDocks[1], LibertyCityDocks[2]);
			UI::SET_BLIP_COORDS(FlyToLSBlip, FlyToLSFromLCCoords[0], FlyToLSFromLCCoords[1], 1200.0f);
			UI::SET_BLIP_COORDS(FlyToCPBlip, FlyToCPFromLCCoords[0], FlyToCPFromLCCoords[1], 1200.0f);
			UI::SET_BLIP_COORDS(FlyToLCBlip, 0.0f, 0.0f, 1200.0f);
		}
		else if (blipLocationID == 2)
		{
			UI::SET_BLIP_COORDS(AirportBlip, NorthYanktonAirport[0], NorthYanktonAirport[1], NorthYanktonAirport[2]);
			UI::SET_BLIP_COORDS(DocksBlip, 0.0f, 0.0f, 0.0f);
			UI::SET_BLIP_COORDS(FlyToLSBlip, 0.0f, 0.0f, 1200.0f);
			UI::SET_BLIP_COORDS(FlyToCPBlip, 0.0f, 0.0f, 1200.0f);
			UI::SET_BLIP_COORDS(FlyToLCBlip, 0.0f, 0.0f, 1200.0f);
		}
		else if (blipLocationID == 3)
		{
			UI::SET_BLIP_COORDS(AirportBlip, CayoPericoAirport[0], CayoPericoAirport[1], CayoPericoAirport[2]);
			UI::SET_BLIP_COORDS(DocksBlip, 0.0f, 0.0f, 0.0f);
			UI::SET_BLIP_COORDS(FlyToLSBlip, FlyToLSFromCPCoords[0], FlyToLSFromCPCoords[1], 1200.0f);
			UI::SET_BLIP_COORDS(FlyToLCBlip, FlyToLCFromCPCoords[0], FlyToLCFromCPCoords[1], 1200.0f);
			UI::SET_BLIP_COORDS(FlyToCPBlip, 0.0f, 0.0f, 1200.0f);
		}
		else if (blipLocationID == 4)
		{
			UI::SET_BLIP_COORDS(AirportBlip, 0.0f, 0.0f, 0.0f);
			UI::SET_BLIP_COORDS(DocksBlip, 0.0f, 0.0f, 0.0f);
			UI::SET_BLIP_COORDS(FlyToLSBlip, 0.0f, 0.0f, 0.0f);
			UI::SET_BLIP_COORDS(FlyToLCBlip, 0.0f, 0.0f, 0.0f);
			UI::SET_BLIP_COORDS(FlyToCPBlip, 0.0f, 0.0f, 0.0f);
		}
	}



	/////////////////////////////////////////////////////////
	////             Loading Helper Functions            ////
	/////////////////////////////////////////////////////////


	void requestIpls(const std::vector<std::string>& iplList)
	{
		for (const auto& ipl : iplList)
		{
			STREAMING::REQUEST_IPL(const_cast<char*>(ipl.c_str()));
		}
	}

	void requestIpls(const std::vector<std::string>& iplList, std::vector<bool>& iplState)
	{
		int index = 0;
		for (const auto& ipl : iplList)
		{
			if (iplState[index])
				STREAMING::REQUEST_IPL(const_cast<char*>(ipl.c_str()));
			index++;
		}
	}

	void enableScenarios(const std::vector<std::string>& scenarioList)
	{
		for (const auto& scenario : scenarioList)
		{
			AI::SET_SCENARIO_GROUP_ENABLED(const_cast<char*>(scenario.c_str()), true);
		}
	}

	void enableScenarios(const std::vector<std::string>& scenarioList, std::vector<bool>& scenarioState)
	{
		int index = 0;
		for (const auto& scenario : scenarioList)
		{
			if (scenarioState[index])
				AI::SET_SCENARIO_GROUP_ENABLED(const_cast<char*>(scenario.c_str()), true);
			index++;
		}
	}

	void enableZones(const std::vector<std::string>& zoneList)
	{
		for (const auto& zoneName : zoneList)
		{
			int zoneId = ZONE::GET_ZONE_FROM_NAME_ID(const_cast<char*>(zoneName.c_str()));
			ZONE::SET_ZONE_ENABLED(zoneId, true);
		}
	}

	void enableAmbientZones(const std::vector<std::string>& ambientZoneList)
	{
		for (const auto& ambientZoneName : ambientZoneList)
		{
			AUDIO::SET_AMBIENT_ZONE_STATE_PERSISTENT(const_cast<char*>(ambientZoneName.c_str()), true, true);
		}
	}

	void enableAmbientZones(const std::vector<std::string>& ambientZoneList, std::vector<bool>& ambientZoneState)
	{
		int index = 0;
		for (const auto& ambientZoneName : ambientZoneList)
		{
			if (ambientZoneState[index])
				AUDIO::SET_AMBIENT_ZONE_STATE_PERSISTENT(const_cast<char*>(ambientZoneName.c_str()), true, true);
			index++;
		}
	}

	void unlockModels(const std::vector<std::string>& modelList, bool isPed = true)
	{
		for (const auto& model : modelList)
		{
			if (isPed)
				PED::SET_PED_MODEL_IS_SUPPRESSED(GAMEPLAY::GET_HASH_KEY(const_cast<char*>(model.c_str())), false);
			else
				VEHICLE::SET_VEHICLE_MODEL_IS_SUPPRESSED(GAMEPLAY::GET_HASH_KEY(const_cast<char*>(model.c_str())), false);
		}
	}

	void unlockRadioStations(const std::vector<std::string>& radioList)
	{
		for (const auto& radioStation : radioList)
		{
			AUDIO::LOCK_RADIO_STATION(const_cast<char*>(radioStation.c_str()), false);
		}
	}

	void setTrainTracksAndFrequency(const std::vector<int>& trackIds, int frequency = 120000)
	{
		for (int trackId : trackIds)
		{
			VEHICLE::SWITCH_TRAIN_TRACK(trackId, true);
			VEHICLE::SET_TRAIN_TRACK_SPAWN_FREQUENCY(trackId, frequency);
		}
	}

	void loadMapLiberty(//const std::vector<std::string>& scenarios,
		//const std::vector<std::string>& zones,
		//const std::vector<std::string>& ambientZones,
		//const std::vector<std::string>& pedModels,
		//const std::vector<std::string>& vehicleModels,
		//const std::vector<std::string>& radioStations,
		const std::vector<int>& trainTracks,
		int trainFrequency = 120000)
	{
		//enableScenarios(scenarios);
		//enableZones(zones);
		//enableAmbientZones(ambientZones);
		//unlockModels(pedModels, true);
		//unlockModels(vehicleModels, false);
		//unlockRadioStations(radioStations);
		setTrainTracksAndFrequency(trainTracks, trainFrequency);
	}

	void loadMapSantos(//const std::vector<std::string>& scenarios,
		//const std::vector<std::string>& zones,
		//const std::vector<std::string>& ambientZones,
		//const std::vector<std::string>& pedModels,
		//const std::vector<std::string>& vehicleModels,
		//const std::vector<std::string>& radioStations,
		const std::vector<int>& trainTracks,
		//std::vector<bool>& scenarioState,
		//std::vector<bool>& ambientZoneState,
		int trainFrequency = 120000)
	{
		//enableScenarios(scenarios, scenarioState);
		//enableZones(zones);
		//enableAmbientZones(ambientZones, ambientZoneState);
		//unlockModels(pedModels, true);
		//unlockModels(vehicleModels, false);
		//unlockRadioStations(radioStations);
		setTrainTracksAndFrequency(trainTracks, trainFrequency);
	}


	/////////////////////////////////////////////////////////
	////             Loading Helper Functions            ////
	/////////////////////////////////////////////////////////

	// Load the Cayo Perico Island
	void loadCayo()
	{
		/*if (!worldtravel::MpMap::IsMPMapActive())
		{
			DLC2::_LOAD_MP_DLC_MAPS();
			while (DLC2::GET_IS_LOADING_SCREEN_ACTIVE())
			{
				WAIT(0); // avoid freezing the game
			}
			WAIT(1000); // 1 Second
			requestIpls(santosMpIpls);
		}*/
		/*if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			STREAMING::SET_ISLAND_ENABLED(const_cast<char*>("HeistIsland"), true);
		}

		//enableScenarios(cayoScenarios);
		cayoScenarioGroup.RequestScenarioGroup(false);
		//requestIpls(cayoMpIpls);
		cayoMpIplGroup.RequestIplGroup(false);
		//enableZones(cayoZones);
		cayoZoneGroup.EnableZoneGroup();
		//enableAmbientZones(cayoAmbientZones);
		cayoAmbientZoneGroup.RequestAmbientZoneGroup(false);

		UI::SET_USE_ISLAND_MAP(true);
		*/
		/*int weatherID = std::rand() % cayoWeatherTypes.size();
		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			GAMEPLAY::SET_WEATHER_TYPE_NOW(const_cast<char*>(cayoWeatherTypes[weatherID].c_str()));
		}
		else
		{
			GAMEPLAY::SET_WEATHER_TYPE_PERSIST(const_cast<char*>(cayoWeatherTypes[weatherID].c_str()));
		}*/
		/*cayoWeatherTypes.SetWeatherType();

		worldtravel::PathNodeState::SetPathNodeState(2);
		STREAMING::LOAD_GLOBAL_WATER_FILE(1);*/
		SetBlipsLocation(3);
	}

	// Load Liberty City
	void loadLiberty()
	{
		/*//STREAMING::SET_ISLAND_ENABLED(const_cast<char*>("LibertyCity"), true);
		

		
		//requestIpls(libertyIpls);
		libertyIplGroup.RequestIplGroup(false);
		// Request MP or SP IPLs based on session status
		if (worldtravel::MpMap::IsMPMapActive())
		{
			//requestIpls(libertyMpIpls);
			libertyMpIplGroup.RequestIplGroup(false);
		}
		else
		{
			//requestIpls(libertySpIpls);
			libertySpIplGroup.RequestIplGroup(false);
		}
		if (loadLibertyLODLights)
		{
			//requestIpls(libertyLODLightIpls);
			libertyLODLightIplGroup.RequestIplGroup(false);
		}

		libertyScenarioGroup.RequestScenarioGroup(false);
		libertyZoneGroup.EnableZoneGroup();
		libertyAmbientZoneGroup.RequestAmbientZoneGroup(false);
		libertyPedGroup.SuppressZonedModels(false, false);
		libertyVehicleGroup.SuppressZonedModels(false, true);
		libertyRadioGroup.EnableRadioStationGroup();*/
		//loadMapLiberty(libertyScenarios, libertyZones, libertyAmbientZones, libertyPeds, libertyVehicles, libertyRadio, { 12, 13, 14, 16, 17, 18 });
		loadMapLiberty({ 12, 13, 14, 16, 17, 18 });

		/*int weatherID = std::rand() % libertyWeatherTypes.size();
		GAMEPLAY::SET_OVERRIDE_WEATHER(const_cast<char*>(libertyWeatherTypes[weatherID].c_str()));*/
		/*libertyWeatherTypes.SetWeatherType();

		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			//GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(const_cast<char*>(libertyWeatherTypes[weatherID].c_str()));
			UI::_SET_MINIMAP_REVEALED(true);
		}
		else
		{
			//GAMEPLAY::SET_WEATHER_TYPE_PERSIST(const_cast<char*>(libertyWeatherTypes[weatherID].c_str()));
		}
		
		worldtravel::PathNodeState::SetPathNodeState(1);
		GRAPHICS::DISABLE_VEHICLE_DISTANTLIGHTS(true);
		STREAMING::LOAD_GLOBAL_WATER_FILE(2);*/
		SetBlipsLocation(1);
	}

	// Load Los Santos
	void loadSantos()
	{
		/*patches::farlods::SetVisible(true);

		//requestIpls(santosIpls, santosIplState);
		santosIplGroup.RequestIplGroup(true);
		// Request MP or SP IPLs based on session status
		if (worldtravel::MpMap::IsMPMapActive())
		{
			//requestIpls(santosMpIpls, santosMpIplState);
			santosMpIplGroup.RequestIplGroup(true);
		}
		else
		{
			//requestIpls(santosSpIpls, santosSpIplState);
			santosSpIplGroup.RequestIplGroup(true);
		}

		//requestIpls(modIpls);
		modIplGroup.RequestIplGroup(false);
		santosScenarioGroup.RequestScenarioGroup(true);
		santosZoneGroup.EnableZoneGroup();
		santosAmbientZoneGroup.RequestAmbientZoneGroup(true);
		santosPedGroup.SuppressZonedModels(false, false);
		santosVehicleGroup.SuppressZonedModels(false, true);
		santosRadioGroup.EnableRadioStationGroup();*/

		//loadMapSantos(santosScenarios, santosZones, santosAmbientZones, santosPeds, santosVehicles, santosRadio, { 0, 3 }, santosScenariosState, santosAmbientZonesState);
		loadMapSantos({ 0, 3 });
		/*worldtravel::PathNodeState::SetPathNodeState(0);
		GRAPHICS::DISABLE_VEHICLE_DISTANTLIGHTS(false);
		STREAMING::LOAD_GLOBAL_WATER_FILE(0);*/
		SetBlipsLocation(0);
		/*STREAMING::_SET_MAPDATACULLBOX_ENABLED((char*)"HeistIsland", false);

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
		}*/

		/*int weatherID = std::rand() % santosWeatherTypes.size();
		// if in multiplayer
		if (NETWORK::NETWORK_IS_IN_SESSION())
		{
			GAMEPLAY::SET_WEATHER_TYPE_NOW(const_cast<char*>(santosWeatherTypes[weatherID].c_str()));
			GAMEPLAY::CLEAR_WEATHER_TYPE_NOW_PERSIST_NETWORK(1);
		}
		else
		{
			//GAMEPLAY::SET_WEATHER_TYPE_PERSIST(const_cast<char*>(santosWeatherTypes[weatherID].c_str()));
			GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
		}

		GAMEPLAY::CLEAR_OVERRIDE_WEATHER();*/
		/*santosWeatherTypes.SetWeatherType();
		santosWeatherTypes.ResetWeatherType(true);

		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			// enable the following scripts in singleplayer that we disabled when not in LS
			RequestScript("blip_controller", 1424);        // responsible for blips
			RequestScript("forsalesigns", 1424);           // responsible for property for sale signs
			RequestScript("respawn_controller", 1424);     // responsible for respawning the player
			if (!restrictedAreaClearance)
				RequestScript("restrictedareas", 1424);        // responsible for restricted areas
			RequestScript("vehicle_gen_controller", 1424); // responsible for player businesses / vehicle blips
		}*/
	}

	// Load North Yankton
	void loadYankton()
	{
		/*//requestIpls(yanktonIpls);
		yanktonIplGroup.RequestIplGroup(false);
		//enableZones(yanktonZones);
		yanktonZoneGroup.EnableZoneGroup();
		//enableAmbientZones(yanktonAmbientZones);
		yanktonAmbientZoneGroup.RequestAmbientZoneGroup(false);
		UI::SET_MINIMAP_IN_PROLOGUE(true);*/

		/*int weatherID = std::rand() % yanktonWeatherTypes.size();
		GAMEPLAY::SET_OVERRIDE_WEATHER(const_cast<char*>(yanktonWeatherTypes[weatherID].c_str()));*/
		//yanktonWeatherTypes.SetWeatherType();
		// if in multiplayer
		/*if (NETWORK::NETWORK_IS_IN_SESSION())
		{
			GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(const_cast<char*>(yanktonWeatherTypes[weatherID].c_str()));
		}
		else
		{
			GAMEPLAY::SET_WEATHER_TYPE_PERSIST(const_cast<char*>(yanktonWeatherTypes[weatherID].c_str()));
		}*/
		/*PATHFIND::SET_ROADS_IN_ANGLED_AREA(5526.24f, -5137.23f, 61.78925f, 3679.327f, -4973.879f, 125.0828f, 192, false, true, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3691.211f, -4941.24f, 94.59368f, 3511.115f, -4869.191f, 126.7621f, 16, false, true, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3510.004f, -4865.81f, 94.69557f, 3204.424f, -4833.817f, 126.8152f, 16, false, true, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3186.534f, -4832.798f, 109.8148f, 3202.187f, -4833.993f, 114.815f, 16, false, true, true);
		worldtravel::PathNodeState::SetPathNodeState(1);*/
		SetBlipsLocation(2);
	}

	void loadFuegos()
	{
		SetBlipsLocation(4);
	}

	/////////////////////////////////////////////////////////
	////           Unloading Helper Functions            ////
	/////////////////////////////////////////////////////////

	void removeIpls(const std::vector<std::string>& ipls)
	{
		for (const auto& ipl : ipls)
		{
			STREAMING::REMOVE_IPL(const_cast<char*>(ipl.c_str()));
		}
	}

	void removeIpls(const std::vector<std::string>& ipls, std::vector<bool>& iplState)
	{
		for (const auto& ipl : ipls)
		{
			iplState.push_back(STREAMING::IS_IPL_ACTIVE(const_cast<char*>(ipl.c_str())));
			STREAMING::REMOVE_IPL(const_cast<char*>(ipl.c_str()));
		}
	}

	void disableScenarios(const std::vector<std::string>& scenarios)
	{
		for (const auto& scenario : scenarios)
		{
			AI::SET_SCENARIO_GROUP_ENABLED(const_cast<char*>(scenario.c_str()), false);
		}
	}

	void disableScenarios(const std::vector<std::string>& scenarios, std::vector<bool>& scenarioState)
	{
		for (const auto& scenario : scenarios)
		{
			scenarioState.push_back(AI::IS_SCENARIO_GROUP_ENABLED(const_cast<char*>(scenario.c_str())));
			AI::SET_SCENARIO_GROUP_ENABLED(const_cast<char*>(scenario.c_str()), false);
		}
	}

	void disableZones(const std::vector<std::string>& zones)
	{
		for (const auto& zoneName : zones)
		{
			int zoneId = ZONE::GET_ZONE_FROM_NAME_ID(const_cast<char*>(zoneName.c_str()));
			ZONE::SET_ZONE_ENABLED(zoneId, false);
		}
	}

	void disableAmbientZones(const std::vector<std::string>& ambientZoneList)
	{
		for (const auto& ambientZoneName : ambientZoneList)
		{
			AUDIO::SET_AMBIENT_ZONE_STATE_PERSISTENT(const_cast<char*>(ambientZoneName.c_str()), false, true);
		}
	}

	void disableAmbientZones(const std::vector<std::string>& ambientZoneList, std::vector<bool>& ambientZoneState)
	{
		int index = 0;
		for (const auto& ambientZoneName : ambientZoneList)
		{
			ambientZoneState.push_back(AUDIO::IS_AMBIENT_ZONE_ENABLED(const_cast<char*>(ambientZoneName.c_str())));
			AUDIO::SET_AMBIENT_ZONE_STATE_PERSISTENT(const_cast<char*>(ambientZoneName.c_str()), false, true);
			index++;
		}
	}

	void suppressModels(const std::vector<std::string>& models, bool isVehicle = false)
	{
		for (const auto& model : models)
		{
			auto modelHash = GAMEPLAY::GET_HASH_KEY(const_cast<char*>(model.c_str()));
			if (isVehicle)
				VEHICLE::SET_VEHICLE_MODEL_IS_SUPPRESSED(modelHash, true);
			else
				PED::SET_PED_MODEL_IS_SUPPRESSED(modelHash, true);
		}
	}

	void lockRadioStations(const std::vector<std::string>& radioStations)
	{
		for (const auto& station : radioStations)
		{
			AUDIO::LOCK_RADIO_STATION(const_cast<char*>(station.c_str()), true);
		}
	}

	void switchTrainTracks(const std::vector<int>& trackIds, bool state)
	{
		for (const auto& trackId : trackIds)
		{
			VEHICLE::SWITCH_TRAIN_TRACK(trackId, state);
		}
	}

	void unloadMapLiberty(//const std::vector<std::string>& scenarios,
		/*const std::vector<std::string>& zones,*/ /*const std::vector<std::string>& ambientZones,*/ /*const std::vector<std::string>& peds,
		const std::vector<std::string>& vehicles,*/ /*const std::vector<std::string>& radios,*/
		const std::vector<int>& trainTracks)
	{
		//disableScenarios(scenarios);
		//disableZones(zones);
		//disableAmbientZones(ambientZones);
		//suppressModels(peds);
		//suppressModels(vehicles, true);
		//lockRadioStations(radios);
		switchTrainTracks(trainTracks, false);
	}

	void unloadMapSantos(//const std::vector<std::string>& scenarios,
		/*const std::vector<std::string>& zones,*/ /*const std::vector<std::string>& ambientZones,(/ /*const std::vector<std::string>& peds,
		const std::vector<std::string>& vehicles,*/ /*const std::vector<std::string>& radios,*/
		const std::vector<int>& trainTracks/*, std::vector<bool>& scenarioState,*/ /*std::vector<bool>& ambientZoneState*/)
	{
		//disableScenarios(scenarios, scenarioState);
		//disableZones(zones);
		//disableAmbientZones(ambientZones, ambientZoneState);
		//suppressModels(peds);
		//suppressModels(vehicles, true);
		//lockRadioStations(radios);
		switchTrainTracks(trainTracks, false);
	}

	/////////////////////////////////////////////////////////
	////           Unloading Helper Functions            ////
	/////////////////////////////////////////////////////////

	// Unload the Cayo Perico Island
	void unloadCayo()
	{
		//removeIpls(cayoMpIpls);
		cayoMpIplGroup.RemoveIplGroup(false);
		/*if (!worldtravel::MpMap::IsMPMapActive())
		{
			DLC2::_UNLOAD_MP_DLC_MAPS();
			while (DLC2::GET_IS_LOADING_SCREEN_ACTIVE())
			{
				WAIT(0); // avoid freezing the game
			}

			removeIpls(santosIpls);
			removeIpls(santosSpIpls);
			WAIT(1000); // 1 Second
		}*/

		//disableScenarios(cayoScenarios);
		cayoScenarioGroup.RemoveScenarioGroup(false);
		//disableZones(cayoZones);
		cayoZoneGroup.DisableZoneGroup();
		//disableAmbientZones(cayoAmbientZones);
		cayoAmbientZoneGroup.RemoveAmbientZoneGroup(false);

		// if in multiplayer
		/*if (NETWORK::NETWORK_IS_IN_SESSION())
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_NOW_PERSIST_NETWORK(1);
		}
		else
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
		}
		GAMEPLAY::CLEAR_OVERRIDE_WEATHER();*/
		cayoWeatherTypes.ResetWeatherType(false);

		// if in singleplayer
		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			STREAMING::SET_ISLAND_ENABLED("HeistIsland", false);
		}
		WAIT(1000);

		

		UI::SET_USE_ISLAND_MAP(false);
	}

	// Unload Liberty City
	void unloadLiberty()
	{
		/*//Settings::Load();
		//STREAMING::SET_ISLAND_ENABLED(const_cast<char*>("LibertyCity"), false);
		//WAIT(1000);

		//removeIpls(libertyIpls);
		libertyIplGroup.RemoveIplGroup(false);
		// Remove MP or SP IPLs based on session status
		if (worldtravel::MpMap::IsMPMapActive())
		{
			//removeIpls(libertyMpIpls);
			libertyMpIplGroup.RemoveIplGroup(false);
		}
		else
		{
			//removeIpls(libertySpIpls);
			libertySpIplGroup.RemoveIplGroup(false);
		}
		if (loadLibertyLODLights)
		{
			//removeIpls(libertyLODLightIpls);
			libertyLODLightIplGroup.RemoveIplGroup(false);
		}

		libertyScenarioGroup.RemoveScenarioGroup(false);
		libertyZoneGroup.DisableZoneGroup();
		libertyAmbientZoneGroup.RemoveAmbientZoneGroup(false);
		libertyPedGroup.SuppressZonedModels(true, false);
		libertyVehicleGroup.SuppressZonedModels(true, true);
		libertyRadioGroup.DisableRadioStationGroup();*/
		//unloadMapLiberty(libertyScenarios, libertyZones, libertyAmbientZones, libertyPeds, libertyVehicles, libertyRadio, { 12, 13, 14, 16, 17, 18 });
		unloadMapLiberty({ 12, 13, 14, 16, 17, 18 });


		// if in multiplayer
		/*if (NETWORK::NETWORK_IS_IN_SESSION())
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_NOW_PERSIST_NETWORK(1);
		}
		else
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
		}
		GAMEPLAY::CLEAR_OVERRIDE_WEATHER();*/
		/*libertyWeatherTypes.ResetWeatherType(false);

		if (!NETWORK::NETWORK_IS_IN_SESSION())
			UI::_SET_MINIMAP_REVEALED(false);*/

		

		
	}

	// Unload Los Santos
	void unloadSantos()
	{
		/*patches::farlods::SetVisible(false);

		//removeIpls(santosIpls, santosIplState);
		santosIplGroup.RemoveIplGroup(true);
		// Remove MP or SP IPLs based on session status
		if (worldtravel::MpMap::IsMPMapActive())
		{
			//removeIpls(santosMpIpls, santosMpIplState);
			santosMpIplGroup.RemoveIplGroup(true);
		}
		else
		{
			//removeIpls(santosSpIpls, santosSpIplState);
			santosSpIplGroup.RemoveIplGroup(true);
		}

		santosScenarioGroup.RemoveScenarioGroup(true);
		santosZoneGroup.DisableZoneGroup();
		santosAmbientZoneGroup.RemoveAmbientZoneGroup(true);
		santosPedGroup.SuppressZonedModels(true, false);
		santosVehicleGroup.SuppressZonedModels(true, true);
		santosRadioGroup.DisableRadioStationGroup();*/
		//unloadMapSantos(santosScenarios, santosZones, santosAmbientZones, santosPeds, santosVehicles, santosRadio, { 0, 3 }, santosScenariosState, santosAmbientZonesState);
		unloadMapSantos({ 0, 3 });
		
		// if in multiplayer
		/*if (NETWORK::NETWORK_IS_IN_SESSION())
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_NOW_PERSIST_NETWORK(1);
		}
		else
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
		}
		GAMEPLAY::CLEAR_OVERRIDE_WEATHER();*/

		// if in singleplayer
		/*if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			UI::_SET_MINIMAP_REVEALED(true);
			for (int i = 0; i < 5; i++)
				GAMEPLAY::DISABLE_HOSPITAL_RESTART(i, true);
			for (int i = 0; i < 7; i++)
				GAMEPLAY::DISABLE_POLICE_RESTART(i, true);
		}

		STREAMING::_SET_MAPDATACULLBOX_ENABLED((char*)"HeistIsland", true);

		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			// disable the following scripts in singleplayer to remove blips and respawn points when not in LS
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"blip_controller");          // responsible for blips
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"forsalesigns");			   // responsible for property for sale signs
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"respawn_controller");	   // responsible for respawning the player
			if (!restrictedAreaClearance)
				GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"restrictedareas");		   // responsible for restricted areas
			GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME((char*)"vehicle_gen_controller");   // responsible for player businesses / vehicle blips
		}*/
	}

	// Unload North Yankton
	void unloadYankton()
	{
		//removeIpls(yanktonIpls);
		yanktonIplGroup.RemoveIplGroup(false);
		//disableZones(yanktonZones);
		yanktonZoneGroup.DisableZoneGroup();
		//disableAmbientZones(yanktonAmbientZones);
		yanktonAmbientZoneGroup.RemoveAmbientZoneGroup(false);
		UI::SET_MINIMAP_IN_PROLOGUE(false);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(5526.24f, -5137.23f, 61.78925f, 3679.327f, -4973.879f, 125.0828f, 192, false, false, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3691.211f, -4941.24f, 94.59368f, 3511.115f, -4869.191f, 126.7621f, 16, false, false, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3510.004f, -4865.81f, 94.69557f, 3204.424f, -4833.817f, 126.8152f, 16, false, false, true);
		PATHFIND::SET_ROADS_IN_ANGLED_AREA(3186.534f, -4832.798f, 109.8148f, 3202.187f, -4833.993f, 114.815f, 16, false, false, true);
		
		// if in multiplayer
		/*if (NETWORK::NETWORK_IS_IN_SESSION())
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_NOW_PERSIST_NETWORK(1);
		}
		else
		{
			GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
		}
		GAMEPLAY::CLEAR_OVERRIDE_WEATHER();*/
		yanktonWeatherTypes.ResetWeatherType(false);
	}

	// used to handle manual flight between maps
	void FlightController()
	{
		Entity player = playerPed;

		if (PED::IS_PED_IN_ANY_VEHICLE(player, 0))
			player = PED::GET_VEHICLE_PED_IS_USING(player);
		float playerAltitude = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(player);
		Vector3 playerVelocity = ENTITY::GET_ENTITY_VELOCITY(player);
		float playerVelocityX = playerVelocity.x;
		float playerVelocityY = playerVelocity.y;
		float playerVelocityZ = playerVelocity.z;
		if (worldtravel::IsLosSantos())
		{
			if (ENTITY::IS_ENTITY_AT_COORD(player, FlyToLCFromLSCoords[0], FlyToLCFromLSCoords[1], playerAltitude, 1500.0f, 1500.0f, 1500.0f, 0, 1, 0))
			{
				CAM::DO_SCREEN_FADE_OUT(800);
				WAIT(800);
				worldtravel::SetPlayerLocationID(-1);
				unloadSantos();
				losSantos.UnloadLevel();
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLSFromLCCoords[0] + 1500.00f, FlyToLSFromLCCoords[1] + 1500.00f, playerAltitude, 0, 0, 1);
				ENTITY::SET_ENTITY_HEADING(player, 300.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
				ENTITY::SET_ENTITY_VELOCITY(player, playerVelocityX, playerVelocityY, playerVelocityZ);
				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					TIME::ADD_TO_CLOCK_TIME(9, 0, 0);
				}
				else
				{
					NetworkClockController(true);
				}
				loadLiberty();
				libertyCity.LoadLevel();
				worldtravel::SetPlayerLocationID(1);
				CAM::DO_SCREEN_FADE_IN(800);
			}

			if (ENTITY::IS_ENTITY_AT_COORD(player, FlyToCPFromLSCoords[0], FlyToCPFromLSCoords[1], playerAltitude, 1500.0f, 1500.0f, 1500.0f, 0, 1, 0) &&
				!NETWORK::NETWORK_IS_IN_SESSION())
			{
				CAM::DO_SCREEN_FADE_OUT(800);
				WAIT(800);
				worldtravel::SetPlayerLocationID(-1);
				unloadSantos();
				losSantos.UnloadLevel();

				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLSFromCPCoords[0] + 1500.00f, FlyToLSFromCPCoords[1] - 1500.00f, playerAltitude, 0, 0, 1);
				}
				else
				{
					if (playerAltitude < 400.0f)
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToCPFromLSCoordsMP[0], FlyToCPFromLSCoordsMP[1], 200.0f, 0, 0, 1);
					}
					else
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToCPFromLSCoordsMP[0], FlyToCPFromLSCoordsMP[1], playerAltitude, 0, 0, 1);
					}
				}
				ENTITY::SET_ENTITY_HEADING(player, 240.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
				ENTITY::SET_ENTITY_VELOCITY(player, playerVelocityX, playerVelocityY, playerVelocityZ);
				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					TIME::ADD_TO_CLOCK_TIME(14, 0, 0);
				}
				else
				{
					NetworkClockController(true);
				}
				loadCayo();
				cayoPerico.LoadLevel();
				worldtravel::SetPlayerLocationID(3);
				CAM::DO_SCREEN_FADE_IN(800);
			}
		}
		else if (worldtravel::IsLibertyCity())
		{
			if (ENTITY::IS_ENTITY_AT_COORD(player, FlyToLSFromLCCoords[0], FlyToLSFromLCCoords[1], playerAltitude, 1500.0f, 1500.0f, 1500.0f, 0, 1, 0))
			{

				CAM::DO_SCREEN_FADE_OUT(800);
				WAIT(800);
				worldtravel::SetPlayerLocationID(-1);
				unloadLiberty();
				libertyCity.UnloadLevel();
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLCFromLSCoords[0] - 1500.00f, FlyToLCFromLSCoords[1] - 1500.00f, playerAltitude, 0, 0, 1);
				ENTITY::SET_ENTITY_HEADING(player, 130.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
				ENTITY::SET_ENTITY_VELOCITY(player, playerVelocityX, playerVelocityY, playerVelocityZ);
				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					TIME::ADD_TO_CLOCK_TIME(3, 0, 0);
				}
				else
				{
					NetworkClockController(false);
					NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
				}
				loadSantos();
				losSantos.UnloadLevel();
				worldtravel::SetPlayerLocationID(0);
				CAM::DO_SCREEN_FADE_IN(800);

			}

			if (ENTITY::IS_ENTITY_AT_COORD(player, FlyToCPFromLCCoords[0], FlyToCPFromLCCoords[1], playerAltitude, 1500.0f, 1500.0f, 1500.0f, 0, 1, 0) &&
				!NETWORK::NETWORK_IS_IN_SESSION())
			{

				CAM::DO_SCREEN_FADE_OUT(800);
				WAIT(800);
				worldtravel::SetPlayerLocationID(-1);
				libertyCity.UnloadLevel();
				worldtravel::SetPlayerLocationID(3);
				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLCFromCPCoords[0] + 1500.00f, FlyToLCFromCPCoords[1] - 1500.00f, playerAltitude, 0, 0, 1);
				}
				else
				{
					if (playerAltitude < 400.0f)
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToCPFromLCCoordsMP[0], FlyToCPFromLCCoordsMP[1], 200.0f, 0, 0, 1);
					}
					else
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToCPFromLCCoordsMP[0], FlyToCPFromLCCoordsMP[1], playerAltitude, 0, 0, 1);
					}
				}
				ENTITY::SET_ENTITY_HEADING(player, 220.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
				ENTITY::SET_ENTITY_VELOCITY(player, playerVelocityX, playerVelocityY, playerVelocityZ);
				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					TIME::ADD_TO_CLOCK_TIME(6, 0, 0);
				}
				unloadLiberty();
				loadCayo();
				cayoPerico.UnloadLevel();
				CAM::DO_SCREEN_FADE_IN(800);

			}
		}
		else if (worldtravel::IsCayoPerico())
		{
			int destination = -1;
			if (!NETWORK::NETWORK_IS_IN_SESSION())
			{
				if (ENTITY::IS_ENTITY_AT_COORD(player, FlyToLSFromCPCoords[0], FlyToLSFromCPCoords[1], playerAltitude, 1500.0f, 1500.0f, 1500.0f, 0, 1, 0))
				{
					destination = 0;
				}

				if (ENTITY::IS_ENTITY_AT_COORD(player, FlyToLCFromCPCoords[0], FlyToLCFromCPCoords[1], playerAltitude, 1500.0f, 1500.0f, 1500.0f, 0, 1, 0))
				{
					destination = 1;
				}
			}
			else
			{
				if (!ENTITY::IS_ENTITY_DEAD(playerPed) &&
					!ENTITY::IS_ENTITY_IN_AREA(playerPed, CayoPericoWestBound, CayoPericoSouthBound, -10000.0f, CayoPericoEastBound, CayoPericoNorthBound, 10000.0f, false, false, false))
				{
					float playerHeading = ENTITY::GET_ENTITY_HEADING(player);
					if ((playerHeading >= CayoPericoMPDestHeading) && playerHeading <= 180.0f + CayoPericoMPDestHeading)
					{
						destination = 0;
					}
					else
					{
						destination = 1;
					}
				}
			}

			if (destination == 0)
			{
				CAM::DO_SCREEN_FADE_OUT(800);
				WAIT(800);
				worldtravel::SetPlayerLocationID(-1);
				//unloadCayo();
				cayoPerico.UnloadLevel();
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToCPFromLSCoords[0] - 1500.00f, FlyToCPFromLSCoords[1] + 1500.00f, playerAltitude, 0, 0, 1);
				ENTITY::SET_ENTITY_HEADING(player, 45.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
				ENTITY::SET_ENTITY_VELOCITY(player, playerVelocityX, playerVelocityY, playerVelocityZ);
				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					TIME::ADD_TO_CLOCK_TIME(6, 0, 0);
				}
				else
				{
					NetworkClockController(false);
					NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
				}
				loadSantos();
				losSantos.LoadLevel();
				worldtravel::SetPlayerLocationID(0);
				CAM::DO_SCREEN_FADE_IN(800);
			}
			else if (destination == 1)
			{
				CAM::DO_SCREEN_FADE_OUT(800);
				WAIT(800);
				worldtravel::SetPlayerLocationID(-1);
				//unloadCayo();
				cayoPerico.UnloadLevel();
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToCPFromLCCoords[0] - 1500.00f, FlyToCPFromLCCoords[1] + 1500.00f, playerAltitude, 0, 0, 1);
				ENTITY::SET_ENTITY_HEADING(player, 10.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
				CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
				ENTITY::SET_ENTITY_VELOCITY(player, playerVelocityX, playerVelocityY, playerVelocityZ);
				if (!NETWORK::NETWORK_IS_IN_SESSION())
				{
					TIME::ADD_TO_CLOCK_TIME(4, 0, 0);
				}
				loadLiberty();
				libertyCity.LoadLevel();
				worldtravel::SetPlayerLocationID(1);
				CAM::DO_SCREEN_FADE_IN(800);
			}
		}
	}

	// Cutscene
	void PlayFlightCutscene(std::string cutsceneName, int flags, bool playbackList, bool start, bool end, bool stopImmediately, bool fadeVal0, bool fadeVal1, bool fadeVal2, bool fadeVal3)
	{
		if (playbackList)
			CUTSCENE::REQUEST_CUTSCENE_WITH_PLAYBACK_LIST(const_cast<char*>(cutsceneName.c_str()), 1, flags);
		else
			CUTSCENE::REQUEST_CUTSCENE(const_cast<char*>(cutsceneName.c_str()), flags);
		CUTSCENE::SET_CUTSCENE_PED_COMPONENT_VARIATION_FROM_PED("MP_1", playerPed, 0);
		CUTSCENE::REGISTER_ENTITY_FOR_CUTSCENE(playerPed, "MP_1", 0, 0, 64);
		CUTSCENE::SET_CUTSCENE_FADE_VALUES(true, true, true, true);
		CUTSCENE::START_CUTSCENE(0);
		if (NETWORK::NETWORK_IS_IN_SESSION() && start)
		{
			NETWORK::NETWORK_SET_IN_MP_CUTSCENE(true, true);
		}
		while (CUTSCENE::IS_CUTSCENE_ACTIVE())
		{
			WAIT(0);
		}
		if (stopImmediately)
			CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
		CUTSCENE::REMOVE_CUTSCENE();

		if (NETWORK::NETWORK_IS_IN_SESSION() && end)
		{
			NETWORK::NETWORK_SET_IN_MP_CUTSCENE(false, false);
		}
		CAM::DO_SCREEN_FADE_OUT(0);
	}

	// Spawn rental car
	void SpawnRentalCar(std::string vehicleName, float x, float y, float z, float heading, bool warpPlayerIntoVehicle)
	{

		rentalVehicleHash = GAMEPLAY::GET_HASH_KEY(const_cast<char*>(vehicleName.c_str()));
		ENTITY::SET_ENTITY_HEADING(playerPed, 86.0f);
		STREAMING::REQUEST_MODEL(rentalVehicleHash);
		while (!STREAMING::HAS_MODEL_LOADED(rentalVehicleHash))
		{
			WAIT(0);
		}
		rentalVehicle = VEHICLE::CREATE_VEHICLE(rentalVehicleHash, x, y, z, heading, true, true, false);
		if (warpPlayerIntoVehicle)
			AI::TASK_WARP_PED_INTO_VEHICLE(playerPed, rentalVehicle, -1);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(rentalVehicleHash);
	}

	// Airport Travel from Los Santos
	void AirportTravelLS()
	{
		if (!ENTITY::IS_ENTITY_DEAD(playerPed) && ENTITY::IS_ENTITY_AT_COORD(playerPed, LosSantosIntAirport[0], LosSantosIntAirport[1], LosSantosIntAirport[2], 150.0f, 150.0f, 150.0f, 0, 1, 0) && !PED::IS_PED_IN_ANY_VEHICLE(player, 0))
		{
			GRAPHICS::DRAW_MARKER(1, -1050.53f, -2741.48f, 13.60f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.6f, 0.6f, 0.5f, 240, 200, 80, 150, false, false, 2, false, nullptr, nullptr, 0);
			if (ENTITY::IS_ENTITY_AT_COORD(playerPed, LosSantosIntAirport[0], LosSantosIntAirport[1], LosSantosIntAirport[2], 1.0f, 1.0f, 1.0f, 0, 1, 0))
			{
				if (PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()) == 0)
				{
					worldtravel::HelpText::DisplayHelpText("Press ~INPUT_CONTEXT~ to fly to Liberty City or ~INPUT_CONTEXT_SECONDARY~ to North Yankton for $350.");
					if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 51) || CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 52))
					{
						int destination = 0;
						if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 51))
						{
							destination = 1;
						}
						else if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 52))
						{
							destination = 2;
						}
						CAM::DO_SCREEN_FADE_OUT(800);
						WAIT(800);
						worldtravel::SetPlayerLocationID(-1);
						int maxWanted = PLAYER::GET_MAX_WANTED_LEVEL();
						PLAYER::SET_MAX_WANTED_LEVEL(0);
						ENTITY::FREEZE_ENTITY_POSITION(playerPed, true);
						if (destination == 1)
							PlayFlightCutscene("hs4_lsa_take_nimb2", 8, false, true, false, false, true, true, true, true);
						else if (destination == 2)
							PlayFlightCutscene("hs4_lsa_take_nimb2", 8, false, true, false, true, true, true, true, true);

						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLCFromLSCoords[0] - 1500.00f, FlyToLCFromLSCoords[1] - 1500.00f, FlyToLCFromLSCoords[2] + 1500.00f, 0, 0, 1);
						unloadSantos();
						losSantos.UnloadLevel();
						WAIT(100);
						if (destination == 1)
						{
							loadLiberty();
							libertyCity.LoadLevel();
						}
						else if (destination == 2)
						{
							loadYankton();
							northYankton.LoadLevel();
						}
						if (destination == 1)
						{
							ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, FrancisIntAirportArrival[0], FrancisIntAirportArrival[1], FrancisIntAirportArrival[2], 0, 0, 1);
							ENTITY::SET_ENTITY_HEADING(playerPed, 97.4f);
						}
						else if (destination == 2)
						{
							ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, NorthYanktonAirportArrival[0], NorthYanktonAirportArrival[1], NorthYanktonAirportArrival[2], 0, 0, 1);
							SpawnRentalCar("asea2", NorthYanktonAirportArrival[0], NorthYanktonAirportArrival[1], NorthYanktonAirportArrival[2], 86.0f, true);
						}
						CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
						CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
						if (!NETWORK::NETWORK_IS_IN_SESSION())
						{
							if (destination == 1)
								TIME::ADD_TO_CLOCK_TIME(13, 0, 0);
							else if (destination == 2)
								TIME::ADD_TO_CLOCK_TIME(12, 0, 0);
						}
						else
						{
							NetworkClockController(true);
						}
						if (destination == 1)
						{
							PlayFlightCutscene("lc_lca_land_nimb", 24, true, false, true, true, true, true, true, true);
						}
						ENTITY::FREEZE_ENTITY_POSITION(playerPed, false);
						if (destination == 1)
							ENTITY::SET_ENTITY_HEADING(playerPed, 97.4f);
						else if (destination == 2)
							ENTITY::SET_ENTITY_HEADING(playerPed, 86.0f);
						PLAYER::SET_MAX_WANTED_LEVEL(maxWanted);
						worldtravel::SetPlayerLocationID(destination);
						CAM::DO_SCREEN_FADE_IN(800);
					}
				}
				else
				{
					worldtravel::HelpText::DisplayHelpText("You cannot buy a plane ticket while wanted by the cops!");
				}
			}
		}
	}

	// Airport Travel from Liberty City
	void AirportTravelLC()
	{
		if (!ENTITY::IS_ENTITY_DEAD(playerPed) && ENTITY::IS_ENTITY_AT_COORD(playerPed, FrancisIntAirport[0], FrancisIntAirport[1], FrancisIntAirport[2], 150.0f, 150.0f, 150.0f, 0, 1, 0) && !PED::IS_PED_IN_ANY_VEHICLE(player, 0))
		{
			GRAPHICS::DRAW_MARKER(1, 7553.86f, -2879.39f, 5.08f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.6f, 0.6f, 0.5f, 240, 200, 80, 150, false, false, 2, false, nullptr, nullptr, 0);
			if (ENTITY::IS_ENTITY_AT_COORD(playerPed, FrancisIntAirport[0], FrancisIntAirport[1], FrancisIntAirport[2], 1.0f, 1.0f, 1.0f, 0, 1, 0))
			{
				if (PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()) == 0)
				{
					worldtravel::HelpText::DisplayHelpText("Press ~INPUT_CONTEXT~ to fly to Los Santos or ~INPUT_CONTEXT_SECONDARY~ to North Yankton for $350.");
					if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 51) || CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 52))
					{
						int destination = 1;
						if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 51))
						{
							destination = 0;
						}
						else if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 52))
						{
							destination = 2;
						}
						CAM::DO_SCREEN_FADE_OUT(800);
						WAIT(800);
						worldtravel::SetPlayerLocationID(-1);
						int maxWanted = PLAYER::GET_MAX_WANTED_LEVEL();
						PLAYER::SET_MAX_WANTED_LEVEL(0);
						ENTITY::FREEZE_ENTITY_POSITION(playerPed, true);
						if (destination == 0)
							PlayFlightCutscene("lc_lca_take_nimb2", 8, false, true, false, false, true, true, true, true);
						else if (destination == 2)
							PlayFlightCutscene("lc_lca_take_nimb2", 8, false, true, false, true, true, true, true, true);
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLSFromLCCoords[0] + 1500.00f, FlyToLSFromLCCoords[1] + 1500.00f, FlyToLSFromLCCoords[2] + 1500.00f, 0, 0, 1);
						unloadLiberty();
						libertyCity.UnloadLevel();
						WAIT(100);
						if (destination == 0)
						{
							loadSantos();
							losSantos.LoadLevel();
						}
						else if (destination == 2)
						{
							loadYankton();
							northYankton.LoadLevel();
						}
						if (destination == 0)
						{
							ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, LosSantosIntAirportArrival[0], LosSantosIntAirportArrival[1], LosSantosIntAirportArrival[2], 0, 0, 1);
							ENTITY::SET_ENTITY_HEADING(playerPed, 341.24f);
						}
						else if (destination == 2)
						{
							ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, NorthYanktonAirportArrival[0], NorthYanktonAirportArrival[1], NorthYanktonAirportArrival[2], 0, 0, 1);
							SpawnRentalCar("asea2", NorthYanktonAirportArrival[0], NorthYanktonAirportArrival[1], NorthYanktonAirportArrival[2], 86.0f, true);
						}

						if (!NETWORK::NETWORK_IS_IN_SESSION())
						{
							if (destination == 0)
								TIME::ADD_TO_CLOCK_TIME(3, 0, 0);
							else if (destination == 2)
								TIME::ADD_TO_CLOCK_TIME(7, 0, 0);
						}
						else
						{
							if (destination == 0)
							{
								NetworkClockController(false);
								NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
							}
						}
						if (destination == 0)
						{
							PlayFlightCutscene("hs4_lsa_land_nimb", 24, true, false, true, true, true, true, true, true);
						}
						ENTITY::FREEZE_ENTITY_POSITION(playerPed, false);
						if (destination == 0)
							ENTITY::SET_ENTITY_HEADING(playerPed, 341.24f);
						else if (destination == 2)
							ENTITY::SET_ENTITY_HEADING(playerPed, 86.0f);
						PLAYER::SET_MAX_WANTED_LEVEL(maxWanted);
						worldtravel::SetPlayerLocationID(destination);
						CAM::DO_SCREEN_FADE_IN(800);
					}
				}
				else
				{
					worldtravel::HelpText::DisplayHelpText("You cannot buy a plane ticket while wanted by the cops!");
				}
			}
		}
	}

	// Airport Travel from North Yankton
	void AirportTravelNY()
	{
		if (!ENTITY::IS_ENTITY_DEAD(playerPed) &&
			(!ENTITY::IS_ENTITY_IN_AREA(playerPed, 2570.86f, -5099.23f, -10000.0f, 3486.60f, -4551.66f, 10000.0f, false, false, false) &&
				!ENTITY::IS_ENTITY_IN_AREA(playerPed, 3486.60f, -5099.23f, -10000.0f, 3747.66f, -4627.10f, 10000.0f, false, false, false) &&
				!ENTITY::IS_ENTITY_IN_AREA(playerPed, 3747.66f, -5399.22f, -10000.0f, 5789.13f, -4657.80f, 10000.0f, false, false, false)))
		{
			if (PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()) == 0)
			{
				worldtravel::HelpText::DisplayHelpText("Press ~INPUT_CONTEXT~ to fly to Los Santos or ~INPUT_CONTEXT_SECONDARY~ to Liberty City for $350.");
				if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 51) || CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 52))
				{
					int destination = 2;
					if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 51))
					{
						destination = 0;
					}
					else if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 52))
					{
						destination = 1;
					}
					CAM::DO_SCREEN_FADE_OUT(800);
					WAIT(800);
					worldtravel::SetPlayerLocationID(-1);
					int maxWanted = PLAYER::GET_MAX_WANTED_LEVEL();
					PLAYER::SET_MAX_WANTED_LEVEL(0);
					ENTITY::FREEZE_ENTITY_POSITION(playerPed, true);
					if (PED::IS_PED_IN_ANY_VEHICLE(player, 0))
					{
						playerVehicle = PED::GET_VEHICLE_PED_IS_USING(player);
						ENTITY::SET_ENTITY_AS_MISSION_ENTITY(playerVehicle, true, true);
						VEHICLE::DELETE_VEHICLE(&playerVehicle);
					}
					if (ENTITY::DOES_ENTITY_EXIST(rentalVehicle))
					{
						VEHICLE::DELETE_VEHICLE(&rentalVehicle);
					}
					//unloadYankton();
					northYankton.UnloadLevel();
					WAIT(100);
					if (destination == 0)
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLCFromLSCoords[0] - 1500.00f, FlyToLCFromLSCoords[1] - 1500.00f, FlyToLCFromLSCoords[2] + 1500.00f, 0, 0, 1);
						loadSantos();
						losSantos.UnloadLevel();
					}
					else if (destination == 1)
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLSFromLCCoords[0] + 1500.00f, FlyToLSFromLCCoords[1] + 1500.00f, FlyToLSFromLCCoords[2] + 1500.00f, 0, 0, 1);
						loadLiberty();
						libertyCity.UnloadLevel();
					}
					if (destination == 0)
					{
						CAM::DO_SCREEN_FADE_OUT(0);
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, LosSantosIntAirportArrival[0], LosSantosIntAirportArrival[1], LosSantosIntAirportArrival[2], 0, 0, 1);
						ENTITY::SET_ENTITY_HEADING(playerPed, 341.24f);
					}
					else if (destination == 1)
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, FrancisIntAirportArrival[0], FrancisIntAirportArrival[1], FrancisIntAirportArrival[2], 0, 0, 1);
						ENTITY::SET_ENTITY_HEADING(playerPed, 97.4f);
					}
					CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
					CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);

					if (!NETWORK::NETWORK_IS_IN_SESSION())
					{
						if (destination == 0)
							TIME::ADD_TO_CLOCK_TIME(7, 0, 0);
						else if (destination == 1)
							TIME::ADD_TO_CLOCK_TIME(9, 0, 0);
					}
					else
					{
						if (destination == 0)
						{
							NetworkClockController(false);
							NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
						}
					}
					if (destination == 0)
						PlayFlightCutscene("hs4_lsa_land_nimb", 24, true, true, true, true, true, true, true, true);
					else if (destination == 1)
						PlayFlightCutscene("lc_lca_land_nimb", 24, true, true, true, true, true, true, true, true);
					
					ENTITY::FREEZE_ENTITY_POSITION(playerPed, false);
					if (destination == 0)
						ENTITY::SET_ENTITY_HEADING(playerPed, 341.24f);
					else if (destination == 1)
						ENTITY::SET_ENTITY_HEADING(playerPed, 97.4f);
					PLAYER::SET_MAX_WANTED_LEVEL(maxWanted);
					worldtravel::SetPlayerLocationID(destination);
					CAM::DO_SCREEN_FADE_IN(800);
				}
			}
			else
			{
				worldtravel::HelpText::DisplayHelpText("You cannot buy a plane ticket while wanted by the cops!");
			}
		}
	}

	void AirportTravel()
	{
		if (worldtravel::IsLosSantos())
		{
			AirportTravelLS();
		}
		if (worldtravel::IsLibertyCity())
		{
			AirportTravelLC();
		}
		if (worldtravel::IsNorthYankton())
		{
			AirportTravelNY();
		}
	}

	// Handles the docks travel system from Los Santos to Liberty City
	void DocksTravel()
	{
		Entity player = playerPed;
		if (PED::IS_PED_IN_ANY_VEHICLE(player, 0))
		{
			player = PED::GET_VEHICLE_PED_IS_USING(player);
			Vector3 playerVelocity = ENTITY::GET_ENTITY_VELOCITY(player);
			float velocityMagnitude = sqrt(playerVelocity.x * playerVelocity.x + playerVelocity.y * playerVelocity.y + playerVelocity.z * playerVelocity.z);
			int currentLocation = worldtravel::IsLosSantos() ? 0 : worldtravel::IsLibertyCity() ? 1 : -1;

			struct DockInfo {
				float* coords;
				float markerX, markerY, markerZ;
				float heading;
				const char* helpText;
				int destination;
			} docks[] = {
				{ LosSantosDocks.data(), 1170.40f, -2973.52f, 4.93f, 334.40f, "Press ~INPUT_CONTEXT~ to ship this vehicle to Liberty City for $3500.", 1},
				{ LibertyCityDocks.data(), 5935.99f, -3531.89f, 4.85f, 90.0f, "Press ~INPUT_CONTEXT~ to ship this vehicle to Los Santos for $3500.", 0}
			};

			if (currentLocation != -1 && !ENTITY::IS_ENTITY_DEAD(playerPed) && ENTITY::IS_ENTITY_AT_COORD(player, docks[currentLocation].coords[0], docks[currentLocation].coords[1], docks[currentLocation].coords[2], 150.0f, 150.0f, 150.0f, 0, 1, 0))
			{
				GRAPHICS::DRAW_MARKER(1, docks[currentLocation].markerX, docks[currentLocation].markerY, docks[currentLocation].markerZ, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 9.0f, 9.0f, 0.8f, 240, 200, 80, 150, false, false, 2, false, nullptr, nullptr, 0);

				if (ENTITY::IS_ENTITY_AT_COORD(player, docks[currentLocation].coords[0], docks[currentLocation].coords[1], docks[currentLocation].coords[2], 10.0f, 10.0f, 1.0f, 0, 1, 0))
				{
					if (PLAYER::GET_PLAYER_WANTED_LEVEL(playerPed) == 0)
					{
						if (velocityMagnitude < 0.1f)
						{
							worldtravel::HelpText::DisplayHelpText(docks[currentLocation].helpText);
							if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(2, 51))
							{
								CAM::DO_SCREEN_FADE_OUT(800);
								WAIT(800);
								worldtravel::SetPlayerLocationID(-1);
								int maxWanted = PLAYER::GET_MAX_WANTED_LEVEL();
								PLAYER::SET_MAX_WANTED_LEVEL(0);
								ENTITY::FREEZE_ENTITY_POSITION(player, true);
								ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLCFromLSCoords[0] + 1500, FlyToLCFromLSCoords[1] + 1500, FlyToLCFromLSCoords[2] + 1500, 0, 0, 1);

								switch (currentLocation)
								{
								case 0:
									unloadSantos();
									losSantos.UnloadLevel();
									break;
								case 1:
									unloadLiberty();
									libertyCity.UnloadLevel();
									break;
								}

								switch (docks[currentLocation].destination)
								{
								case 1:
									loadLiberty();
									libertyCity.LoadLevel();
									ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, LibertyCityDocksArrival[0], LibertyCityDocksArrival[1], LibertyCityDocksArrival[2], 0, 0, 1);
									break;
								case 0:
									loadSantos();
									losSantos.LoadLevel();
									ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, LosSantosDocksArrival[0], LosSantosDocksArrival[1], LosSantosDocksArrival[2], 0, 0, 1);
									break;
								}

								ENTITY::SET_ENTITY_HEADING(player, docks[currentLocation].heading);
								CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
								CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);

								if (!NETWORK::NETWORK_IS_IN_SESSION())
								{
									TIME::ADD_TO_CLOCK_TIME(12, 0, 0);
								}
								else
								{
									NetworkClockController(docks[currentLocation].destination == 1);
									if (docks[currentLocation].destination == 0)
										NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
								}

								ENTITY::SET_ENTITY_HEADING(playerPed, docks[currentLocation].heading);
								PLAYER::SET_MAX_WANTED_LEVEL(maxWanted);
								ENTITY::FREEZE_ENTITY_POSITION(player, false);
								worldtravel::SetPlayerLocationID(docks[currentLocation].destination);
								CAM::DO_SCREEN_FADE_IN(800);
							}
						}
					}
					else
					{
						worldtravel::HelpText::DisplayHelpText("You cannot ship this vehicle while wanted by the cops!");
					}
				}
			}
		}
	}


	// Handles Fast Travelling
	void TeleportPlayer(Entity& player, float x, float y, float z, float heading)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(player, 0))
		{
			player = PED::GET_VEHICLE_PED_IS_USING(player);
		}
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, x, y, z, 0, 0, 1);
		ENTITY::SET_ENTITY_HEADING(player, heading);
		CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
		CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
	}

	void PerformTeleport(Entity player, int currentLocation, int destination)
	{
		CAM::DO_SCREEN_FADE_OUT(800);
		WAIT(800);
		worldtravel::SetPlayerLocationID(-1);

		// TP player to temp location
		ENTITY::FREEZE_ENTITY_POSITION(player, true);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(player, FlyToLCFromLSCoords[0] + 1500, FlyToLCFromLSCoords[1] + 1500, FlyToLCFromLSCoords[2] + 1500, 0, 0, 1);

		if (currentLocation != destination)
		{
			// Unload the current map
			switch (currentLocation)
			{
			case 1: // Liberty City
				unloadLiberty();
				libertyCity.UnloadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-3, 0, 0);
				break;
			case 2: // North Yankton
				//unloadYankton();
				northYankton.UnloadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-2, 0, 0);
				break;
			case 3: // Cayo Perico
				//unloadCayo();
				cayoPerico.UnloadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-5, 0, 0);
				break;
			case 4: // Santa Fuegos
				santaFuegos.UnloadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-5, 0, 0);
				break;
			default: // Los Santos
				unloadSantos();
				losSantos.UnloadLevel();
				break;
			}

			// Load the new map
			switch (destination)
			{
			case 1: // Liberty City
				loadLiberty();
				libertyCity.LoadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(3, 0, 0);
				else NetworkClockController(true);
				break;

			case 2: // North Yankton
				loadYankton();
				northYankton.LoadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(2, 0, 0);
				break;

			case 3: // Cayo Perico
				loadCayo();
				cayoPerico.LoadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(5, 0, 0);
				break;

			case 4: // Santa Fuegos
				santaFuegos.LoadLevel();
				if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(5, 0, 0);
				break;

			case 0: // Los Santos
				loadSantos();
				losSantos.LoadLevel();
				if (NETWORK::NETWORK_IS_IN_SESSION())
				{
					NetworkClockController(false);
					NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
				}
				break;
			}
		}

		// Teleport the player
		switch (destination)
		{
		case 1: // Liberty City
			TeleportPlayer(player, Settings::LC_X, Settings::LC_Y, Settings::LC_Z, 0.0f);
			break;

		case 2: // North Yankton
			TeleportPlayer(player, Settings::NY_X, Settings::NY_Y, Settings::NY_Z, 0.0f);
			break;

		case 3: // Cayo Perico
			TeleportPlayer(player, Settings::CP_X, Settings::CP_Y, Settings::CP_Z, 0.0f);
			break;

		case 4: // Santa Fuegos
			TeleportPlayer(player, Settings::SF_X, Settings::SF_Y, Settings::SF_Z, 0.0f);
			break;

		case 0: // Los Santos
			TeleportPlayer(player, Settings::LS_X, Settings::LS_Y, Settings::LS_Z, 0.0f);
			break;
		}

		ENTITY::FREEZE_ENTITY_POSITION(player, false);
		worldtravel::SetPlayerLocationID(destination);
		CAM::DO_SCREEN_FADE_IN(800);
	}

	int activeLineIndexTeleport = 0;

	void TeleportBetweenMaps()
	{

		if (IsKeyJustUp(tpKey))
		{
			//Settings::Load();
			if (fastTravel)
			{
				Entity player = playerPed;
				int currentLocation = worldtravel::GetPlayerLocationID();
				if (PED::IS_PED_IN_ANY_VEHICLE(player, 0))
				{
					player = PED::GET_VEHICLE_PED_IS_USING(player);
				}
				int destination = -1;

				/*const float lineWidth = 250.0;
				const int lineCount = 4;

				worldtravel::Menu::MenuBeep("SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET");

				std::string caption = "FAST TRAVEL";

				static LPCSTR lineCaption[lineCount] = {
					"Los Santos",
					"North Yankton",
					"Cayo Perico",
					"Liberty City"
				};

				DWORD waitTime = 150;
				
				while (true)
				{
					worldtravel::Menu::DisableControls(true);
					// timed menu draw, used for pause after active line switch
					DWORD maxTickCount = GetTickCount64() + waitTime;
					do
					{
						// draw menu
						worldtravel::Menu::DrawMenuLine(caption, lineWidth, 15.0, 18.0, 0.0, 5.0, false, true);
						for (int i = 0; i < lineCount; i++)
							if (i != activeLineIndexTeleport)
								worldtravel::Menu::DrawMenuLine(lineCaption[i], lineWidth, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
						worldtravel::Menu::DrawMenuLine(lineCaption[activeLineIndexTeleport], lineWidth + 1.0, 11.0, 56.0 + activeLineIndexTeleport * 36.0, 0.0, 7.0, true, false);


						WAIT(0);
					} while (GetTickCount64() < maxTickCount);
					waitTime = 0;

					// process buttons
					bool bSelect, bBack, bUp, bDown;
					worldtravel::Menu::GetButtonState(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);

					if (bSelect)
					{
						worldtravel::Menu::MenuBeep("SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET");
						switch (activeLineIndexTeleport)
						{
						case 0:
							destination = 0; // Los Santos
							PerformTeleport(player, currentLocation, destination);
							break;
						case 1:
							destination = 2; // North Yankton
							PerformTeleport(player, currentLocation, destination);
							break;
						case 2:
							destination = 3; // Cayo Perico
							PerformTeleport(player, currentLocation, destination);
							break;
						case 3:
							destination = 1; // Liberty City
							PerformTeleport(player, currentLocation, destination);
							break;
						}
						waitTime = 200;
					}
					else
					{
						if (bBack || IsKeyJustUp(tpKey))
						{
							worldtravel::Menu::MenuBeep("BACK", "HUD_FRONTEND_DEFAULT_SOUNDSET");
							break;
						}
						else
						{
							if (bUp)
							{
								worldtravel::Menu::MenuBeep("NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET");
								if (activeLineIndexTeleport == 0)
									activeLineIndexTeleport = lineCount;
								activeLineIndexTeleport--;
								waitTime = 150;
							}
							else
							{
								if (bDown)
								{
									worldtravel::Menu::MenuBeep("NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET");
									activeLineIndexTeleport++;
									if (activeLineIndexTeleport == lineCount)
										activeLineIndexTeleport = 0;
									waitTime = 150;
								}
							}
						}
					}
				}*/
				// Determine destination based on current location
				/*switch (currentLocation)
				{
				case 0:
					if (loadLibertyCity) // Liberty City (if enabled)
						destination = 1; // Liberty City
					break;
				case 1:
					if (loadNorthYankton) // North Yankton (if enabled), or Cayo Perico (if enabled and not in session), or back to Los Santos
					{
						destination = 2;
					}
					break;
				case 2:
					destination = (loadCayoPerico && !NETWORK::NETWORK_IS_IN_SESSION()) ? 3 : 0; // Cayo Perico (if enabled and not in session) or back to Los Santos
					break;
				case 3:
					destination = 0; // Los Santos
					break;
				default:
					destination = 0; // Fallback to Los Santos
					break;
				}

				// Teleport to the determined destination
				if (destination != -1)
				{
					PerformTeleport(player, currentLocation, destination);
				}*/

				int selectedLine = worldtravel::Menu::DrawMenu("Fast Travel", teleportOptions, teleportOptionsAvalible, "HUD_FRONTEND_DEFAULT_SOUNDSET");

				switch (selectedLine)
				{
				case 0:
					destination = 0; // Los Santos
					PerformTeleport(player, currentLocation, destination);
					break;
				case 1:
					destination = 2; // North Yankton
					PerformTeleport(player, currentLocation, destination);
					break;
				case 2:
					destination = 3; // Cayo Perico
					PerformTeleport(player, currentLocation, destination);
					break;
				case 3:
					destination = 1; // Liberty City
					PerformTeleport(player, currentLocation, destination);
					break;
				case 4:
					destination = 4; // Santa Fuegos
					PerformTeleport(player, currentLocation, destination);
				}
			}
		}
	}

	void SwitchMap(int currentLocation, int destination)
	{
		CAM::DO_SCREEN_FADE_OUT(800);
		WAIT(800);
		worldtravel::SetPlayerLocationID(-1);

		// Unload the current map
		switch (currentLocation)
		{
		case 1:
			unloadLiberty();
			libertyCity.UnloadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-3, 0, 0);
			break;
		case 2:
			//unloadYankton();
			northYankton.UnloadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-2, 0, 0);
			break;
		case 3:
			//unloadCayo();
			cayoPerico.UnloadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-5, 0, 0);
			break;
		case 4:
			santaFuegos.UnloadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(-5, 0, 0);
			break;
		default:
			unloadSantos();
			losSantos.UnloadLevel();
			break;
		}

		// Load the new map
		switch (destination)
		{
		case 1: // Liberty City
			loadLiberty();
			libertyCity.LoadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(3, 0, 0);
			else NetworkClockController(true);
			break;
		case 2: // North Yankton
			loadYankton();
			northYankton.LoadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(2, 0, 0);
			break;
		case 3: // Cayo Perico
			loadCayo();
			cayoPerico.LoadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(5, 0, 0);
			break;
		case 4: // Santa Fuegos
			loadFuegos();
			santaFuegos.LoadLevel();
			if (!NETWORK::NETWORK_IS_IN_SESSION()) TIME::ADD_TO_CLOCK_TIME(5, 0, 0);
			break;
		case 0: // Los Santos
			loadSantos();
			losSantos.LoadLevel();
			if (NETWORK::NETWORK_IS_IN_SESSION())
			{
				NetworkClockController(false);
				NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
			}
			break;
		}


		ENTITY::FREEZE_ENTITY_POSITION(player, false);
		worldtravel::SetPlayerLocationID(destination);
		CAM::DO_SCREEN_FADE_IN(800);
	}

	void CharacterSwitchLoadLS()
	{
		int currentLocation = worldtravel::GetPlayerLocationID();
		if (currentLocation > 0)
		{
			if (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
			{
				if (STREAMING::GET_PLAYER_SWITCH_TYPE() == 1)
				{
					if (!NETWORK::NETWORK_IS_IN_SESSION())
					{
						if (STREAMING::GET_PLAYER_SWITCH_STATE() > 4)
						{
							SwitchMap(currentLocation, 0);
							while (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
							{
								WAIT(0);
							}
						}
					}
					else
					{
						if (STREAMING::GET_PLAYER_SWITCH_STATE() > 2)
						{
							SwitchMap(currentLocation, 0);
							while (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
							{
								WAIT(0);
							}
						}
					}
				}
			}
		}
	}

	void RunTick()
	{
		AirportTravel();
		DocksTravel();
		TeleportBetweenMaps();
		FlightController();
		CharacterSwitchLoadLS();
		BlipVisibilityController();
		CreateBlips();
		static bool hasRunOnStartup = false; // Static variable to ensure this runs only once
		if (Settings::EnableLCOnStartup && !hasRunOnStartup)
		{
			hasRunOnStartup = true;
			if (worldtravel::IsLosSantos())
			{
				SwitchMap(0, 1);
				playerPed = PLAYER::PLAYER_PED_ID();
				bPlayerExists = ENTITY::DOES_ENTITY_EXIST(playerPed);
				ENTITY::SET_ENTITY_COORDS(playerPed, 5022.0f, -2644.89f, 15.55f, 1, 0, 0, 1);
			}
		}
	}


	void LevelSwitchMain()
	{
		Settings::Load();
		loadLibertyCity = Settings::EnableLibertyCity;
		loadLibertyLODLights = Settings::EnableLibertyCityLODLights;
		loadNorthYankton = Settings::EnableNorthYankton;
		loadCayoPerico = Settings::EnableCayoPerico;
		loadSantaFuegos = Settings::EnableSantaFuegos;
		fastTravel = Settings::FastTravel;
		restrictedAreaClearance = Settings::GrantAccessToRestrictedAreas;
		worldtravel::RestrictedAreaClearance::SetRestrictedAreaClearance(restrictedAreaClearance);
		tpKey = Settings::TeleportKey;
		initialize();
		while (true)
		{
			playerPed = PLAYER::PLAYER_ID();
			playerPed = PLAYER::PLAYER_PED_ID();
			bPlayerExists = ENTITY::DOES_ENTITY_EXIST(playerPed);
			RunTick();
			WAIT(0);

		}
	}
}

void LevelSwitch()
{
	srand(GetTickCount());
	levelSwitch::LevelSwitchMain();
}