#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleLanDiscoveryListener : public Utils::Singleton<ModuleLanDiscoveryListener>, public ModuleBase
	{
	public:
		ModuleLanDiscoveryListener();
	};
}