#include <ctime>
#include "ModulePlayer.hpp"
#include "../ElDorito.hpp"
#include "../Game/Armor.hpp"
#include "../Patches/PlayerRepresentation.hpp"
#include "../Patches/PlayerUid.hpp"
#include <iomanip>
#include <algorithm>
#include <unordered_set>

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

	bool VariablePlayerServiceTagUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!Arguments.size())
			return false;

		auto value = Arguments[0];
		std::transform(value.begin(), value.end(), value.begin(), toupper);

		if (!Modules::ModulePlayer::ValidServiceTag(value))
		{
			returnInfo = "Invalid service tag!";
			return false;
		}

		auto &modulePlayer = Modules::ModulePlayer::Instance();
		modulePlayer.VarPlayerServiceTag->ValueString = value;

#ifdef _DEBUG
		Patches::PlayerRepresentation::UpdateLocalRepresentation();
#endif
		return true;
	}

	bool VariablePlayerGenderUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!Arguments.size() || (Arguments[0].compare("male") && Arguments[0].compare("female")))
			return false;
#ifdef _DEBUG
		Patches::PlayerRepresentation::UpdateLocalRepresentation();
#endif
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

	bool CommandSetCarryType(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Pointer &playerCtrlGlobalsPtr = ElDorito::GetMainTls(GameGlobals::Input::TLSOffset)[0](GameGlobals::Input::CarryType);
		playerCtrlGlobalsPtr.WriteFast<uint8_t>(!playerCtrlGlobalsPtr.Read<uint8_t>());
		return true;
	}
	bool GenerateTimestamp(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto &modulePlayer = Modules::ModulePlayer::Instance();
		std::string encryptedString;
		time_t now = time(0);
		std::tm* now_tm = gmtime(&now);
		char buf[42];
		strftime(buf, 42, "%Y%m%d %X", now_tm);
		Utils::Cryptography::PrivateEncrypt(Utils::Cryptography::ReformatKey(true, modulePlayer.VarPlayerPrivKey->ValueString), buf, strlen(buf), encryptedString);
		returnInfo = encryptedString;
		return true;
	}
	std::string GenerateRandomServiceTag()
	{
		std::string tag(4, 0);
		tag[0] = rand() % 26 + 'A';
		tag[1] = rand() % 10 + '0';
		tag[2] = rand() % 10 + '0';
		tag[3] = rand() % 10 + '0';
		return tag;
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

		AddCommand("AlertCarry", "alert_carry", "Toggle the alert carry pose", eCommandFlagsNone, CommandSetCarryType);

		VarColorsPrimary = AddVariableString("Colors.Primary", "colors_primary", "The primary colors hex value", eCommandFlagsArchived, "#171F0E", VariablePlayerArmorUpdate);
		VarColorsSecondary = AddVariableString("Colors.Secondary", "colors_secondary", "The secondary colors hex value", eCommandFlagsArchived, "#171F0E", VariablePlayerArmorUpdate);
		VarColorsVisor = AddVariableString("Colors.Visor", "colors_visor", "The visor colors hex value", eCommandFlagsArchived, "#FF7F00", VariablePlayerArmorUpdate);
		VarColorsLights = AddVariableString("Colors.Lights", "colors_lights", "The lights colors hex value", eCommandFlagsArchived, "#9685FF", VariablePlayerArmorUpdate);

		VarRepresentation = AddVariableString("Representation", "player_race", "(DEBUG BUILDS ONLY) The representation to display for the player's render mannequin", eCommandFlagsInternal, "spartan", VariablePlayerRepresentationUpdate);

		VarPlayerName = AddVariableString("Name", "name", "The players ingame name", CommandFlags(eCommandFlagsArchived|eCommandFlagsNoReset), "Jasper", VariablePlayerNameUpdate);
		VarPlayerServiceTag = AddVariableString("ServiceTag", "service_tag", "The players service tag", eCommandFlagsArchived, "117", VariablePlayerServiceTagUpdate);
		VarPlayerGender = AddVariableString("Gender", "gender", "The players gender", eCommandFlagsArchived, "male", VariablePlayerGenderUpdate);

		// hack to add a small notice before Player.PrivKey in the cfg file
		AddVariableString("PrivKeyNote", "priv_key_note", "", (CommandFlags)(eCommandFlagsWriteToKeys | eCommandFlagsHidden), "The PrivKey below is used to keep your stats safe. Treat it like a password and don't share it with anyone!");
		VarPlayerPrivKey = AddVariableString("PrivKey", "player_privkey", "The players unique stats private key", (CommandFlags)(eCommandFlagsOmitValueInList | eCommandFlagsWriteToKeys), "");
		VarPlayerPubKey = AddVariableString("PubKey", "player_pubkey", "The players unique stats public key", (CommandFlags)(eCommandFlagsOmitValueInList | eCommandFlagsWriteToKeys), "");
		memset(this->UserName, 0, sizeof(wchar_t)* 17);

		AddCommand("PrintUID", "uid", "Prints the players UID", eCommandFlagsNone, CommandPlayerPrintUID);
		AddCommand("EncryptGmtTimestamp", "encryptgmttimestamp", "encrypts a timestamp using the player's private key.", eCommandFlagsHidden, GenerateTimestamp);


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
		Modules::CommandMap::Instance().SetVariable(VarPlayerServiceTag, GenerateRandomServiceTag(), previousValue);
	}

	bool ModulePlayer::ValidServiceTag(const std::string &tag)
	{
		static const std::unordered_set<std::string> filter = {"ANUS", "ARSE", "CLIT", "COCK", "COON", "CUNT", "DAGO", "DAMN","DICK", "DIKE", "DYKE", "FUCK", "GOOK", "HEEB", "HELL", "HOMO", "JIZZ", "KIKE", "KUNT", "KYKE", "MICK", "MUFF", "PAKI", "PISS", "POON", "PUTO", "SHIT", "SHIZ", "SLUT", "SMEG", "SPIC", "TARD", "TITS", "TWAT", "WANK", "NIGA"};

		return tag.length() > 2 && tag.length() < 5
			&& all_of(tag.begin(), tag.end(), [](auto c) { return c >= '0' && c <= 'Z'; })
			&& filter.find(tag) == filter.end();
	}
}
