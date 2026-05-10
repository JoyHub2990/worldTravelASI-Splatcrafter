#pragma once
#include <windows.h>

class Settings
{
public:
    static bool EnableLogging;
    static bool FastTravel;
    static int TeleportKey;
    static bool EnableLibertyCity;
    static bool EnableCayoPerico;
    static bool EnableNorthYankton;
    static bool EnableSantaFuegos;
	static bool EnableLibertyCityLODLights;
	static bool EnableLCOnStartup;
    static bool GrantAccessToRestrictedAreas;
    // Liberty City
    static float LC_X;
    static float LC_Y;
    static float LC_Z;
    // North Yankton
    static float NY_X;
    static float NY_Y;
    static float NY_Z;
    // Cayo Perico
    static float CP_X;
    static float CP_Y;
    static float CP_Z;
    // Los Santos
    static float LS_X;
    static float LS_Y;
    static float LS_Z;
    // Santa Fuegos
    static float SF_X;
    static float SF_Y;
    static float SF_Z;

    static void Load();

private:
    static bool LoadBoolSetting(const char* section, const char* key, bool defaultValue, const char* iniFilePath);
    static int LoadIntSetting(const char* section, const char* key, int defaultValue, const char* iniFilePath);
    static float LoadFloatSetting(const char* section, const char* key, float defaultValue, const char* iniFilePath);
    static void ParseCoords(const char* section, const char* key,
        float defaultX, float defaultY, float defaultZ,
        float& outX, float& outY, float& outZ,
        const char* iniFilePath);
};