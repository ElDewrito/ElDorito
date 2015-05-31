#pragma once

#include <Windows.h>
#include <vector>
#include <deque>

#include "Utils/Singleton.h"

enum VariableSetReturnValue
{
	eVariableSetReturnValueSuccess,
	eVariableSetReturnValueError,
	eVariableSetReturnValueOutOfRange,
	eVariableSetReturnValueInvalidArgument,
};

typedef bool (*CommandUpdateFunc)(const std::vector<std::string>& Arguments, std::string& returnInfo);

namespace
{
	PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc);
}

namespace Modules
{
	enum CommandType
	{
		eCommandTypeCommand,
		eCommandTypeVariableInt,
		eCommandTypeVariableInt64,
		eCommandTypeVariableFloat,
		eCommandTypeVariableString
	};
	struct Command
	{
		std::string Name; // modulename is added to this too, makes looking it up easier
		std::string ModuleName;
		std::string ShortName; // because some people can't be bothered to type in module names
		std::string Description;

		CommandType Type;

		CommandUpdateFunc UpdateEvent;

		unsigned long ValueInt;
		unsigned long long ValueInt64;
		float ValueFloat;
		std::string ValueString;

		unsigned long DefaultValueInt;
		unsigned long long DefaultValueInt64;
		float DefaultValueFloat;
		std::string DefaultValueString;

		unsigned long ValueIntMin;
		unsigned long ValueIntMax;
		unsigned long long ValueInt64Min;
		unsigned long long ValueInt64Max;
		float ValueFloatMin;
		float ValueFloatMax;

		// CommandArgs is only used for help text, commands will have to parse the args themselves

		std::vector<std::string> CommandArgs; // arg names can't contain a space, since space/EOL is used to find where the command name ends and the command description begins
		// this is for the help function, so you can specify an arg here like "parameter_1 This parameter controls the first parameter"
		// in the help text this will be printed like "Usage: Game.Command <parameter_1>\r\nparameter_1: This parameter controls the first parameter."
		// also don't end descriptions with a period since it'll be added in later
		// modifiers might be added to the name later, so you can do things like "parameter_1:o" to signify the parameter is optional
		Command();
		std::string GenerateHelpText();
	};

	class CommandMap : public Utils::Singleton<CommandMap>
	{
	public:
		std::deque<Command> Commands;

		Command* AddCommand(Command command);
		Command* FindCommand(const std::string& name);
		std::string ExecuteCommand(std::vector<std::string> command);
		std::string ExecuteCommand(std::string command);
		std::string ExecuteCommands(std::string& commands);
		bool ExecuteCommandWithStatus(std::string command);

		bool GetVariableInt(const std::string& name, unsigned long& value);
		bool GetVariableInt64(const std::string& name, unsigned long long& value);
		bool GetVariableFloat(const std::string& name, float& value);
		bool GetVariableString(const std::string& name, std::string& value);

		VariableSetReturnValue SetVariable(const std::string& name, std::string& value, std::string& previousValue);
		VariableSetReturnValue SetVariable(Command* command, std::string& value, std::string& previousValue);

		std::string GenerateHelpText();

		std::string SaveVariables();
	};
}