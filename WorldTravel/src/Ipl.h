#include <string>
#include "../dependencies/include/natives.h"

using namespace std;
#ifndef IPL
#define IPL
class Ipl
{
public: 
	std::string iplName;
	bool iplState;

	std::string GetIplName();
	void SetIplName(const std::string& ipl);

	bool GetIplState();
	void SetIplState();
	void SetIplState(bool state);

	void RequestIpl(bool checkState);
	void RequestIplIfNotActive();
	
	void RemoveIpl(bool saveState);
	void RemoveIplIfActive();
};
#endif