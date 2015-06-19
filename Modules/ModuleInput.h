#pragma once

#include "ModuleBase.h"

namespace Modules
{
	class ModuleInput : public Utils::Singleton<ModuleInput>, public ModuleBase
	{
	public:
		Command* VarInputRawInput;

		ModuleInput();
	};
}