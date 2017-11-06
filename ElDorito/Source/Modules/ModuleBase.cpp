#include "ModuleBase.hpp"
#include <string>

namespace Modules
{
	ModuleBase::ModuleBase(std::string moduleName)
	{
		this->moduleName = moduleName;
	}

	Command* ModuleBase::AddCommand(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, CommandUpdateFunc updateEvent, std::initializer_list<std::string> arguments)
	{
		Command command;
		command.Name = moduleName + "." + name;
		command.ModuleName = moduleName;
		command.ShortName = shortName;
		command.Description = description;

		for (auto arg : arguments)
			command.CommandArgs.push_back(arg);

		if (moduleName.length() <= 0)
			command.Name = name;

		command.Flags = flags;
		command.Type = eCommandTypeCommand;
		command.UpdateEvent = updateEvent;

		return CommandMap::Instance().AddCommand(command);
	}

	Command* ModuleBase::AddVariableInt(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, unsigned long defaultValue, CommandUpdateFunc updateEvent)
	{
		Command command;
		command.Name = moduleName + "." + name;
		command.ModuleName = moduleName;
		command.ShortName = shortName;
		command.Description = description;

		if (moduleName.length() <= 0)
			command.Name = name;

		command.Flags = flags;
		command.Type = eCommandTypeVariableInt;
		command.DefaultValueInt = defaultValue;
		command.ValueInt = defaultValue;
		command.ValueString = std::to_string(defaultValue); // set the ValueString too so we can print the value out easier
		command.UpdateEvent = updateEvent;

		return CommandMap::Instance().AddCommand(command);
	}

	Command* ModuleBase::AddVariableInt64(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, unsigned long long defaultValue, CommandUpdateFunc updateEvent)
	{
		Command command;
		command.Name = moduleName + "." + name;
		command.ModuleName = moduleName;
		command.ShortName = shortName;
		command.Description = description;

		if (moduleName.length() <= 0)
			command.Name = name;

		command.Flags = flags;
		command.Type = eCommandTypeVariableInt64;
		command.DefaultValueInt64 = defaultValue;
		command.ValueInt64 = defaultValue;
		command.ValueString = std::to_string(defaultValue); // set the ValueString too so we can print the value out easier
		command.UpdateEvent = updateEvent;

		return CommandMap::Instance().AddCommand(command);
	}

	Command* ModuleBase::AddVariableFloat(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, float defaultValue, CommandUpdateFunc updateEvent)
	{
		Command command;
		command.Name = moduleName + "." + name;
		command.ModuleName = moduleName;
		command.ShortName = shortName;
		command.Description = description;

		if (moduleName.length() <= 0)
			command.Name = name;

		command.Flags = flags;
		command.Type = eCommandTypeVariableFloat;
		command.DefaultValueFloat = defaultValue;
		command.ValueFloat = defaultValue;
		command.ValueString = std::to_string(defaultValue); // set the ValueString too so we can print the value out easier
		command.UpdateEvent = updateEvent;

		return CommandMap::Instance().AddCommand(command);
	}

	Command* ModuleBase::AddVariableString(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, std::string defaultValue, CommandUpdateFunc updateEvent)
	{
		Command command;
		command.Name = moduleName + "." + name;
		command.ModuleName = moduleName;
		command.ShortName = shortName;
		command.Description = description;

		if (moduleName.length() <= 0)
			command.Name = name;

		command.Flags = flags;
		command.Type = eCommandTypeVariableString;
		command.DefaultValueString = defaultValue;
		command.ValueString = defaultValue;
		command.UpdateEvent = updateEvent;

		return CommandMap::Instance().AddCommand(command);
	}

	bool ModuleBase::GetVariableInt(const std::string& name, unsigned long& value)
	{
		return CommandMap::Instance().GetVariableInt(moduleName + "." + name, value);
	}
	bool ModuleBase::GetVariableFloat(const std::string& name, float& value)
	{
		return CommandMap::Instance().GetVariableFloat(moduleName + "." + name, value);
	}
	bool ModuleBase::GetVariableString(const std::string& name, std::string& value)
	{
		return CommandMap::Instance().GetVariableString(moduleName + "." + name, value);
	}
}