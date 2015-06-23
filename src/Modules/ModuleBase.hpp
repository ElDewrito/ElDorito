#pragma once

#include "../CommandMap.hpp"

namespace Modules
{
	class ModuleBase
	{
	public:
		ModuleBase(std::string moduleName);

		bool GetVariableInt(const std::string& name, unsigned long& value);
		bool GetVariableFloat(const std::string& name, float& value);
		bool GetVariableString(const std::string& name, std::string& value);

	protected:
		Command* AddCommand(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, CommandUpdateFunc updateEvent, std::initializer_list<std::string> arguments = {});
		Command* AddVariableInt(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, unsigned long defaultValue = 0, CommandUpdateFunc updateEvent = 0);
		Command* AddVariableInt64(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, unsigned long long defaultValue = 0, CommandUpdateFunc updateEvent = 0);
		Command* AddVariableFloat(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, float defaultValue = 0, CommandUpdateFunc updateEvent = 0);
		Command* AddVariableString(const std::string& name, const std::string& shortName, const std::string& description, CommandFlags flags, std::string defaultValue = "", CommandUpdateFunc updateEvent = 0);

	private:
		std::string moduleName;
	};
}