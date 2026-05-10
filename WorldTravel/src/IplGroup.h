
#include "Ipl.h"
#include <string>
#include <vector>
#include "FileReader.h"

using namespace std;
#ifndef IPLGROUP
#define IPLGROUP
class IplGroup
{
public:
	std::vector<Ipl> groupIpls;
	std::vector<std::string> groupIplNames;


	void ReadFile(const std::string& filePath);
	void RequestIplGroup(bool checkState);
	void RequestIplGroupIfNotActive();
	void RemoveIplGroup(bool checkState);
	void RemoveIplGroupIfActive();
};
#endif
