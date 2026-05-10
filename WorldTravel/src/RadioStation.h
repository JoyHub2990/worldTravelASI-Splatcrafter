#include <string>
#include "../dependencies/include/natives.h"

using namespace std;
#ifndef RADIOSTATION
#define RADIOSTATION
class RadioStation
{
public:
	std::string radioStationName;

	std::string GetRadioStationName();
	void SetRadioStationName(const std::string& radioStation);

	void EnableRadioStation();
	void DisableRadioStation();
};
#endif