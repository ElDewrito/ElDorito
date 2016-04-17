#include <ctime>
#include "ModulePlayer.hpp"
#include "../ElDorito.hpp"
#include "../Patches/Armor.hpp"
#include "../Patches/PlayerUid.hpp"

namespace
{
	bool VariablePlayerArmorUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Armor::RefreshUiPlayer();
		return true;
	}

	bool VariablePlayerNameUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto &modulePlayer = Modules::ModulePlayer::Instance();
		auto name = modulePlayer.VarPlayerName->ValueString;
		auto nameW = Utils::String::WidenString(name);
		wcsncpy_s(modulePlayer.UserName, nameW.c_str(), 16);
		modulePlayer.UserName[15] = 0;
		return true;
	}

	bool CommandPlayerPrintUID(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto uid = Patches::PlayerUid::Get();
		std::string uidStr;
		Utils::String::BytesToHexString(&uid, sizeof(uint64_t), uidStr);
		returnInfo = "Player UID: 0x" + uidStr;
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

		VarRenderWeapon = AddVariableString("RenderWeapon", "render_weapon", "The weapon to display on the player's render mannequin", eCommandFlagsArchived, "assault_rifle", VariablePlayerArmorUpdate);

		VarPlayerName = AddVariableString("Name", "name", "The players ingame name", eCommandFlagsArchived, "Jasper", VariablePlayerNameUpdate);
		// hack to add a small notice before Player.PrivKey in the cfg file
		AddVariableString("PrivKeyNote", "priv_key_note", "", (CommandFlags)(eCommandFlagsArchived | eCommandFlagsHidden), "The PrivKey below is used to keep your stats safe. Treat it like a password and don't share it with anyone!");
		VarPlayerPrivKey = AddVariableString("PrivKey", "player_privkey", "The players unique stats private key", (CommandFlags)(eCommandFlagsOmitValueInList | eCommandFlagsArchived), "");
		VarPlayerPubKey = AddVariableString("PubKey", "player_pubkey", "The players unique stats public key", (CommandFlags)(eCommandFlagsOmitValueInList | eCommandFlagsArchived), "");
		memset(this->UserName, 0, sizeof(wchar_t)* 17);

		AddCommand("PrintUID", "uid", "Prints the players UID", eCommandFlagsNone, CommandPlayerPrintUID);

		// patch Game_GetPlayerName to get the name from our field
		Pointer::Base(0x42AA1).Write<uint32_t>((uint32_t)&this->UserName);

		// patch BLF save func to get the name from our field
		Pointer::Base(0x124E6A).Write<uint32_t>((uint32_t)&this->UserName);

		char* defaultNames[] = {
            "Mustard", "Sleepy", "Dopey", "Grumpy", "Mopey", "Sketch", "Crazy", "Saucy", "Wheezy", "Kitty",
            "Killer", "Steak", "Butcher", "Darling", "Disco", "Donut", "Cotton", "Stank", "Ratjar", "Noodle",
            "Weasel", "Snake", "Shadow", "Caboose", "King", "Sneak", "The Bear", "Ghost", "Penguin", "Coward",
            "Moogley", "Doggy Dog", "Potato", "Wilshire", "Pirate", "Wallace", "Tooth", "Goat", "Stumpy", "Stomp",
            "Winky", "Ling-ling", "Hippo", "Turtle", "Moose", "Derby", "Wiggly", "Bongo", "Skip", "Bork",
            "Lemon", "Hermit", "Ralfie", "Cheese", "Duke", "Badger", "Beagle", "Wort", "Wiggler", "Soffish",
            "Yodler", "Geronimo", "Darkman", "Snoozy", "Shaggy", "Banana", "Shiz", "Giant", "Slinky", "Pretzel",
            "Brutus", "Pinky", "Milo", "Monkey", "Bozo", "Grunty", "Bower", "Floss", "Nappy", "Gerbil",
            "Goose", "Scruffy", "Soap", "Polo", "Muffin", "Beast", "Clubby", "Surly", "Balin", "Poodle",
            "Snaz", "Jungle Jim", "Spork", "Grub", "Snook", "Spleen", "Goose", "Pumpkin", "Monkey", "Custard",
            "Cracky", "Pickle", "Slim", "Lucky", "Pudding"
		};

		srand(static_cast<unsigned int>(time(nullptr)));
		std::string randomName = defaultNames[rand() % _countof(defaultNames)];
		std::string previousValue;
		Modules::CommandMap::Instance().SetVariable(VarPlayerName, randomName, previousValue);
	}
}
