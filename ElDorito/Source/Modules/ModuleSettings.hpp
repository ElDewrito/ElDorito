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
		Command *VarContrast;
		Command *VarDetailsQuality;
		Command *VarDisplayHints;
		Command *VarEffectsQuality;
		Command *VarFullscreen;
		Command *VarScreenResolution;
		Command *VarHUDShake;
		Command *VarInvertLook;
		Command *VarInvertMouse;
		Command *VarLightingQuality;
		Command *VarLookSensitivity;
		Command *VarMotionBlur;
		Command *VarMouseAcceleration;
		Command *VarMouseFilter;
		Command *VarGamepadEnabled;
		Command *VarMouseSensitivityHorizontal;
		Command *VarMouseSensitivityVertical;
		Command *VarMouseSensitivityVehicleHorizontal;
		Command *VarMouseSensitivityVehicleVertical;
		Command *VarPlayerMarkerColors;
		Command *VarPostprocessingQuality;
		Command *VarShadowQuality;
		Command *VarTextureFilteringQuality;
		Command *VarTextureResolution;
		Command *VarToggleCrouch;
		Command *VarVSync;
		Command *VarMasterVolume;
		Command *VarSfxVolume;
		Command *VarMusicVolume;
		Command *VarAudioOutputDevice;
		

		ModuleSettings();

		void GetScreenResolution(int* width, int* height) const;
	};
}
