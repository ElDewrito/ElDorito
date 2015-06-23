#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleInput : public Utils::Singleton<ModuleInput>, public ModuleBase
	{
	public:
		Command* VarInputRawInput;

		ModuleInput();
	};
}