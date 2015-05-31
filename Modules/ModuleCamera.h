#pragma once

#include "ModuleBase.h"

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