#include <ctime>
#include "ModulePlayer.hpp"
#include "../ElDorito.hpp"
#include "../Game/Armor.hpp"
#include "../Patches/PlayerRepresentation.hpp"
#include "../Patches/PlayerUid.hpp"
#include <iomanip>

namespace
{
	bool VariablePlayerArmorUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Game::Armor::RefreshUiPlayer();
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

	bool VariablePlayerRepresentationUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
#ifdef _DEBUG
		Patches::PlayerRepresentation::UpdateLocalRepresentation();
#endif
		Game::Armor::RefreshUiPlayer();
		return true;
	}

	bool CommandPlayerPrintUID(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto uid = Patches::PlayerUid::Get();
		std::stringstream ss;
		ss << std::setw(16) << std::setfill('0') << std::hex << uid << std::dec << std::setw(0);
		returnInfo = "Player UID: 0x" + ss.str();
		return true;
	}

	bool TryParseFloat(const std::string& str, float* value)
	{
		const auto c_str = str.c_str();
		char* endp;
		*value = static_cast<float>(std::strtod(c_str, &endp));
		return c_str != endp;
	}

	bool CommandSetUiPlayerModelPosition(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Blam::Math::RealVector3D position;
		if (Arguments.size() < 3 ||
			!TryParseFloat(Arguments[0], &position.I) ||
			!TryParseFloat(Arguments[1], &position.J) ||
			!TryParseFloat(Arguments[2], &position.K))
		{
			returnInfo = "Invalid arguments";
			return false;
		}

		Game::Armor::SetUiPlayerModelTransform(&position, nullptr);

		return true;
	}

	bool CommandSetUiPlayerModelRotation(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		float rotationAngle;
		if (Arguments.size() < 1 || !TryParseFloat(Arguments[0], &rotationAngle))
		{
			returnInfo = "Invalid arguments";
			return false;
		}

		Game::Armor::SetUiPlayerModelTransform(nullptr, &rotationAngle);

		return true;
	}
}

namespace Modules
{
	ModulePlayer::ModulePlayer() : ModuleBase("Player")
	{
		VarArmorHelmet = AddVariableString("Armor.Helmet", "armor_helmet", "Armor ID for player helmet", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorChest = AddVariableString("Armor.Chest", "armor_chest", "Armor ID for player chest", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorRightShoulder = AddVariableString("Armor.RightShoulder", "armor_right_shoulder", "Armor ID for player right shoulder", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		VarArmorLeftShoulder = AddVariableString("Armor.LeftShoulder", "armor_left_shoulder", "Armor ID for player left shoulder", eCommandFlagsArchived, "", VariablePlayerArmorUpdate);
		AddCommand("Armor.Update", "armor_update", "Update the player's armor.", eCommandFlagsHidden, VariablePlayerArmorUpdate);
		AddCommand("Armor.SetUiModelPosition", "armor_ui_player_model_position", "Set the position of the ui player model", (CommandFlags)(eCommandFlagsOmitValueInList | eCommandFlagsHidden), CommandSetUiPlayerModelPosition);
		AddCommand("Armor.SetUiModelRotation", "armor_ui_player_model_rotation", "Set the rotation of the ui player model", (CommandFlags)(eCommandFlagsOmitValueInList | eCommandFlagsHidden), CommandSetUiPlayerModelRotation);

		VarColorsPrimary = AddVariableString("Colors.Primary", "colors_primary", "The primary colors hex value", eCommandFlagsArchived, "#171F0E", VariablePlayerArmorUpdate);
		VarColorsSecondary = AddVariableString("Colors.Secondary", "colors_secondary", "The secondary colors hex value", eCommandFlagsArchived, "#171F0E", VariablePlayerArmorUpdate);
		VarColorsVisor = AddVariableString("Colors.Visor", "colors_visor", "The visor colors hex value", eCommandFlagsArchived, "#FF7F00", VariablePlayerArmorUpdate);
		VarColorsLights = AddVariableString("Colors.Lights", "colors_lights", "The lights colors hex value", eCommandFlagsArchived, "#9685FF", VariablePlayerArmorUpdate);
		VarColorsHolo = AddVariableString("Colors.Holo", "colors_holo", "The holo colors hex value", eCommandFlagsArchived, "#FFFFFF", VariablePlayerArmorUpdate);

		VarRepresentation = AddVariableString("Representation", "player_race", "(DEBUG BUILDS ONLY) The representation to display for the player's render mannequin", (CommandFlags)(eCommandFlagsArchived | eCommandFlagsHidden), "spartan", VariablePlayerRepresentationUpdate);

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
            "Snaz", "Jungle Jim", "Spork", "Grub", "Snook", "Spleen", "Pumpkin", "Custard",
            "Cracky", "Pickle", "Slim", "Lucky", "Pudding"
		};

		srand(static_cast<unsigned int>(time(nullptr)));
		std::string randomName = defaultNames[rand() % _countof(defaultNames)];
		std::string previousValue;
		Modules::CommandMap::Instance().SetVariable(VarPlayerName, randomName, previousValue);
	}
}
