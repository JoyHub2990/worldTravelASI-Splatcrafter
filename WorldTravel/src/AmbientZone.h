#include <string>
#include "../dependencies/include/natives.h"

using namespace std;
#ifndef AMBIENTZONE
#define AMBIENTZONE
class AmbientZone
{
public:
	std::string ambientZoneName;
	bool ambientZoneState;

	std::string GetAmbientZoneName();
	void SetAmbientZoneName(const std::string& ambientZone);

	bool GetAmbientZoneState();
	void SetAmbientZoneState();
	void SetAmbientZoneState(bool state);

	void RequestAmbientZone(bool checkState);
	void RequestAmbientZoneIfNotActive();

	void RemoveAmbientZone(bool saveState);
	void RemoveAmbientZoneIfActive();
};
#endif