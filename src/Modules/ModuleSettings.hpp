#pragma once
#include "ModuleBase.hpp"
#include "../Patch.hpp"

namespace Modules
{
	class ModuleSettings : public Utils::Singleton<ModuleSettings>, public ModuleBase
	{
	public:
		Command *VarAntialiasing;
		Command *VarAutoCentering;
		Command *VarBrightness;
		Command *VarDetailsQuality;
		Command *VarDisplayHints;
		Command *VarEffectsQuality;
		Command *VarFullscreen;
		Command *VarHUDShake;
		Command *VarInvertLook;
		Command *VarInvertMouse;
		Command *VarLightingQuality;
		Command *VarLookSensitivity;
		Command *VarMotionBlur;
		Command *VarMouseAcceleration;
		Command *VarMouseFilter;
		Command *VarMouseSensitivityHorizontal;
		Command *VarMouseSensitivityVertical;
		Command *VarMouseSensitivityVehicleHorizontal;
		Command *VarMouseSensitivityVehicleVertical;
		Command *VarPostprocessingQuality;
		Command *VarShadowQuality;
		Command *VarTextureFilteringQuality;
		Command *VarTextureResolution;
		Command *VarToggleCrouch;
		Command *VarVSync;

		ModuleSettings();
	};
}