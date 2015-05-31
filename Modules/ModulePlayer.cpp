#include "ModulePlayer.h"
#include <sstream>
#include "../ElDorito.h"
#include "../ElPreferences.h"

namespace Modules
{
	ModulePlayer::ModulePlayer() : ModuleBase("Player")
	{
		VarArmorAccessory = AddVariableString("Armor.Accessory", "armor_accessory", "Armor ID for player accessory", "", VariablePlayerArmorAccessory);
		VarArmorArms = AddVariableString("Armor.Arms", "armor_arms", "Armor ID for player arms", "", VariablePlayerArmorArms);
		VarArmorChest = AddVariableString("Armor.Chest", "armor_chest", "Armor ID for player chest", "", VariablePlayerArmorChest);
		VarArmorHelmet = AddVariableString("Armor.Helmet", "armor_helmet", "Armor ID for player helmet", "", VariablePlayerArmorHelmet);
		VarArmorLegs = AddVariableString("Armor.Legs", "armor_legs", "Armor ID for player legs", "", VariablePlayerArmorLegs);
		VarArmorPelvis = AddVariableString("Armor.Pelvis", "armor_pelvis", "Armor ID for player pelvis", "", VariablePlayerArmorPelvis);
		VarArmorShoulders = AddVariableString("Armor.Shoulders", "armor_shoulders", "Armor ID for player shoulders", "", VariablePlayerArmorShoulders);

		VarPlayerName = AddVariableString("Name", "name", "Changes the players ingame name", "Jasper", VariablePlayerNameUpdate);

		memset(this->UserName, 0, sizeof(wchar_t)* 17);

		// patch Game_GetPlayerName to get the name from our field
		Pointer::Base(0x42AA1).Write<uint32_t>((uint32_t)&this->UserName);

		// patch BLF save func to get the name from our field
		Pointer::Base(0x124E6A).Write<uint32_t>((uint32_t)&this->UserName);

		char* defaultNames[41] = {
			"Donut", "Penguin", "Stumpy", "Whicker", "Shadow", "Howard", "Wilshire",
			"Darling", "Disco", "Jack", "The Bear", "Sneak", "The Big ", "Whisp",
			"Wheezy", "Crazy", "Goat", "Pirate", "Saucy", "Hambone", "Butcher",
			"Walla Walla", "Snake", "Caboose", "Sleepy", "Killer", "Stompy",
			"Mopey", "Dopey", "Wease", "Ghost", "Dasher", "Grumpy", "Hollywood",
			"Tooth", "Noodle", "King", "Cupid", "Prancer", "Pyong", "Jasper"
		};

		TODO("Make a SetVariableString function for this instead");

		srand((unsigned int)time(0));
		Modules::CommandMap::Instance().SetVariable(VarPlayerName, std::string(defaultNames[rand() % 41]), std::string());
	}
}

namespace
{
	bool VariablePlayerArmorAccessory(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		ElPreferences::Instance().setAccessoryName(Modules::ModulePlayer::Instance().VarArmorAccessory->ValueString);
		return true;
	}

	bool VariablePlayerArmorArms(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		ElPreferences::Instance().setArmsName(Modules::ModulePlayer::Instance().VarArmorArms->ValueString);
		return true;
	}

	bool VariablePlayerArmorChest(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		ElPreferences::Instance().setChestName(Modules::ModulePlayer::Instance().VarArmorChest->ValueString);
		return true;
	}

	bool VariablePlayerArmorHelmet(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		ElPreferences::Instance().setHelmetName(Modules::ModulePlayer::Instance().VarArmorHelmet->ValueString);
		return true;
	}

	bool VariablePlayerArmorLegs(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		ElPreferences::Instance().setLegsName(Modules::ModulePlayer::Instance().VarArmorLegs->ValueString);
		return true;
	}

	bool VariablePlayerArmorPelvis(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		ElPreferences::Instance().setPelvisName(Modules::ModulePlayer::Instance().VarArmorPelvis->ValueString);
		return true;
	}

	bool VariablePlayerArmorShoulders(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		ElPreferences::Instance().setShouldersName(Modules::ModulePlayer::Instance().VarArmorShoulders->ValueString);
		return true;
	}

	bool VariablePlayerNameUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::string name = Modules::ModulePlayer::Instance().VarPlayerName->ValueString;

		std::wstring nameStr = Utils::String::WidenString(name);
		wcscpy_s(Modules::ModulePlayer::Instance().UserName, 16, nameStr.c_str());
		std::string actualName = Utils::String::ThinString(Modules::ModulePlayer::Instance().UserName);

		ElPreferences::Instance().setPlayerName(actualName);
		ElPreferences::Instance().save();

		return true;
	}
}
