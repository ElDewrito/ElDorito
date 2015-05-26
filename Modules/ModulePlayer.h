#pragma once

#include "ModuleBase.h"

namespace
{
	std::string VariablePlayerNameUpdate(const std::vector<std::string>& Arguments);
}

namespace Modules
{
	class ModulePlayer : public Utils::Singleton<ModulePlayer>, public ModuleBase
	{
	public:
		ModulePlayer();

		wchar_t UserName[17];
	private:
	};
}