#pragma once

#include "ModuleBase.h"

namespace
{
	bool VariableInputRawInputUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
}

namespace Modules
{
	class ModuleInput : public Utils::Singleton<ModuleInput>, public ModuleBase
	{
	public:
		Command* VarInputRawInput;

		ModuleInput();
	};
}