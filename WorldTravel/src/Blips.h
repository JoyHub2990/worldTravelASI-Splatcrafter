#pragma once

#include <windows.h>
#include "../dependencies/include/natives.h"
#include "../dependencies/include/types.h"
#include "../dependencies/include/enums.h"
#include "../include/main.h"
#include <string>


namespace worldtravel
{
	namespace Blips
	{
		void CreateBlip(Blip& RadarBlip, float x, float y, float z, int scale, int sprite, std::string label, int color, bool flashes, int display, bool shortRange, int category);
		void MoveBlip(Blip& RadarBlip, float x, float y, float z);
		void RelableBlip(Blip& RadarBlip, std::string label);
		void SetBlipDisplay(Blip& RadarBlip, int display);
	}
}