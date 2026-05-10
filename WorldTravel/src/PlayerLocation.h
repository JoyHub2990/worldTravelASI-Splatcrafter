#pragma once

#include <windows.h>
#include "../dependencies/include/natives.h"
#include "../dependencies/include/types.h"
#include "../dependencies/include/enums.h"
#include "../include/main.h"

namespace worldtravel {
	void SetPlayerLocationID(int id);
	int GetPlayerLocationID();
	bool IsLosSantos();
	bool IsLibertyCity();
	bool IsNorthYankton();
	bool IsCayoPerico();
	bool IsSantaFuegos();
}