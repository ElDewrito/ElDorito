#include "CommandMap.hpp"
#include <algorithm>
#include <sstream>
#include "ElDorito.hpp"
#include "Blam\BlamNetwork.hpp"


namespace Modules
{
	Command::Command() : UpdateEvent(0), ValueInt(0), ValueFloat(0.f), DefaultValueInt(0), DefaultValueFloat(0.f), ValueIntMin(0), ValueIntMax(0), ValueFloatMin(0.f), ValueFloatMax(0.f)
	{
	}

	std::string Command::GenerateHelpText()
	{
		std::stringstream ss;
		ss << Name << " - " << Description << "." << std::endl;
		ss << "Usage: " << Name << " ";
		if (Type != eCommandTypeCommand)
		{
			ss << "<value(";
			switch (Type)
			{
			case eCommandTypeVariableInt:
				ss << "int";
				break;
			case eCommandTypeVariableInt64:
				ss << "int64";
				break;
			case eCommandTypeVariableFloat:
				ss << "float";
				break;
			case eCommandTypeVariableString:
				ss << "string";
				break;
			}
			ss << ")>" << std::endl << "Current value: " << ValueString << std::endl << std::endl;
		}
		else
		{
			std::stringstream paramStream;
			for (auto arg : CommandArgs)
			{
				auto argname = arg;
				std::string argdesc = "";
				auto nameEnd = arg.find(" ");
				if (nameEnd != std::string::npos)
				{
					argname = arg.substr(0, nameEnd);
					if (arg.length() > (nameEnd + 1))
						argdesc = arg.substr(nameEnd + 1);
				}
				ss << "<" << argname << "> ";
				if (argdesc.length() > 0)
					paramStream << "  " << argname << ": " << argdesc << std::endl;
			}
			ss << std::endl;
			auto paramDescs = paramStream.str();
			if (paramDescs.length() > 0)
				ss << paramDescs << std::endl;
			else
				ss << std::endl;
		}

		return ss.str();
	}

	Command* CommandMap::FindCommand(const std::string& name)
	{
		for (auto it = Commands.begin(); it < Commands.end(); it++)
		if ((it->Name.length() > 0 && !_stricmp(it->Name.c_str(), name.c_str())) || (it->ShortName.length() > 0 && !_stricmp(it->ShortName.c_str(), name.c_str())))
			return &(*it);

		return nullptr;
	}

	Command* CommandMap::AddCommand(Command command)
	{
		if (FindCommand(command.Name) || FindCommand(command.ShortName))
			return nullptr;

		this->Commands.push_back(command);

		return &this->Commands.back();
	}

	void CommandMap::FinishAddCommands()
	{
		for (auto command : Commands)
		{
			if (command.Type != eCommandTypeCommand && (command.Flags & eCommandFlagsDontUpdateInitial) != eCommandFlagsDontUpdateInitial)
				if (command.UpdateEvent)
					command.UpdateEvent(std::vector<std::string>(), std::string());
		}
	}

	std::string CommandMap::ExecuteCommand(std::vector<std::string> command, bool isUserInput)
	{
		auto commandEnd = command.end();

		std::stringstream buff;
		for (auto it = command.begin(); it != commandEnd; ++it)
		{
			buff << *it;
			if (it != (commandEnd - 1))
				buff << " ";
		}

		return ExecuteCommand(buff.str(), isUserInput);
	}

	bool CommandMap::ExecuteCommandWithStatus(std::string command, bool isUserInput, std::string *output)
	{
		*output = "";

		int numArgs = 0;
		auto args = CommandLineToArgvA((PCHAR)command.c_str(), &numArgs);

		if (numArgs <= 0)
		{
			*output = "Invalid input";
			return false;
		}

		auto cmd = FindCommand(args[0]);
		if (!cmd || (isUserInput && cmd->Flags & eCommandFlagsInternal))
		{
			*output = "Command/Variable not found";
			return false;
		}

		if ((cmd->Flags & eCommandFlagsRunOnMainMenu) && !ElDorito::Instance().GameHasMenuShown)
		{
			queuedCommands.push_back(command);
			*output = "Command queued until mainmenu shows";
			return true;
		}

		// Host-only commands
		if (cmd->Flags & eCommandFlagsCheat || cmd->Flags & eCommandFlagsHostOnly)
		{
			auto session = Blam::Network::GetActiveSession();
			if (session && session->IsEstablished() && !session->IsHost())
			{
				*output = "Only a player hosting a game can use this command";
				return false;
			}
		}

		if (cmd->Flags & eCommandFlagsForge)
		{
			const auto game_options_is_valid = (bool(*)())(0x005314B0);
			const auto game_engine_is_forge = (bool(*)())(0x0059A780);

			if (!game_options_is_valid() || !game_engine_is_forge())
			{
				*output = "This command can only be run in forge";
				return false;
			}
		}

		std::vector<std::string> argsVect;
		if (numArgs > 1)
			for (int i = 1; i < numArgs; i++)
				argsVect.push_back(args[i]);

		if (cmd->Type == eCommandTypeCommand && cmd->Flags == eCommandFlagsArgsNoParse)
		{
			argsVect.clear();
			if (numArgs >= 2)
				argsVect.push_back(command.substr(std::strlen(args[0]) + 1)); //push unparsed arguments after the command
			return cmd->UpdateEvent(argsVect, *output);
		}

		if (cmd->Type == eCommandTypeCommand)
			return cmd->UpdateEvent(argsVect, *output); // if it's a command call it and return

		if (numArgs <= 1)
		{
			*output = cmd->ValueString;
			return true;
		}

		std::string previousValue;
		auto updateRet = SetVariable(cmd, argsVect[0], previousValue);
		switch (updateRet)
		{
		case eVariableSetReturnValueError:
			*output = "Command/variable not found";
			return false;
		case eVariableSetReturnValueInvalidArgument:
			*output = "Invalid value";
			return false;
		case eVariableSetReturnValueOutOfRange:
			if (cmd->Type == eCommandTypeVariableInt)
				*output = "Value " + argsVect[0] + " out of range [" + std::to_string(cmd->ValueIntMin) + ".." + std::to_string(cmd->ValueIntMax) + "]";
			else if (cmd->Type == eCommandTypeVariableInt64)
				*output = "Value " + argsVect[0] + " out of range [" + std::to_string(cmd->ValueInt64Min) + ".." + std::to_string(cmd->ValueInt64Max) + "]";
			else if (cmd->Type == eCommandTypeVariableFloat)
				*output = "Value " + argsVect[0] + " out of range [" + std::to_string(cmd->ValueFloatMin) + ".." + std::to_string(cmd->ValueFloatMax) + "]";
			else
				*output = "Value " + argsVect[0] + " out of range [this shouldn't be happening!]";
			return false;
		}

		if (!cmd->UpdateEvent)
		{
			*output = previousValue + " -> " + cmd->ValueString; // no update event, so we'll just return with what we set the value to
			return true;
		}

		auto ret = cmd->UpdateEvent(argsVect, *output);

		if (!ret) // error, revert the variable
			this->SetVariable(cmd, previousValue, std::string());

		if (output->length() <= 0)
			*output = previousValue + " -> " + cmd->ValueString;

		return ret;
	}

	std::string CommandMap::ExecuteCommand(std::string command, bool isUserInput)
	{
		std::string output;
		ExecuteCommandWithStatus(command, isUserInput, &output);
		return output;
	}

	bool CommandMap::GetVariableInt(const std::string& name, unsigned long& value)
	{
		auto command = FindCommand(name);
		if (!command || command->Type != eCommandTypeVariableInt)
			return false;

		value = command->ValueInt;
		return true;
	}

	bool CommandMap::GetVariableInt64(const std::string& name, unsigned long long& value)
	{
		auto command = FindCommand(name);
		if (!command || command->Type != eCommandTypeVariableInt64)
			return false;

		value = command->ValueInt64;
		return true;
	}

	bool CommandMap::GetVariableFloat(const std::string& name, float& value)
	{
		auto command = FindCommand(name);
		if (!command || command->Type != eCommandTypeVariableFloat)
			return false;

		value = command->ValueFloat;
		return true;
	}

	bool CommandMap::GetVariableString(const std::string& name, std::string& value)
	{
		auto command = FindCommand(name);
		if (!command || command->Type != eCommandTypeVariableString)
			return false;

		value = command->ValueString;
		return true;
	}

	VariableSetReturnValue CommandMap::SetVariable(const std::string& name, std::string& value, std::string& previousValue)
	{
		auto command = FindCommand(name);
		if (!command)
			return eVariableSetReturnValueError;

		return SetVariable(command, value, previousValue);
	}

	VariableSetReturnValue CommandMap::SetVariable(Command* command, std::string& value, std::string& previousValue)
	{
		// Disallow setting internal variables through the console
		if (command->Flags & eCommandFlagsInternal)
			return eVariableSetReturnValueError;

		try {
			switch (command->Type)
			{
			case eCommandTypeVariableString:
				previousValue = command->ValueString;
				command->ValueString = value;
				break;
			case eCommandTypeVariableInt:
				{
					previousValue = std::to_string(command->ValueInt);
					auto newValue = std::stoul(value, 0, 0);
					if ((command->ValueIntMin || command->ValueIntMax) && (newValue < command->ValueIntMin || newValue > command->ValueIntMax))
						return eVariableSetReturnValueOutOfRange;

					command->ValueInt = newValue;
					command->ValueString = std::to_string(command->ValueInt); // set the ValueString too so we can print the value out easier
					break;
				}
			case eCommandTypeVariableInt64:
				{
					previousValue = std::to_string(command->ValueInt);
					auto newValue = std::stoull(value, 0, 0);
					if ((command->ValueInt64Min || command->ValueInt64Max) && (newValue < command->ValueInt64Min || newValue > command->ValueInt64Max))
						return eVariableSetReturnValueOutOfRange;

					command->ValueInt64 = newValue;
					command->ValueString = std::to_string(command->ValueInt64); // set the ValueString too so we can print the value out easier
					break;
				}
			case eCommandTypeVariableFloat:
				{
					previousValue = std::to_string(command->ValueFloat);
					auto newValue = std::stof(value, 0);
					if ((command->ValueFloatMin || command->ValueFloatMax) && (newValue < command->ValueFloatMin || newValue > command->ValueFloatMax))
						return eVariableSetReturnValueOutOfRange;

					command->ValueFloat = newValue;
					command->ValueString = std::to_string(command->ValueFloat); // set the ValueString too so we can print the value out easier
					break;
				}
			}
		}
		catch (std::invalid_argument)
		{
			return eVariableSetReturnValueInvalidArgument;
		}
		catch (std::out_of_range)
		{
			return eVariableSetReturnValueInvalidArgument;
		}

		NotifyVariableUpdated(command);

		return eVariableSetReturnValueSuccess;
	}

	bool compare_commands(const Command& lhs, const Command& rhs) {
		return lhs.Name < rhs.Name;
	}

	std::string CommandMap::GenerateHelpText(std::string moduleFilter)
	{
		std::deque<Command> tempCommands(Commands);

		std::sort(tempCommands.begin(), tempCommands.end(), compare_commands);
		std::stringstream ss;
		std::stringstream hasParent; // store commands with a parent module seperately, so they can be added to the main stringstream after the non-parent commands
		for (auto cmd : tempCommands)
		{
			if (cmd.Flags & eCommandFlagsHidden || cmd.Flags & eCommandFlagsInternal)
				continue;

			if (!moduleFilter.empty() && (cmd.ModuleName.empty() || _stricmp(cmd.ModuleName.c_str(), moduleFilter.c_str())))
				continue;

			std::string helpText = cmd.Name;
			if (cmd.Type != eCommandTypeCommand && !(cmd.Flags & eCommandFlagsOmitValueInList))
				helpText += " " + cmd.ValueString;

			helpText += " - " + cmd.Description;

			if (cmd.ModuleName.length() > 0)
				hasParent << helpText << std::endl;
			else
				ss << helpText << std::endl;
		}

		ss << hasParent.str();

		return ss.str();
	}

	std::string CommandMap::SaveVariables()
	{
		std::stringstream ss;
		for (auto cmd : Commands)
		{
			if (cmd.Type == eCommandTypeCommand || !(cmd.Flags & eCommandFlagsArchived) || (cmd.Flags & eCommandFlagsInternal))
				continue;

			ss << cmd.Name << " \"" << cmd.ValueString << "\"" << std::endl;
		}
		return ss.str();
	}

	std::string CommandMap::SaveKeys()
	{
		std::stringstream ss;
		for (auto cmd : Commands)
		{
			if (cmd.Type == eCommandTypeCommand || !(cmd.Flags & eCommandFlagsWriteToKeys) || (cmd.Flags & eCommandFlagsInternal))
				continue;

			ss << cmd.Name << " \"" << cmd.ValueString << "\"" << std::endl;
		}
		return ss.str();
	}

	std::string CommandMap::ExecuteCommands(std::string& commands, bool isUserInput)
	{
		std::istringstream stream(commands);
		std::stringstream ss;
		std::string line;
		std::string output;
		int lineIdx = 0;
		while (std::getline(stream, line))
		{
			line.erase(line.find_last_not_of("\r") + 1);
			if (!this->ExecuteCommandWithStatus(line, isUserInput, &output))
			{
				ss << "Error at line " << lineIdx << std::endl;
			}
			lineIdx++;
		}
		return ss.str();
	}

	std::string CommandMap::ExecuteQueue()
	{
		std::stringstream ss;
		for (auto cmd : queuedCommands)
		{
			ss << ExecuteCommand(cmd, true) << std::endl;
		}
		queuedCommands.clear();
		return ss.str();
	}

	void CommandMap::NotifyVariableUpdated(const Command *command)
	{
		for (auto &&callback : variableUpdateCallbacks)
			callback(command);
	}

	void CommandMap::OnVariableUpdate(VariableUpdateCallback callback)
	{
		variableUpdateCallbacks.push_back(callback);
	}
}

namespace Modules
{
	PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc)
	{
		PCHAR* argv;
		PCHAR  _argv;
		ULONG   len;
		ULONG   argc;
		CHAR   a;
		ULONG   i, j;

		BOOLEAN  in_QM;
		BOOLEAN  in_TEXT;
		BOOLEAN  in_SPACE;

		len = strlen(CmdLine);
		i = ((len + 2) / 2)*sizeof(PVOID)+sizeof(PVOID);

		argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
			i + (len + 2)*sizeof(CHAR));

		if (!argv)
			return 0;

		_argv = (PCHAR)(((PUCHAR)argv) + i);

		argc = 0;
		argv[argc] = _argv;
		in_QM = FALSE;
		in_TEXT = FALSE;
		in_SPACE = TRUE;
		i = 0;
		j = 0;

		while (a = CmdLine[i]) {
			if (in_QM) {
				if (a == '\"') {
					in_QM = FALSE;
				}
				else {
					_argv[j] = a;
					j++;
				}
			}
			else {
				switch (a) {
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if (in_TEXT) {
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					_argv[j] = a;
					j++;
					in_SPACE = FALSE;
					break;
				}
			}
			i++;
		}
		_argv[j] = '\0';
		argv[argc] = NULL;

		(*_argc) = argc;
		return argv;
	}
}