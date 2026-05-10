#pragma once

#include <windows.h>
#include "../dependencies/include/natives.h"
#include "../dependencies/include/types.h"
#include "../dependencies/include/enums.h"
#include "../include/main.h"
#include "Settings.h"

namespace worldtravel {
	namespace RestrictedAreaClearance {

		bool HasRestrictedAreaClearance();
		void GetRestrictedAreaClearanceFromIni();
		void SetRestrictedAreaClearance(bool clearance);
	}
}