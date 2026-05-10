#pragma once
#include <windows.h>
#include <string>

class Settings
{
public:
    static bool EnableLogging;
    static char DefaultGroupMap[256];

    static int CayoPericoMinX, CayoPericoMinY, CayoPericoMaxX, CayoPericoMaxY;
    static int LosSantosMinX, LosSantosMinY, LosSantosMaxX, LosSantosMaxY;
    static int LibertyCityMinX, LibertyCityMinY, LibertyCityMaxX, LibertyCityMaxY;

    static void Load();
private:
    static int LoadInt(const char* section, const char* key, int defaultValue, const char* filePath);
    static void LoadString(const char* section, const char* key, const char* defaultValue, char* result, int size, const char* filePath);
};