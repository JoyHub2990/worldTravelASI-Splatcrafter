#pragma once

#include <windows.h>
#include "../dependencies/include/natives.h"
#include "../dependencies/include/types.h"
#include "../dependencies/include/enums.h"
#include "../include/main.h"

namespace worldtravel {
	namespace WaterState {
		int GetWaterState();
		void SetWaterState(int s);
	}
}