#pragma once

#include <windows.h>
#include "../dependencies/include/natives.h"
#include "../dependencies/include/types.h"
#include "../dependencies/include/enums.h"
#include "../include/main.h"
#include <string>
#include <vector>


namespace worldtravel
{
	namespace Menu
	{
		void DrawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7);
		void DrawMenuLine(std::string caption, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active, bool title, bool rescaleText = true);
		void GetButtonState(bool* a, bool* b, bool* up, bool* down, bool* l, bool* r);
		void MenuBeep(std::string sound, std::string soundGroup);
		void UpdateStatusText();
		void SetStatusText(std::string str, DWORD time = 2500, bool isGxtEntry = false);
		std::string LineAsStr(std::string text, std::string text2);
		void DisableControls(bool toggle);
		void EnableControls(bool toggle);
		int DrawMenu(std::string caption, const std::vector<std::string>& options, const std::vector<bool>& optionAvalible, std::string frontendSoundSet);
	}
}