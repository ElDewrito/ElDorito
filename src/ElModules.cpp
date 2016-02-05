#include "ElModules.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
namespace
{
	bool CommandHelp(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto& commandMap = Modules::CommandMap::Instance();
		if (Arguments.size() > 0)
		{
			auto cmdName = Arguments[0];
			auto* cmd = commandMap.FindCommand(cmdName);
			if (!cmd)
			{
				// try searching for it as a module
				bool isModule = false;
				for (auto it = commandMap.Commands.begin(); it < commandMap.Commands.end(); it++)
					if (it->ModuleName.length() > 0 && !_stricmp(it->ModuleName.c_str(), cmdName.c_str()))
					{
						isModule = true;
						break;
					}

				if (isModule)
					returnInfo = commandMap.GenerateHelpText(cmdName);
				else
					returnInfo = "Command/Variable not found";

				return isModule;
			}
			returnInfo = cmd->GenerateHelpText();
			return true;
		}
		returnInfo = commandMap.GenerateHelpText();
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
			returnInfo = Modules::CommandMap::Instance().ExecuteCommands(contents, true);
			return true;
		}
		returnInfo = "Unable to open file " + Arguments[0] + " for reading.";
		return false;
	}

	bool CommandWriteConfig(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::string prefsName = "dewrito_prefs.cfg";
		if (Arguments.size() > 0)
			prefsName = Arguments[0];

		std::ofstream outFile(prefsName, std::ios::trunc);
		if (outFile.fail())
		{
			returnInfo = "Failed to write config to " + prefsName + "!";
			return false;
		}
		outFile << Modules::CommandMap::Instance().SaveVariables();
		outFile << "\n" << Modules::ModuleInput::Instance().ExportBindings();

		returnInfo = "Wrote config to " + prefsName;
		return true;
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
		ModuleIRC::Instance();
		ModuleVoIP::Instance();
		ModuleTime::Instance();
		ModuleGraphics::Instance();

		AddCommand("Help", "help", "Displays this help text", eCommandFlagsNone, CommandHelp);
		AddCommand("Execute", "exec", "Executes a list of commands", eCommandFlagsNone, CommandExecute, { "filename(string) The list of commands to execute" });
		AddCommand("WriteConfig", "config_write", "Writes the ElDewrito config file", eCommandFlagsNone, CommandWriteConfig, { "filename(string) Optional, the filename to write the config to" });

		CommandMap::Instance().FinishAddCommands(); // call this so that the default values can be applied to the game
	}
}
