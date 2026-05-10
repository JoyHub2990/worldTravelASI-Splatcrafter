
#include <string>
#include <vector>
#include "FileReader.h"
#include "../dependencies/include/natives.h"

using namespace std;
#ifndef WEATHERTYPES
#define WEATHERTYPES
class WeatherTypes
{
public:
	std::vector<std::string> weatherTypeNames;


	void ReadFile(const std::string& filePath);
	void SetWeatherType();
	void ResetWeatherType(bool loadNewWeatherBeforeReset);
};
#endif
