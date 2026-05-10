#include <string>
#include "../dependencies/include/natives.h"

using namespace std;
#ifndef ZONEDMODEL
#define ZONEDMODEL
class ZonedModel
{
public:
	std::string zonedModelName;

	std::string GetZonedModelName();
	void SetZonedModelName(const std::string& zonedModelName);

	void SetZonedModelSuppressed(bool isSuppressed, bool type);
};
#endif