#pragma once

#include "ModuleBase.h"

namespace
{
	bool VariablePlayerArmorAccessory(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorArms(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorChest(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorHelmet(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorLegs(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorPelvis(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorShoulders(const std::vector<std::string>& Arguments, std::string& returnInfo);

	bool VariablePlayerNameUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
}

namespace Modules
{
	class ModulePlayer : public Utils::Singleton<ModulePlayer>, public ModuleBase
	{
	public:
		Command* VarArmorAccessory;
		Command* VarArmorArms;
		Command* VarArmorChest;
		Command* VarArmorHelmet;
		Command* VarArmorLegs;
		Command* VarArmorPelvis;
		Command* VarArmorShoulders;
		Command* VarPlayerName;

		wchar_t UserName[17];

		ModulePlayer();
	};
}