#pragma once

#include "ModuleBase.h"

namespace
{
	bool VariablePlayerArmorAccessoryUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorArmsUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorChestUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorHelmetUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorLegsUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorPelvisUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerArmorShouldersUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);

	bool VariablePlayerColorsPrimaryUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerColorsSecondaryUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerColorsVisorUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerColorsLightsUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerColorsHoloUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);

	bool VariablePlayerNameUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
	bool VariablePlayerUserIDUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo);
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

		Command* VarColorsPrimary;
		Command* VarColorsSecondary;
		Command* VarColorsVisor;
		Command* VarColorsLights;
		Command* VarColorsHolo;

		Command* VarPlayerName;
		Command* VarUserID;

		wchar_t UserName[17];

		ModulePlayer();
	};
}