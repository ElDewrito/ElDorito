#include "ElModules.h"
#include <fstream>
namespace
{
	bool CommandHelp(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		returnInfo = Modules::CommandMap::Instance().GenerateHelpText();
		return true;
	}

	bool CommandExecute(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() <= 0)
		{
			returnInfo = "Usage: Execute <filename>";
			return false;
		}
		std::ifstream in(Arguments[0], std::ios::in | std::ios::binary);
		if (in && in.is_open())
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize((unsigned int)in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			returnInfo = Modules::CommandMap::Instance().ExecuteCommands(contents);
			return true;
		}
		returnInfo = "Unable to open file " + Arguments[0] + " for reading.";
		return false;
	}
}

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
		AddCommand("Execute", "exec", "Executes a list of commands", CommandExecute, { "filename(string) The list of commands to execute" });
	}
}
