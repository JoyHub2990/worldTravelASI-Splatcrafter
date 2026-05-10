#include "IplGroup.h"

using namespace std;





void IplGroup::ReadFile(const std::string& filePath)
{
	worldtravel::file::readFileToVector(filePath, groupIplNames);
	for (int i = 0; i < groupIplNames.size(); i++)
	{
		Ipl temp;
		groupIpls.push_back(temp);
		groupIpls[i].SetIplName(groupIplNames[i]);
	}
	/*for (const auto& ipl : groupIplNames)
	{
		groupIpls[ipl].SetIplName(groupIplNames[ipl]);
	}*/

}



void IplGroup::RequestIplGroup(bool checkState)
{
	for (int i = 0; i < groupIplNames.size(); i++)
	{
		groupIpls[i].RequestIpl(checkState);
	}
}
			
void IplGroup::RequestIplGroupIfNotActive()
{
	for (int i = 0; i < groupIplNames.size(); i++)
	{
		groupIpls[i].RequestIplIfNotActive();
	}
}


			
void IplGroup::RemoveIplGroup(bool checkState)
{
	for (int i = 0; i < groupIplNames.size(); i++)
	{
		groupIpls[i].RemoveIpl(checkState);
	}
}

void IplGroup::RemoveIplGroupIfActive()
{
	for (int i = 0; i < groupIplNames.size(); i++)
	{
		groupIpls[i].RemoveIplIfActive();
	}
}