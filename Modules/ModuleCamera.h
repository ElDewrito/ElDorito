#pragma once

#include "ModuleBase.h"

namespace
{
	std::string VariableCameraCrosshairUpdate(const std::vector<std::string>& Arguments);
	std::string VariableCameraFovUpdate(const std::vector<std::string>& Arguments);
	std::string VariableCameraHideHudUpdate(const std::vector<std::string>& Arguments);
	std::string VariableCameraModeUpdate(const std::vector<std::string>& Arguments);
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