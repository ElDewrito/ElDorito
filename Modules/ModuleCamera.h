#pragma once

#include "ModuleBase.h"

namespace
{
	bool VariableCameraCrosshairUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariableCameraFovUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariableCameraHideHudUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariableCameraModeUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
}

namespace Modules
{
	class ModuleCamera : public Utils::Singleton<ModuleCamera>, public ModuleBase
	{
	public:
		Command* VarCameraCrosshair;
		Command* VarCameraFov;
		Command* VarCameraHideHud;
		Command* VarCameraMode;

		ModuleCamera();
	};
}