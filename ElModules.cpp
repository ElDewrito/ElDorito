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
	std::string CommandHelp(const std::vector<std::string>& Arguments)
	{
		return Modules::CommandMap::Instance().GenerateHelpText();
	}
}
