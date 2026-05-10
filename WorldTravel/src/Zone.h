#include <string>
#include "../dependencies/include/natives.h"

using namespace std;
#ifndef MAPZONE
#define MAPZONE
class Zone
{
public:
	std::string zoneName;
	int zoneId;

	std::string GetZoneName();
	void SetZoneName(const std::string& zone);

	int GetZoneId();
	
	void EnableZone();
	void DisableZone();
};
#endif