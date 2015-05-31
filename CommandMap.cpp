#include "CommandMap.h"
#include <algorithm>
#include <sstream>

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

	std::string CommandMap::ExecuteCommand(std::vector<std::string> command)
	{
		std::string commandStr = "";
		for (auto cmd : command)
			commandStr += "\"" + cmd + "\" ";

		return ExecuteCommand(commandStr);
	}

	std::string CommandMap::ExecuteCommand(std::string command)
	{
		int numArgs = 0;
		auto args = CommandLineToArgvA((PCHAR)command.c_str(), &numArgs);

		if (numArgs <= 0)
			return "Invalid input";

		auto cmd = FindCommand(args[0]);
		if (!cmd)
			return "Command/Variable not found";

		std::vector<std::string> argsVect;
		if (numArgs > 1)
		for (int i = 1; i < numArgs; i++)
			argsVect.push_back(args[i]);

		if (cmd->Type == eCommandTypeCommand)
		{
			std::string retInfo;
			cmd->UpdateEvent(argsVect, retInfo); // if it's a command call it and return
			return retInfo;
		}

		std::string previousValue;
		auto updateRet = SetVariable(cmd, (numArgs > 1 ? argsVect[0] : ""), previousValue);

		switch (updateRet)
		{
		case eVariableSetReturnValueError:
			return "Command/Variable not found";
		case eVariableSetReturnValueInvalidArgument:
			return "Invalid value";
		case eVariableSetReturnValueOutOfRange:
			if (cmd->Type == eCommandTypeVariableInt)
				return "Value " + argsVect[0] + " out of range [" + std::to_string(cmd->ValueIntMin) + ".." + std::to_string(cmd->ValueIntMax) + "]";
			if (cmd->Type == eCommandTypeVariableFloat)
				return "Value " + argsVect[0] + " out of range [" + std::to_string(cmd->ValueFloatMin) + ".." + std::to_string(cmd->ValueFloatMax) + "]";

			return "Value " + argsVect[0] + " out of range [this shouldn't be happening!]";
		}

		if (numArgs <= 1)
			return previousValue;

		if (!cmd->UpdateEvent)
			return previousValue + " -> " + cmd->ValueString; // no update event, so we'll just return with what we set the value to

		std::string retVal;
		auto ret = cmd->UpdateEvent(argsVect, retVal);

		if (!ret) // error, revert the variable
			this->SetVariable(cmd, previousValue, std::string());

		if (retVal.length() <= 0)
			return previousValue + " -> " + cmd->ValueString;

		return retVal;
	}

	bool CommandMap::GetVariableInt(const std::string& name, unsigned long& value)
	{
		auto command = FindCommand(name);
		if (!command || command->Type != eCommandTypeVariableInt)
			return false;

		value = command->ValueInt;
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
		try {
			switch (command->Type)
			{
			case eCommandTypeVariableString:
				previousValue = command->ValueString;
				if (value.length() > 0)
					command->ValueString = value;
				break;
			case eCommandTypeVariableInt:
				previousValue = std::to_string(command->ValueInt);
				if (value.length() > 0)
				{
					auto newValue = std::stoul(value, 0, 0);
					if ((command->ValueIntMin || command->ValueIntMax) && (newValue < command->ValueIntMin || newValue > command->ValueIntMax))
						return eVariableSetReturnValueOutOfRange;

					command->ValueInt = newValue;
					command->ValueString = std::to_string(command->ValueInt); // set the ValueString too so we can print the value out easier
				}
				break;
			case eCommandTypeVariableFloat:
				previousValue = std::to_string(command->ValueFloat);
				if (value.length() > 0)
				{
					auto newValue = std::stof(value, 0);
					if ((command->ValueFloatMin || command->ValueFloatMax) && (newValue < command->ValueFloatMin || newValue > command->ValueFloatMax))
						return eVariableSetReturnValueOutOfRange;

					command->ValueFloat = newValue;
					command->ValueString = std::to_string(command->ValueFloat); // set the ValueString too so we can print the value out easier
				}
				break;
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

		return eVariableSetReturnValueSuccess;
	}

	bool compare_commands(const Command& lhs, const Command& rhs) {
		return lhs.Name < rhs.Name;
	}

	std::string CommandMap::GenerateHelpText()
	{
		std::sort(Commands.begin(), Commands.end(), compare_commands);
		std::stringstream ss;
		std::stringstream hasParent;
		for (auto cmd : Commands)
		{
			auto helpText = cmd.GenerateHelpText();

			if (cmd.ModuleName.length() > 0)
				hasParent << cmd.GenerateHelpText();
			else
				ss << cmd.GenerateHelpText();
		}

		ss << hasParent.str();

		return ss.str();
	}
}

namespace
{
	unsigned long stou(std::string const & str, size_t * idx, int base)
	{
		unsigned long result = std::stoul(str, idx, base);
		if (result > UINT_MAX)
		{
			throw std::out_of_range("stou");
		}
		return result;
	}

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