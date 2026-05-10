#include "Settings.h"
#include <windows.h>
#include <cstring>

bool Settings::EnableLogging = true;
char Settings::DefaultGroupMap[256] = {};

int Settings::CayoPericoMinX = 2700;
int Settings::CayoPericoMinY = -7150;
int Settings::CayoPericoMaxX = 6700;
int Settings::CayoPericoMaxY = -3150;

int Settings::LosSantosMinX = -4000;
int Settings::LosSantosMinY = -4000;
int Settings::LosSantosMaxX = 4500;
int Settings::LosSantosMaxY = 8000;

int Settings::LibertyCityMinX = 2200;
int Settings::LibertyCityMinY = -6300;
int Settings::LibertyCityMaxX = 8200;
int Settings::LibertyCityMaxY = -300;

void Settings::Load()
{
    char iniFilePath[MAX_PATH];
    GetFullPathName("WorldTravelPatches.ini", MAX_PATH, iniFilePath, nullptr);

    // Load general settings
    EnableLogging = LoadInt("WorldTravelPatches", "EnableLogging", 0, iniFilePath) != 0;
    LoadString("WorldTravelPatches", "DefaultGroupMap", "GROUP_MAP_SP", DefaultGroupMap, sizeof(DefaultGroupMap), iniFilePath);

    // Load water settings for each map
    CayoPericoMinX = LoadInt("Cayo_Perico", "min_x", CayoPericoMinX, iniFilePath);
    CayoPericoMinY = LoadInt("Cayo_Perico", "min_y", CayoPericoMinY, iniFilePath);
    CayoPericoMaxX = LoadInt("Cayo_Perico", "max_x", CayoPericoMaxX, iniFilePath);
    CayoPericoMaxY = LoadInt("Cayo_Perico", "max_y", CayoPericoMaxY, iniFilePath);

    LosSantosMinX = LoadInt("Los_Santos", "min_x", LosSantosMinX, iniFilePath);
    LosSantosMinY = LoadInt("Los_Santos", "min_y", LosSantosMinY, iniFilePath);
    LosSantosMaxX = LoadInt("Los_Santos", "max_x", LosSantosMaxX, iniFilePath);
    LosSantosMaxY = LoadInt("Los_Santos", "max_y", LosSantosMaxY, iniFilePath);

    LibertyCityMinX = LoadInt("Liberty_City", "min_x", LibertyCityMinX, iniFilePath);
    LibertyCityMinY = LoadInt("Liberty_City", "min_y", LibertyCityMinY, iniFilePath);
    LibertyCityMaxX = LoadInt("Liberty_City", "max_x", LibertyCityMaxX, iniFilePath);
    LibertyCityMaxY = LoadInt("Liberty_City", "max_y", LibertyCityMaxY, iniFilePath);
}

int Settings::LoadInt(const char* section, const char* key, int defaultValue, const char* filePath)
{
    return GetPrivateProfileInt(section, key, defaultValue, filePath);
}

void Settings::LoadString(const char* section, const char* key, const char* defaultValue, char* result, int size, const char* filePath)
{
    DWORD n = GetPrivateProfileString(section, key, defaultValue, result, size, filePath);
    if (n <= 0)
    {
        strcpy_s(result, size, defaultValue);
    }
}