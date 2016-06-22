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

		ModuleSettings();
	};
}