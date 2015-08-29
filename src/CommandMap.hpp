#pragma once

#include <Windows.h>
#include <vector>
#include <deque>

#include "Utils/Singleton.hpp"

enum VariableSetReturnValue
{
	eVariableSetReturnValueSuccess,
	eVariableSetReturnValueError,
	eVariableSetReturnValueOutOfRange,
	eVariableSetReturnValueInvalidArgument,
};

enum CommandFlags
{
	eCommandFlagsNone,
	eCommandFlagsCheat             = 1 << 0, // only allow this command on cheat-enabled servers, whenever they get implemented
	eCommandFlagsReplicated        = 1 << 1, // value of this variable should be output into the server info JSON, clients should update their variable to match the one in JSON
	eCommandFlagsArchived          = 1 << 2, // value of this variable should be written when using WriteConfig
	eCommandFlagsDontUpdateInitial = 1 << 3, // don't call the update event when the variable is first being initialized
	eCommandFlagsHidden            = 1 << 4, // hide this command/var from the help listing
	eCommandFlagsRunOnMainMenu     = 1 << 5, // if run at startup queue the command until the main menu is shown
	eCommandFlagsHostOnly          = 1 << 6, // only execute the command if the user is host
	eCommandFlagsOmitValueInList   = 1 << 7, // omit the variables value in help listing
	eCommandFlagsInternal          = 1 << 8  // disallow the user from using this command, only internal ExecuteCommand calls can use it
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
		eCommandTypeVariableString,

		eCommandType_Count
	};
	struct Command
	{
		std::string Name; // modulename is added to this too, makes looking it up easier
		std::string ModuleName;
		std::string ShortName; // because some people can't be bothered to type in module names
		std::string Description;

		CommandFlags Flags;
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
		void FinishAddCommands();
		Command* FindCommand(const std::string& name);

		std::string ExecuteCommand(std::vector<std::string> command, bool isUserInput = false);
		std::string ExecuteCommand(std::string command, bool isUserInput = false);
		std::string ExecuteCommands(std::string& commands, bool isUserInput = false);
		bool ExecuteCommandWithStatus(std::string command, bool isUserInput = false);
		std::string ExecuteQueue();

		bool GetVariableInt(const std::string& name, unsigned long& value);
		bool GetVariableInt64(const std::string& name, unsigned long long& value);
		bool GetVariableFloat(const std::string& name, float& value);
		bool GetVariableString(const std::string& name, std::string& value);

		VariableSetReturnValue SetVariable(const std::string& name, std::string& value, std::string& previousValue);
		VariableSetReturnValue SetVariable(Command* command, std::string& value, std::string& previousValue);

		std::string GenerateHelpText(std::string moduleFilter = "");

		std::string SaveVariables();
	private:
		std::vector<std::string> queuedCommands;
	};
}