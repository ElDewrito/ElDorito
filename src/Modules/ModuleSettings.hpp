#pragma once
#include "ModuleBase.hpp"
#include "../Patch.hpp"

namespace Modules
{
	class ModuleSettings : public Utils::Singleton<ModuleSettings>, public ModuleBase
	{
	public:
		Command *VarFullscreen;

		ModuleSettings();
	};
}