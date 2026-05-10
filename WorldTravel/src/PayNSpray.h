#pragma once


#include <windows.h>
#include "../dependencies/include/natives.h"
#include "../dependencies/include/types.h"
#include "../dependencies/include/enums.h"
#include "../include/main.h"
#include <string>
#include <vector>
#include "HelpText.h"
#include <ctime>

namespace worldtravel
{
	namespace PayNSpray
	{
		std::string GereratePlate();
		void ResprayVehicle(Vehicle playerVeh, std::string doorName);
		//void PayNSpray(std::string doorHash, std::string doorName, Vector3 garageBoundsMin, Vector3 garageBoundsMax);
		void PayNSpray(std::string doorHash, std::string garage);
		bool IsPayNSprayActive(std::string doorName);
		void DeactivatePayNSpray(std::string doorName);
		void ActivatePayNSpray(std::string doorName, std::string doorObjName, float doorCoordX, float doorCoordY, float doorCoordZ);
	}
}