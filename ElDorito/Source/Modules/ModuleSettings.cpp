#include "ModuleSettings.hpp"
#include "../Patches/Audio.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Patches/Ui.hpp"

namespace
{
	auto SSL_GetCharValue = reinterpret_cast<char(__cdecl *)(int)>(0x5047F0);
	auto SSL_GetIntValue = reinterpret_cast<int(__cdecl *)(int)>(0x5042A0);

	auto SSL_SetAntialiasing = reinterpret_cast<int(__stdcall *)(bool)>(0x79B240);
	auto SSL_SetAutoCentering = reinterpret_cast<int(__stdcall *)(bool)>(0x79B260);
	auto SSL_SetContrast = reinterpret_cast<int(__stdcall *)(int)>(0x79B290);
	auto SSL_SetButtonLayout = reinterpret_cast<int(__stdcall *)(int)>(0x79B2B0);
	auto SSL_SetCameraFOV = reinterpret_cast<int(__stdcall *)(int)>(0x79B300);
	auto SSL_SetControlsMethod = reinterpret_cast<int(__stdcall *)(int)>(0x79B330);
	auto SSL_SetDetailsQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B350);
	auto SSL_SetDisplayHints = reinterpret_cast<int(__stdcall *)(bool)>(0x79B370);
	auto SSL_SetEffectsQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B390);
	auto SSL_SetFullscreen = reinterpret_cast<int(__stdcall *)(int)>(0x79B3B0);
	auto SSL_SetHUDShake = reinterpret_cast<int(__stdcall *)(bool)>(0x79B3D0);
	auto SSL_SetInvertLook = reinterpret_cast<int(__stdcall *)(bool)>(0x79B3F0);
	auto SSL_SetInvertMouse = reinterpret_cast<int(__stdcall *)(bool)>(0x79B420);
	auto SSL_SetKeyBinds = reinterpret_cast<int(__stdcall *)(void *, bool)>(0x79B440);
	auto SSL_SetLightingQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B760);
	auto SSL_SetLookSensitivity = reinterpret_cast<int(__stdcall *)(int)>(0x79B780);
	auto SSL_SetMasterVolume = reinterpret_cast<int(__stdcall *)(int, char)>(0x79B7D0);
	auto SSL_SetMotionBlur = reinterpret_cast<int(__stdcall *)(bool)>(0x79B860);
	auto SSL_SetMouseAcceleration = reinterpret_cast<int(__stdcall *)(int)>(0x79B880);
	auto SSL_SetMouseFilter = reinterpret_cast<int(__stdcall *)(bool)>(0x79B8A0);
	auto SSL_SetMouseSensitivityHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8C0);
	auto SSL_SetMouseSensitivityVehicleHor = reinterpret_cast<int(__stdcall *)(int)>(0x79B8E0);
	auto SSL_SetMouseSensitivityVehicleVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B900);
	auto SSL_SetMouseSensitivityVert = reinterpret_cast<int(__stdcall *)(int)>(0x79B920);
	auto SSL_SetMusicVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B940);
	auto SSL_SetPostprocessingQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79B9C0);
	auto SSL_SetSFXVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79B9E0);
	auto SSL_SetScreenResolution = reinterpret_cast<int(__stdcall *)(int, int)>(0x79BA30);
	auto SSL_SetShadowQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BAD0);
	auto SSL_SetTeamColor = reinterpret_cast<int(__stdcall *)(int)>(0x79BAF0);
	auto SSL_SetTextureFilteringQuality = reinterpret_cast<int(__stdcall *)(int)>(0x79BB10);
	auto SSL_SetTextureResolution = reinterpret_cast<int(__stdcall *)(int)>(0x79BB30);
	auto SSL_SetToggleCrouch = reinterpret_cast<int(__stdcall *)(int)>(0x79BB50);
	auto SSL_SetVSync = reinterpret_cast<int(__stdcall *)(int)>(0x79BB70);
	auto SSL_SetVoiceChatControl = reinterpret_cast<int(__stdcall *)(int)>(0x79BB90);
	auto SSL_SetVoiceChatVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79BBB0);
	auto SSL_SetVoiceVolume = reinterpret_cast<int(__stdcall *)(int, bool)>(0x79BBE0);

	bool TryParseInt(const std::string& str, int* value);
	bool TryParseResolutionString(const std::string& str, int* width, int* height);
	void UpdateScreenResolution();

	bool VariableSettingsAntialiasingUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarAntialiasing->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetAntialiasing(statusBool);

		std::stringstream ss;
		ss << "Antialiasing " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsAutoCenteringUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarAutoCentering->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetAutoCentering(statusBool);

		std::stringstream ss;
		ss << "Auto-Centering " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsBrightnessUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarBrightness->ValueInt;

		if (value < 0 || value > 100)
			return false;

		*reinterpret_cast<int*>(0x022C0128 + 0x41BD8) = static_cast<int>(value);

		std::stringstream ss;
		ss << "Brightness set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsContrastUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarContrast->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetContrast(value);

		std::stringstream ss;
		ss << "Contrast set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsDetailsQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarDetailsQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetDetailsQuality(intValue);

		std::stringstream ss;
		ss << "Details Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsDisplayHintsUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarDisplayHints->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetDisplayHints(statusBool);

		std::stringstream ss;
		ss << "Displaying of hints " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsEffectsQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarEffectsQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetEffectsQuality(intValue);

		std::stringstream ss;
		ss << "Effects Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsFullscreenUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		const auto& moduleSettings = Modules::ModuleSettings::Instance();
		auto value = moduleSettings.VarFullscreen->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetFullscreen(statusBool);
		UpdateScreenResolution();
	
		std::stringstream ss;
		ss << "Fullscreen " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsHUDShakeUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarHUDShake->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetHUDShake(statusBool);

		std::stringstream ss;
		ss << "HUD Shake " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsInvertLookUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarInvertLook->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetInvertLook(statusBool);

		std::stringstream ss;
		ss << "Invert Look " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsInvertMouseUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarInvertMouse->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetInvertMouse(statusBool);

		std::stringstream ss;
		ss << "Invert Mouse " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsLightingQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarLightingQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetLightingQuality(intValue);

		std::stringstream ss;
		ss << "Lighting Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsLookSensitivityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarLookSensitivity->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetLookSensitivity(value);

		std::stringstream ss;
		ss << "Look Sensitivity set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMotionBlurUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMotionBlur->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetMotionBlur(statusBool);

		std::stringstream ss;
		ss << "Motion Blur " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseAccelerationUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseAcceleration->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseAcceleration(value);

		std::stringstream ss;
		ss << "Mouse Acceleration set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseFilterUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseFilter->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetMouseFilter(statusBool);

		std::stringstream ss;
		ss << "Mouse Filter " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityHorizontalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityHorizontal->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityHor(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Horizontal set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityVerticalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityVertical->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityVert(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Vertical set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityVehicleHorizontalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityVehicleHorizontal->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityVehicleHor(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Vehicle Horizontal set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsMouseSensitivityVehicleVerticalUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMouseSensitivityVehicleVertical->ValueInt;

		if (value < 0 || value > 100)
			return false;

		SSL_SetMouseSensitivityVehicleVert(value);

		std::stringstream ss;
		ss << "Mouse Sensitivity Vehicle Vertical set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsPlayerMarkerColorsUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarPlayerMarkerColors->ValueString;

		if (!(value == "default" || value == "blue" || value == "armor"))
			return false;

		auto intValue = value == "default" ? 0 : value == "blue" ? 1 : 2;

		if (intValue == 1)
		{
			Patches::Ui::enableAllyBlueWaypointsFix = true;
			SSL_SetTeamColor(0);
		}
		else
		{ 
			Patches::Ui::enableAllyBlueWaypointsFix = false;
			SSL_SetTeamColor(intValue);
		}

		std::stringstream ss;
		ss << "Player Marker Colors set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsPostprocessingQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarPostprocessingQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetPostprocessingQuality(intValue);

		std::stringstream ss;
		ss << "Postprocessing Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsShadowQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarShadowQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetShadowQuality(intValue);

		std::stringstream ss;
		ss << "Shadow Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsTextureFilteringQualityUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarTextureFilteringQuality->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetTextureFilteringQuality(intValue);

		std::stringstream ss;
		ss << "Texture Filtering Quality set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsTextureResolutionUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarTextureResolution->ValueString;

		if (!(value == "low" || value == "medium" || value == "high"))
			return false;

		auto intValue = value == "low" ? 0 : value == "medium" ? 1 : 2;

		SSL_SetTextureResolution(intValue);

		std::stringstream ss;
		ss << "Texture Resolution set to " << value << ".";
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsToggleCrouchUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarToggleCrouch->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetToggleCrouch(statusBool);

		std::stringstream ss;
		ss << "Toggle Crouch " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsVSyncUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarVSync->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetVSync(statusBool);

		std::stringstream ss;
		ss << "Vertical Sync " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsGamepadEnabledUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarGamepadEnabled->ValueInt;
		auto statusBool = (value != 0);

		if (value < 0 || value > 1)
			return false;

		SSL_SetControlsMethod(value);

		std::stringstream ss;
		ss << "Gamepad " << (statusBool ? "enabled." : "disabled.");
		returnInfo = ss.str();
		return true;
	}

	bool VariableSettingsScreenResolutionUpdate(const std::vector<std::string> &args, std::string &returnInfo)
	{
		const auto& moduleSettings = Modules::ModuleSettings::Instance();
		auto value = moduleSettings.VarScreenResolution->ValueString;

		int width, height;
		if (!TryParseResolutionString(value, &width, &height) && value != "default")
			return false;

		UpdateScreenResolution();

		std::stringstream ss;
		ss << "Screen Resolution set to " << value << ".";
		returnInfo = ss.str();

		return true;
	}

	bool SoundSystemInitialized()
	{
		return *(uint32_t*)(0x069AD06C);
	}

	bool VariableSettingsMasterVolumeUpdated(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMasterVolume->ValueInt;
		if (value < 0 || value > 100)
			return false;

		if (SoundSystemInitialized())
			SSL_SetMasterVolume(value, true);

		std::stringstream ss;
		ss << "Master Volume set to " << value << ".";
		returnInfo = ss.str();

		return true;
	}

	bool VariableSettingsSfxVolumeUpdated(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarSfxVolume->ValueInt;
		if (value < 0 || value > 100)
			return false;

		if (SoundSystemInitialized())
			SSL_SetSFXVolume(value, true);

		std::stringstream ss;
		ss << "Sfx Volume set to " << value << ".";
		returnInfo = ss.str();

		return true;
	}

	bool VariableSettingsMusicVolumeUpdated(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto value = Modules::ModuleSettings::Instance().VarMusicVolume->ValueInt;
		if (value < 0 || value > 100)
			return false;

		if(SoundSystemInitialized())
			SSL_SetMusicVolume(value, true);

		std::stringstream ss;
		ss << "Music Volume set to " << value << ".";
		returnInfo = ss.str();

		return true;
	}

	bool VariableAudioOutputDeviceUpdated(const std::vector<std::string> &args, std::string &returnInfo)
	{
		if (!SoundSystemInitialized())
			return true;

		int device;
		if (args.size() < 1 || !TryParseInt(args[0], &device) || device < 0)
			return false;


		if (!Patches::Audio::SetOutputDevice(device))
		{
			returnInfo = "Error setting audo output device.";
			return false;
		}

		std::stringstream ss;
		ss << "Audio output device set to " << device << ".";
		returnInfo = ss.str();

		return true;
	}

	bool CommandAudioOutputDeviceList(const std::vector<std::string> &args, std::string &returnInfo)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartArray();

		WAVEOUTCAPS caps = {};
		auto numDevs = waveOutGetNumDevs();
		for (auto i = 0u; i < numDevs; i++)
		{	
			if (waveOutGetDevCaps(i, &caps, sizeof(caps)) != MMSYSERR_NOERROR)
				continue;

			writer.String(caps.szPname);	
		}
		writer.EndArray();

		returnInfo = buffer.GetString();
		return true;
	}

	bool CommandResetSettings(const std::vector<std::string> &args, std::string &returnInfo)
	{
		auto &commandMap = Modules::CommandMap::Instance();

		for (auto &cmd : commandMap.Commands)
		{
			if (!(cmd.Flags & CommandFlags::eCommandFlagsArchived)
				|| cmd.Flags & eCommandFlagsNoReset
				|| cmd.Flags & eCommandFlagsWriteToKeys)
			{
				continue;
			}

			switch (cmd.Type)
			{
			case Modules::CommandType::eCommandTypeCommand:
				continue;
			case Modules::CommandType::eCommandTypeVariableFloat:
				cmd.ValueFloat = cmd.DefaultValueFloat;
				cmd.ValueString = std::to_string(cmd.DefaultValueFloat);
				break;
			case Modules::CommandType::eCommandTypeVariableInt:
				cmd.ValueInt = cmd.DefaultValueInt;
				cmd.ValueString = std::to_string(cmd.ValueInt);
				break;
			case Modules::CommandType::eCommandTypeVariableInt64:
				cmd.ValueInt64 = cmd.DefaultValueInt64;
				cmd.ValueString = std::to_string(cmd.DefaultValueInt64);
				break;
			case Modules::CommandType::eCommandTypeVariableString:
				cmd.ValueString = cmd.DefaultValueString;
				break;
			}

			if (cmd.UpdateEvent)
			{
				std::string returnInfo;
				cmd.UpdateEvent({ cmd.ValueString }, returnInfo);
			}

			commandMap.NotifyVariableUpdated(&cmd);
		}

		returnInfo = "Default settings restored";
		return true;
	}
}

namespace Modules
{
	ModuleSettings::ModuleSettings() : ModuleBase("Settings")
	{
		VarAntialiasing = AddVariableInt("Antialiasing", "antialiasing", "Controls whether antialiasing is enabled (1) or disabled (0)", eCommandFlagsArchived, 1, VariableSettingsAntialiasingUpdate);
		VarAutoCentering = AddVariableInt("AutoCentering", "autocenter", "Controls whether auto-centering is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsAutoCenteringUpdate);
		VarBrightness = AddVariableInt("Brightness", "brightness", "Controls the brightness level (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsBrightnessUpdate);
		VarContrast = AddVariableInt("Contrast", "Contrast", "Controls the contrast level (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsContrastUpdate);
		VarDetailsQuality = AddVariableString("DetailsQuality", "details", "Controls whether the details quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsDetailsQualityUpdate);
		VarDisplayHints = AddVariableInt("DisplayHints", "hints", "Controls whether displaying of hints is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsDisplayHintsUpdate);
		VarEffectsQuality = AddVariableString("EffectsQuality", "effects", "Controls whether the effects Quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsEffectsQualityUpdate);
		VarFullscreen = AddVariableInt("Fullscreen", "fullscreen", "Controls whether the game is windowed (0) or fullscreen (1)", eCommandFlagsArchived, 1, VariableSettingsFullscreenUpdate);
		VarScreenResolution = AddVariableString("ScreenResolution", "resolution", "Controls the screen resolution", eCommandFlagsArchived, "default", VariableSettingsScreenResolutionUpdate);
		VarHUDShake = AddVariableInt("HUDShake", "hud_shake", "Controls whether hud shake is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsHUDShakeUpdate);
		VarInvertLook = AddVariableInt("InvertLook", "invert_look", "Controls whether look-inversion is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsInvertLookUpdate);
		VarInvertMouse = AddVariableInt("InvertMouse", "invert_mouse", "Controls whether mouse-inversion is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsInvertMouseUpdate);
		VarLightingQuality = AddVariableString("LightingQuality", "lighting", "Controls whether the lighting quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsLightingQualityUpdate);
		VarLookSensitivity = AddVariableInt("LookSensitivity", "look_sensitivity", "Controls the controller look sensitivity (0 - 100)", eCommandFlagsArchived, 50, VariableSettingsLookSensitivityUpdate);
		VarMotionBlur = AddVariableInt("MotionBlur", "motion_blur", "Controls whether motion blur is enabled (1) or disabled (0)", eCommandFlagsArchived, 1, VariableSettingsMotionBlurUpdate);
		VarMouseAcceleration = AddVariableInt("MouseAcceleration", "mouse_accel", "Controls the level of mouse acceleration (0 - 100)", eCommandFlagsArchived, 0, VariableSettingsMouseAccelerationUpdate);
		VarMouseFilter = AddVariableInt("MouseFilter", "mouse_filter", "Controls whether mouse-filtering is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsMouseFilterUpdate);
		VarGamepadEnabled = AddVariableInt("Gamepad", "gamepad", "Controls whether gamepad is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsGamepadEnabledUpdate);
		VarMouseSensitivityHorizontal = AddVariableInt("MouseSensitivityHorizontal", "mouse_sensitivity_hor", "Controls the horizontal mouse sensitivity (0 - 100)", eCommandFlagsArchived, 25, VariableSettingsMouseSensitivityHorizontalUpdate);
		VarMouseSensitivityVertical = AddVariableInt("MouseSensitivityVertical", "mouse_sensitivity_vert", "Controls the vertical mouse sensitivity (0 - 100)", eCommandFlagsArchived, 25, VariableSettingsMouseSensitivityVerticalUpdate);
		VarMouseSensitivityVehicleHorizontal = AddVariableInt("MouseSensitivityVehicleHorizontal", "mouse_sensitivity_vehicle_hor", "Controls the horizontal mouse sensitivity for vehicles (0 - 100)", eCommandFlagsArchived, 25, VariableSettingsMouseSensitivityVehicleHorizontalUpdate);
		VarMouseSensitivityVehicleVertical = AddVariableInt("MouseSensitivityVehicleVertical", "mouse_sensitivity_vehicle_vert", "Controls the vertical mouse sensitivity for vehicles (0 - 100)", eCommandFlagsArchived, 25, VariableSettingsMouseSensitivityVehicleVerticalUpdate);
		VarPlayerMarkerColors = AddVariableString("PlayerMarkerColors", "markers", "Controls whether the player marker colors are default, ally or armor", eCommandFlagsArchived, "default", VariableSettingsPlayerMarkerColorsUpdate);
		VarPostprocessingQuality = AddVariableString("PostprocessingQuality", "postprocessing", "Controls whether the postprocessing quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsPostprocessingQualityUpdate);
		VarShadowQuality = AddVariableString("ShadowQuality", "shadows", "Controls whether the shadow quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsShadowQualityUpdate);
		VarTextureFilteringQuality = AddVariableString("TextureFilteringQuality", "texture_filtering", "Controls whether the texture-filtering quality level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsTextureFilteringQualityUpdate);
		VarTextureResolution = AddVariableString("TextureResolution", "textures", "Controls whether the texture resolution level is low, medium or high", eCommandFlagsArchived, "high", VariableSettingsTextureResolutionUpdate);
		VarToggleCrouch = AddVariableInt("ToggleCrouch", "toggle_crouch", "Controls whether crouch-toggling is enabled (1) or disabled (0)", eCommandFlagsArchived, 0, VariableSettingsToggleCrouchUpdate);
		VarVSync = AddVariableInt("VSync", "vsync", "Controls whether vertical sync is enabled (1) or disabled (0)", eCommandFlagsArchived, 1, VariableSettingsVSyncUpdate);
		VarMasterVolume = AddVariableInt("MasterVolume", "volume", "Controls the master volume", CommandFlags(eCommandFlagsArchived | eCommandFlagsDontUpdateInitial), 100, VariableSettingsMasterVolumeUpdated);
		VarSfxVolume = AddVariableInt("SfxVolume", "volume_sfx", "Controls the sfx volume", CommandFlags(eCommandFlagsArchived | eCommandFlagsDontUpdateInitial), 100, VariableSettingsSfxVolumeUpdated);
		VarMusicVolume = AddVariableInt("MusicVolume", "volume_music", "Controls the music volume", CommandFlags(eCommandFlagsArchived | eCommandFlagsDontUpdateInitial), 100, VariableSettingsMusicVolumeUpdated);
		VarAudioOutputDevice = AddVariableInt("AudioOutputDevice", "audio_out", "Sets the audio output device to use (0) being system default", eCommandFlagsArchived, 0, VariableAudioOutputDeviceUpdated);

		AddCommand("AudioOutputDeviceList", "audio_out_devs", "List available audio output devices", eCommandFlagsNone, CommandAudioOutputDeviceList);
		AddCommand("Reset", "settings_reset", "Restores default settings", eCommandFlagsNone, CommandResetSettings);
	}

	void ModuleSettings::GetScreenResolution(int* width, int* height) const
	{
		const auto& moduleSettings = Modules::ModuleSettings::Instance();
		const auto& resolutionStr = moduleSettings.VarScreenResolution->ValueString;

		if (!TryParseResolutionString(resolutionStr, width, height))
		{
			auto fullScreen = Modules::ModuleSettings::Instance().VarFullscreen->ValueInt;
			if (fullScreen)
			{
				*width = GetSystemMetrics(SM_CXSCREEN);
				*height = GetSystemMetrics(SM_CYSCREEN);
			}
			else
			{
				*width = 1280;
				*height = 720;
			}
		}
	}
}

namespace
{
	void UpdateScreenResolution()
	{
		auto gameWindow = *(uint32_t*)0x0199C014;
		if (!gameWindow)
			return;

		const auto& moduleSettings = Modules::ModuleSettings::Instance();

		int width, height;
		moduleSettings.GetScreenResolution(&width, &height);

		static auto GetNewDisplayPreferences = (void*(*)())(0xA1FC60);
		static auto GetDisplayPreferences = (void*(*)())(0x00A1FC50);
		auto newPrefs = GetNewDisplayPreferences();
		memcpy(newPrefs, GetDisplayPreferences(), 0x38u);

		*(int32_t *)newPrefs = width;
		*((int32_t*)newPrefs + 1) = height;
		*(uint8_t*)0x050DD9D1 = 1; // dirty
	}

	bool TryParseInt(const std::string& str, int* value)
	{
		if (str.length() == 0)
			return false;

		auto c_str = str.c_str();
		char* endp;

		*value = std::strtol(c_str, &endp, 10);

		return endp != c_str;
	}

	bool TryParseResolutionString(const std::string& str, int* width, int* height)
	{
		auto separatorPos = str.find("x");
		if (separatorPos != std::string::npos && separatorPos + 1 < str.length())
		{
			auto widthString = str.substr(0, separatorPos);
			auto heightString = str.substr(separatorPos + 1);

			return TryParseInt(widthString, width) && TryParseInt(heightString, height);
		}

		return false;
	}
}
