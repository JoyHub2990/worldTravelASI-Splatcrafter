#include "Ipl.h"

using namespace std;



std::string Ipl::GetIplName()
{
	return iplName;
}

void Ipl::SetIplName(const std::string& ipl)
{
	iplName = ipl;
	iplState = STREAMING::IS_IPL_ACTIVE((char*)iplName.c_str());
}



bool Ipl::GetIplState()
{
	return iplState;
}

void Ipl::SetIplState()
{
	iplState = STREAMING::IS_IPL_ACTIVE((char*)iplName.c_str());
}

void Ipl::SetIplState(bool state)
{
	iplState = state;
}



void Ipl::RequestIpl(bool checkState)
{
	if (checkState)
	{
		if (!iplState)
		{
			return;
		}
	}
	STREAMING::REQUEST_IPL((char*)iplName.c_str());
}

void Ipl::RequestIplIfNotActive()
{
	if (!STREAMING::IS_IPL_ACTIVE((char*)iplName.c_str()))
	{
		STREAMING::REQUEST_IPL((char*)iplName.c_str());
	}
}



void Ipl::RemoveIpl(bool saveState)
{
	if (saveState)
		SetIplState();
	STREAMING::REMOVE_IPL((char*)iplName.c_str());
}

void Ipl::RemoveIplIfActive()
{
	if (STREAMING::IS_IPL_ACTIVE((char*)iplName.c_str()))
	{
		STREAMING::REMOVE_IPL((char*)iplName.c_str());
	}
}