#include <string>
#include "../dependencies/include/natives.h"

using namespace std;
#ifndef SCENARIO
#define SCENARIO
class Scenario
{
public:
	std::string scenarioName;
	bool scenarioState;

	std::string GetScenarioName();
	void SetScenarioName(const std::string& scenario);

	bool GetScenarioState();
	void SetScenarioState();
	void SetScenarioState(bool state);
	
	void RequestScenario(bool checkState);
	void RequestScenarioIfNotActive();
	
	void RemoveScenario(bool saveState);
	void RemoveScenarioIfActive();
};
#endif