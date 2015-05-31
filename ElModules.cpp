#include "ElModules.h"

namespace Modules
{
	ElModules::ElModules() : ModuleBase("")
	{
		ModuleGame::Instance();
		ModulePlayer::Instance();
		ModuleServer::Instance();
		ModuleCamera::Instance();
		ModuleInput::Instance();

		AddCommand("Help", "help", "Displays this help text", CommandHelp);
	}
}

namespace
{
	bool CommandHelp(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		returnInfo = Modules::CommandMap::Instance().GenerateHelpText();
		return true;
	}
}
