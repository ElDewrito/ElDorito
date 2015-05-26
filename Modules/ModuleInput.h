#pragma once

#include "ModuleBase.h"

namespace
{
	std::string VariableInputRawInputUpdate(const std::vector<std::string>& Arguments);
}

namespace Modules
{
	class ModuleInput : public Utils::Singleton<ModuleInput>, public ModuleBase
	{
	public:
		ModuleInput();

	private:
	};
}