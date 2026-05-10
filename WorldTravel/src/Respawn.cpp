#include "Respawn.h"
#include <cfloat>
#include <cmath>
#include <fstream>
#include <windows.h>
#include <string>
#include <ctime>
#include <iostream>
#include <vector>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <combaseapi.h>

namespace respawn
{
	//////////////////////////////////////////////////////
	//      Fix to mute game audio when respawning      //
	//////////////////////////////////////////////////////
	
	// this code is needed to ensure your ears are not blown out when respawning by the respawn tune looping 1000 times in one second

	DWORD GetGTA5ProcessID()
	{
		DWORD processID = 0;

		HWND hwnd = FindWindowA(NULL, "Grand Theft Auto V");
		if (hwnd != NULL)
		{
			GetWindowThreadProcessId(hwnd, &processID);
		}

		return processID;
	}

	bool MuteGTA5Audio(bool mute)
	{
		HRESULT hr;
		bool result = false;
		DWORD gta5ProcessID = GetGTA5ProcessID();

		CoInitialize(NULL);

		IMMDeviceEnumerator* pDeviceEnumerator = nullptr;
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDeviceEnumerator));
		if (FAILED(hr)) return result;

		IMMDevice* pDevice = nullptr;
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
		if (SUCCEEDED(hr))
		{
			IAudioSessionManager2* pSessionManager = nullptr;
			hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&pSessionManager);
			if (SUCCEEDED(hr))
			{
				IAudioSessionEnumerator* pSessionEnumerator = nullptr;
				hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);
				if (SUCCEEDED(hr))
				{
					int sessionCount;
					pSessionEnumerator->GetCount(&sessionCount);

					for (int i = 0; i < sessionCount; i++)
					{
						IAudioSessionControl* pSessionControl = nullptr;
						hr = pSessionEnumerator->GetSession(i, &pSessionControl);
						if (SUCCEEDED(hr))
						{
							IAudioSessionControl2* pSessionControl2 = nullptr;
							hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
							if (SUCCEEDED(hr))
							{
								DWORD processId;
								hr = pSessionControl2->GetProcessId(&processId);
								if (SUCCEEDED(hr) && processId == gta5ProcessID)
								{
									ISimpleAudioVolume* pSimpleAudioVolume = nullptr;
									hr = pSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleAudioVolume);
									if (SUCCEEDED(hr))
									{
										pSimpleAudioVolume->SetMute(mute, nullptr);
										pSimpleAudioVolume->Release();
										result = true;
									}
								}
								pSessionControl2->Release();
							}
							pSessionControl->Release();
						}
					}
					pSessionEnumerator->Release();
				}
				pSessionManager->Release();
			}
			pDevice->Release();
		}
		pDeviceEnumerator->Release();

		CoUninitialize();
		return result;
	}
	//////////////////////////////////////////////////////
	//      Fix to mute game audio when respawning      //
	//////////////////////////////////////////////////////


	Player playerPed;
	int respawnType = 0; // 0 = No respawn required, 1 = wasted, 2 = busted

	struct RespawnPoint {
		float x, y, z, heading;
	};

	std::vector<RespawnPoint> hospitalRespawnsLC = {
		{ 4795.89f, -1976.82f, 23.64f, 180.0f }, // North Holland
		{ 5254.93f, -3099.54f, 15.95f, 180.0f }, // Lower Manhattan 1
		{ 3874.92f, -1968.38f, 23.53f, -55.0f }, // North Jersey
		{ 3657.4f, -2884.33f, 23.19f, 90.0f },   // South Jersey
		{ 5039.23f, -3769.83f, 16.71f, 0.0f },   // Lower Manhattan 2
		{ 6388.7f, -3059.42f, 34.6f, 90.0f },    // Brooklyn
		{ 6164.14f, -1433.95f, 22.43f, 90.0f }   // Bronx
	};

	std::vector<RespawnPoint> policeRespawnsLC = {
		{ 4795.89f, -1976.82f, 23.64f, 180.0f }, // North Holland
		{ 5254.93f, -3099.54f, 15.95f, 180.0f }, // Lower Manhattan 1
		{ 3874.92f, -1968.38f, 23.53f, -55.0f }, // North Jersey
		{ 3657.4f, -2884.33f, 23.19f, 90.0f },   // South Jersey
		{ 5039.23f, -3769.83f, 16.71f, 0.0f },   // Lower Manhattan 2
		{ 6388.7f, -3059.42f, 34.6f, 90.0f },    // Brooklyn
		{ 6164.14f, -1433.95f, 22.43f, 90.0f }   // Bronx
	};
	RespawnPoint hospitalRespawnsNY =   { 3359.87f, -4849.52f, 111.67f, 0.0f };
	RespawnPoint policeRespawnsNY =		{ 3359.87f, -4849.52f, 111.67f, 0.0f };
	RespawnPoint respawnCP =			{ 4906.26f, -4912.76f, 3.3600f, 0.0f };

	float respawnRadius = 1500.0f; // Radius to check for closest respawn point

	float GetDistance(float x1, float y1, float z1, float x2, float y2, float z2) {
		return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
	}

	void CustomMapRespawn() {
		while (!CAM::IS_SCREEN_FADED_OUT()) {
			WAIT(0);
		}
		MuteGTA5Audio(true);

		worldtravel::RespawnBlockToggle::SetRespawnBlock(true);
		WAIT(5000);

		PLAYER::SET_PLAYER_WANTED_LEVEL(playerPed, 0, false);
		GAMEPLAY::IGNORE_NEXT_RESTART(true);
		GAMEPLAY::_DISABLE_AUTOMATIC_RESPAWN(true);

		Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(playerPed, true);

		// Find the closest hospital respawn point within the radius
		RespawnPoint closestRespawn;
		float closestDistance = FLT_MAX;
		
		if (worldtravel::IsLibertyCity())
		{
			if (respawnType == 1)
			{
				for (const auto& respawn : hospitalRespawnsLC) {
					float distance = GetDistance(playerPos.x, playerPos.y, playerPos.z, respawn.x, respawn.y, respawn.z);
					if (distance < respawnRadius && distance < closestDistance) {
						closestRespawn = respawn;
						closestDistance = distance;
					}
				}
			}
			else if (respawnType == 2)
			{
				for (const auto& respawn : policeRespawnsLC) {
					float distance = GetDistance(playerPos.x, playerPos.y, playerPos.z, respawn.x, respawn.y, respawn.z);
					if (distance < respawnRadius && distance < closestDistance) {
						closestRespawn = respawn;
						closestDistance = distance;
					}
				}
			}
		}
		else if (worldtravel::IsNorthYankton())
		{
			if (respawnType == 1)
			{
				closestRespawn = hospitalRespawnsNY;
			}
			else if (respawnType == 2)
			{
				closestRespawn = policeRespawnsNY;
			}
		}
		else if (worldtravel::IsCayoPerico())
		{
			closestRespawn = respawnCP;
		}
		// Respawn the player at the closest location found
		NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(closestRespawn.x, closestRespawn.y, closestRespawn.z, closestRespawn.heading, false, false);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, closestRespawn.x, closestRespawn.y, closestRespawn.z, 0, 0, 1);
		ENTITY::SET_ENTITY_HEADING(playerPed, closestRespawn.heading);

		TIME::ADD_TO_CLOCK_TIME(6, 0, 0);
		CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
		CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(0.0f, 0.0f);
		PLAYER::RESET_PLAYER_ARREST_STATE(playerPed);
		ENTITY::SET_ENTITY_HEALTH(playerPed, 200);
		ENTITY::FREEZE_ENTITY_POSITION(playerPed, false);

		worldtravel::RespawnBlockToggle::SetRespawnBlock(false);
		WAIT(5000);

		UI::DISPLAY_HUD(true);
		GRAPHICS::ANIMPOSTFX_STOP_ALL();
		GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();
		GAMEPLAY::_RESET_LOCALPLAYER_STATE();
		WAIT(500);
		CAM::DO_SCREEN_FADE_IN(250);
		MuteGTA5Audio(false);
	}

	void RespawnPlayer()
	{
		playerPed = PLAYER::PLAYER_PED_ID();
		if (!NETWORK::NETWORK_IS_IN_SESSION())
		{
			if (!worldtravel::IsLosSantos())
			{
				if (PED::IS_PED_DEAD_OR_DYING(playerPed, 1))
				{

					respawnType = 1;
					CustomMapRespawn();
				}
				else if (PLAYER::IS_PLAYER_BEING_ARRESTED(playerPed, false))
				{

					respawnType = 2;
					CustomMapRespawn();
				}
			}
		}
	}

	void RespawnMain()
	{
		while (true)
		{
			RespawnPlayer();
			WAIT(0);
		}
	}

}

void Respawn()
{
	srand(GetTickCount());
	respawn::RespawnMain();
}