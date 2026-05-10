#include <windows.h>
#include "Hooking.Patterns.h"
#include "Farlods.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "../include/main.h"
#include "Entrances.h"
#include "Helpers.h"
#include "LevelSwitch.h"
#include "Minimap.h"
#include "Shops.h"
#include "Respawn.h"
#include "RespawnBlock.h"
#include "RestrictedAreas.h"
#include "keyboard.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <iostream>
#include <tlhelp32.h>
#include "Settings.h"
#include <fstream>
#include "ToggleFogVolume.h"
#include "GameVersion.h"


//////////////////////////////////////////////////////
// Fix to make sure audio is not muted from respawn //
//////////////////////////////////////////////////////

DWORD GetGTA5ProcessID()
{
    DWORD processID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            if (_stricmp(pe32.szExeFile, "GTA5.exe") == 0)
            {
                processID = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processID;
}

bool WaitForGTA5Process(int maxRetries, int delayMilliseconds)
{
    spdlog::info("Waiting for GTA5 process to start...");

    for (int attempt = 1; attempt <= maxRetries; ++attempt)
    {
        DWORD processID = GetGTA5ProcessID();
        if (processID != 0)
        {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }
    return false;
}

bool MuteGTA5Audio(bool mute)
{
    if (!WaitForGTA5Process(10, 2500)) // Retry up to 10 times with 2.5 second delay
    {
        spdlog::error("Cannot mute/unmute GTA5 audio because the process was not found.");
        return false;
    }

    HRESULT hr;
    bool result = false;
    DWORD gta5ProcessID = GetGTA5ProcessID();

    CoInitialize(NULL);

    IMMDeviceEnumerator* pDeviceEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDeviceEnumerator));
    if (FAILED(hr))
    {
        CoUninitialize();
        return result;
    }

    IMMDevice* pDevice = nullptr;
    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (SUCCEEDED(hr))
    {
        IAudioSessionManager2* pSessionManager = nullptr;
        hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&pSessionManager);
        if (SUCCEEDED(hr))
        {
            int maxRetries = 10;
            int delayMilliseconds = 2500;
            bool sessionFound = false;

            for (int retry = 1; retry <= maxRetries; ++retry)
            {
                IAudioSessionEnumerator* pSessionEnumerator = nullptr;
                hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);
                if (SUCCEEDED(hr))
                {
                    int sessionCount = 0;
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
                                DWORD processId = 0;
                                hr = pSessionControl2->GetProcessId(&processId);
                                if (SUCCEEDED(hr))
                                {
                                    if (processId == gta5ProcessID)
                                    {
                                        ISimpleAudioVolume* pSimpleAudioVolume = nullptr;
                                        hr = pSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSimpleAudioVolume);
                                        if (SUCCEEDED(hr))
                                        {
                                            hr = pSimpleAudioVolume->SetMute(mute, nullptr);
                                            if (SUCCEEDED(hr))
                                            {
                                                result = true;
                                                sessionFound = true;
                                            }
                                            pSimpleAudioVolume->Release();
                                        }
                                    }
                                }
                                pSessionControl2->Release();
                            }
                            pSessionControl->Release();
                        }
                    }
                    pSessionEnumerator->Release();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
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
// Fix to make sure audio is not muted from respawn //
//////////////////////////////////////////////////////



DWORD WINAPI Main()
{
    worldtravel::gameversion::SetGameVersion();
    Settings::Load();

    if (Settings::EnableLogging)
    {
        spdlog::set_level(spdlog::level::info);
        spdlog::set_default_logger(spdlog::basic_logger_mt("WorldTravel", "WorldTravel.log"));
        spdlog::flush_every(std::chrono::seconds(5));
    }
    else
    {
        spdlog::set_level(spdlog::level::off);
    }

    patches::farlods::Init();
    spdlog::info("Farlods Patch Done!");
    Minimap_Hooks();
    spdlog::info("Minimap Hooks Set.");
    ToggleFogVolumeOff();

    return 1;
}




BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Main, NULL, NULL, NULL));

        scriptRegister(hModule, LevelSwitch);
        scriptRegister(hModule, Entrances);
        scriptRegister(hModule, Helpers);
        scriptRegister(hModule, Minimap);
        scriptRegister(hModule, Respawn);
        scriptRegister(hModule, RespawnBlock);
        scriptRegister(hModule, RestrictedAreas);
        scriptRegister(hModule, Shops);
        keyboardHandlerRegister(OnKeyboardMessage);
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        spdlog::shutdown();
        scriptUnregister(hModule);
        keyboardHandlerUnregister(OnKeyboardMessage);
    }

    return TRUE;
}