#include "ModuleCamera.h"
#include <sstream>
#include "../ElDorito.h"
#include "../ElPreferences.h"
#include "../Patches/Ui.h"

namespace Modules
{
	ModuleCamera::ModuleCamera() : ModuleBase("Camera")
	{
		auto cmd = AddVariableInt("Crosshair", "crosshair", "Controls whether the crosshair should be centered", 0, VariableCameraCrosshairUpdate);
		cmd->ValueIntMin = 0;
		cmd->ValueIntMax = 1;

		cmd = AddVariableFloat("FOV", "fov", "The cameras field of view", 0, VariableCameraFovUpdate);
		cmd->ValueFloatMin = 55.f;
		cmd->ValueFloatMax = 155.f;

		cmd = AddVariableInt("HideHUD", "hud", "Toggles the HUD", 0, VariableCameraHideHudUpdate);
		cmd->ValueIntMin = 0;
		cmd->ValueIntMax = 1;

		//cmd = AddVariableString("Mode", "camera_mode", "Controls the camera mode", "default", VariableCameraModeUpdate);
	}
}

namespace
{
	std::string VariableCameraCrosshairUpdate(const std::vector<std::string>& Arguments)
	{
		unsigned long value;
		if (!Modules::ModuleCamera::Instance().GetVariableInt("Crosshair", value))
			return "";

		std::string status = "disabled.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "enabled.";

		ElPreferences::Instance().setCrosshairCentered(statusBool);
		ElPreferences::Instance().save();

		Patches::Ui::EnableCenteredCrosshairPatch(statusBool);

		return "Centered crosshair " + status;
	}

	std::string VariableCameraFovUpdate(const std::vector<std::string>& Arguments)
	{
		float value;
		if (!Modules::ModuleCamera::Instance().GetVariableFloat("FOV", value))
			return "";

		Pointer::Base(0x1F01D98).Write(value);
		Pointer::Base(0x149D42C).Write(value);

		ElPreferences::Instance().setFieldOfView(value);
		ElPreferences::Instance().save();

		return "FOV set to " + std::to_string(value) + ".";
	}

	std::string VariableCameraHideHudUpdate(const std::vector<std::string>& Arguments)
	{
		unsigned long value;
		if (!Modules::ModuleCamera::Instance().GetVariableInt("HideHud", value))
			return "";

		std::string status = "shown.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "hidden.";

		TODO("Fix this to use a Patch and maybe save it to preferences")
		DWORD temp;
		VirtualProtect(Pointer(0x016B5A5C), 4, PAGE_READWRITE, &temp);

		if (statusBool)
			Pointer(0x016B5A5C).Write<float>(3.14f);
		else
			Pointer(0x016B5A5C).Write<uint32_t>(0x3F83DDE2);

		VirtualProtect(Pointer(0x016B5A5C), 4, temp, NULL);

		return "HUD " + status;
	}

	std::string VariableCameraModeUpdate(const std::vector<std::string>& Arguments)
	{
		return "";
	}
}
