#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleForge : public Utils::Singleton<ModuleForge>, public ModuleBase
	{
	public:
		Command* VarCloneDepth;
		Command* VarCloneMultiplier;
		Command* VarRotationSnap;

		ModuleForge();
	};
}