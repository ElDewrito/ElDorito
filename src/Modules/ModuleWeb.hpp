#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleWeb : public Utils::Singleton<ModuleWeb>, public ModuleBase
	{
	public:
		ModuleWeb();
	};
}
