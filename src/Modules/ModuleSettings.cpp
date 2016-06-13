#include "ModuleSettings.hpp"

namespace
{
	auto SSL_GetCharValue = reinterpret_cast<char(__cdecl *)(int)>(0x5047F0);
	auto SSL_GetIntValue = reinterpret_cast<int(__cdecl *)(int)>(0x5042A0);

	auto SetAntialiasing = reinterpret_cast<int(__stdcall *)(int)>(0x79B240);
	auto SetAutoCentering = reinterpret_cast<int(__stdcall *)(char)>(0x79B260);
	auto SetBrightness = reinterpret_cast<int(__stdcall *)(int)>(0x79B290);
	auto SetFullscreen = reinterpret_cast<int(__stdcall *)(int a1)>(0x79B3B0);
	auto SetSFXVolume = reinterpret_cast<int(__stdcall *)(signed int, char)>(0x79B9E0);
	auto SetScreenResolution = reinterpret_cast<int(__stdcall *)(int, int)>(0x79BA30);
	auto SetShadowQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BAD0);
	auto SetTeamColor = reinterpret_cast<int(__stdcall *)(int)>(0x79BAF0);
	auto SetTextureFilteringQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BB10);
	auto SetTextureResolution = reinterpret_cast<int(__stdcall *)(int)>(0x79BB30);
	auto SetToggleCrouch = reinterpret_cast<int(__stdcall *)(int)>(0x79BB50);
	auto SetVSync = reinterpret_cast<int(__stdcall *)(int)>(0x79BB70);
	auto SetVoiceChatControl = reinterpret_cast<int(__stdcall *)(int)>(0x79BB90);
	auto SetVoiceChatVolume = reinterpret_cast<int(__stdcall *)(int, char)>(0x79BBB0);
	auto SetVoiceVolume = reinterpret_cast<int(__stdcall *)(signed int, char)>(0x79BBE0);

	bool VariableSettingsFullscreenUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		unsigned long value = Modules::ModuleSettings::Instance().VarFullscreen->ValueInt;

		std::string status = "disabled.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "enabled.";

		SetFullscreen(statusBool ? 1 : 0);

		returnInfo = "Fullscreen " + status;
		return true;
	}
}

namespace Modules
{
	ModuleSettings::ModuleSettings() : ModuleBase("Settings")
	{
		VarFullscreen = AddVariableInt("Fullscreen", "fullscreen", "Controls whether the game is windowed or fullscreen", eCommandFlagsArchived, 1, VariableSettingsFullscreenUpdate);
	}
}
