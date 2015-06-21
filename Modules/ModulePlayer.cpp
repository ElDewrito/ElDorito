#include "ModulePlayer.h"
#include <sstream>
#include "../ElDorito.h"
#include "../Patches/Armor.h"

namespace
{
	bool VariablePlayerArmorUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Armor::RefreshUiPlayer();
		return true;
	}

	bool VariablePlayerNameUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::string name = Modules::ModulePlayer::Instance().VarPlayerName->ValueString;

		std::wstring nameStr = Utils::String::WidenString(name);
		wcscpy_s(Modules::ModulePlayer::Instance().UserName, 16, nameStr.c_str());
		std::string actualName = Utils::String::ThinString(Modules::ModulePlayer::Instance().UserName);

		return true;
	}

	bool VariablePlayerUserIDUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		return true;
	}
}

namespace Modules
{
	ModulePlayer::ModulePlayer() : ModuleBase("Player")
	{
		VarArmorAccessory = AddVariableString("Armor.Accessory", "armor_accessory", "Armor ID for player accessory", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorArms = AddVariableString("Armor.Arms", "armor_arms", "Armor ID for player arms", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorChest = AddVariableString("Armor.Chest", "armor_chest", "Armor ID for player chest", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorHelmet = AddVariableString("Armor.Helmet", "armor_helmet", "Armor ID for player helmet", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorLegs = AddVariableString("Armor.Legs", "armor_legs", "Armor ID for player legs", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorPelvis = AddVariableString("Armor.Pelvis", "armor_pelvis", "Armor ID for player pelvis", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorShoulders = AddVariableString("Armor.Shoulders", "armor_shoulders", "Armor ID for player shoulders", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);

		VarColorsPrimary = AddVariableString("Colors.Primary", "colors_primary", "The primary colors hex value", eCommandFlagsArchived, "#000000", VariablePlayerArmorUpdate);
		VarColorsSecondary = AddVariableString("Colors.Secondary", "colors_secondary", "The secondary colors hex value", eCommandFlagsArchived, "#000000", VariablePlayerArmorUpdate);
		VarColorsVisor = AddVariableString("Colors.Visor", "colors_visor", "The visor colors hex value", eCommandFlagsArchived, "#000000", VariablePlayerArmorUpdate);
		VarColorsLights = AddVariableString("Colors.Lights", "colors_lights", "The lights colors hex value", eCommandFlagsArchived, "#000000", VariablePlayerArmorUpdate);
		VarColorsHolo = AddVariableString("Colors.Holo", "colors_holo", "The holo colors hex value", eCommandFlagsArchived, "#000000", VariablePlayerArmorUpdate);

		VarPlayerName = AddVariableString("Name", "name", "The players ingame name", eCommandFlagsArchived, "Jasper", VariablePlayerNameUpdate);
		// hack to add a small notice before Player.PrivKey in the cfg file
		AddVariableString("PrivKeyNote", "priv_key_note", "", (CommandFlags)(eCommandFlagsArchived | eCommandFlagsHidden), "The PrivKey below is used to keep your stats safe. Treat it like a password and don't share it with anyone!");
		VarPlayerPrivKey = AddVariableString("PrivKey", "player_privkey", "The players unique stats private key", eCommandFlagsArchived, "");
		VarPlayerPubKey = AddVariableString("PubKey", "player_pubkey", "The players unique stats public key", eCommandFlagsArchived, "");
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

		srand((unsigned int)time(0));
		Modules::CommandMap::Instance().SetVariable(VarPlayerName, std::string(defaultNames[rand() % 41]), std::string());
	}
}
