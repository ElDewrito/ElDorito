#include "ModuleCamera.h"
#include <sstream>
#include "../ElDorito.h"
#include "../Patches/Ui.h"

namespace Modules
{
	ModuleCamera::ModuleCamera() : ModuleBase("Camera")
	{
		VarCameraCrosshair = AddVariableInt("Crosshair", "crosshair", "Controls whether the crosshair should be centered", 0, VariableCameraCrosshairUpdate);
		VarCameraCrosshair->ValueIntMin = 0;
		VarCameraCrosshair->ValueIntMax = 1;

		VarCameraFov = AddVariableFloat("FOV", "fov", "The cameras field of view", 90.f, VariableCameraFovUpdate);
		VarCameraFov->ValueFloatMin = 55.f;
		VarCameraFov->ValueFloatMax = 155.f;

		VarCameraHideHud = AddVariableInt("HideHUD", "hud", "Toggles the HUD", 0, VariableCameraHideHudUpdate);
		VarCameraHideHud->ValueIntMin = 0;
		VarCameraHideHud->ValueIntMax = 1;

		//this->VarCameraMode = AddVariableString("Mode", "camera_mode", "Controls the camera mode", "default", VariableCameraModeUpdate);
	}
}

namespace
{
	bool VariableCameraCrosshairUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		unsigned long value = Modules::ModuleCamera::Instance().VarCameraCrosshair->ValueInt;

		std::string status = "disabled.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "enabled.";

		Patches::Ui::EnableCenteredCrosshairPatch(statusBool);

		returnInfo = "Centered crosshair " + status;
		return true;
	}

	bool VariableCameraFovUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		float value = Modules::ModuleCamera::Instance().VarCameraFov->ValueFloat;

		Pointer::Base(0x1F01D98).Write(value);
		Pointer::Base(0x149D42C).Write(value);

		return true;
	}

	bool VariableCameraHideHudUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		unsigned long value = Modules::ModuleCamera::Instance().VarCameraHideHud->ValueInt;

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

		returnInfo = "HUD " + status;
		return true;
	}

	bool VariableCameraModeUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return false;
	}
}
