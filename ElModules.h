#pragma once

#include "Modules/ModuleBase.h"
#include "Modules/ModuleGame.h"
#include "Modules/ModulePlayer.h"
#include "Modules/ModuleServer.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleInput.h"

namespace
{
	bool CommandHelp(const std::vector<std::string>& Arguments, std::string& returnInfo);
}

namespace Modules
{
	class ElModules : public Utils::Singleton<ElModules>, public ModuleBase
	{
	public:
		ElModules();

	private:
	};
}