#include "ModulePlayer.h"
#include <sstream>
#include "../ElDorito.h"
#include "../ElPreferences.h"

namespace Modules
{
	ModulePlayer::ModulePlayer() : ModuleBase("Player")
	{
		AddVariableString("Name", "name", "Lets you change your ingame player name", "Jasper", VariablePlayerNameUpdate);

		memset(this->UserName, 0, sizeof(wchar_t)* 17);

		// patch Game_GetPlayerName to get the name from our field
		Pointer::Base(0x42AA1).Write<uint32_t>((uint32_t)&this->UserName);

		// patch BLF save func to get the name from our field
		Pointer::Base(0x124E6A).Write<uint32_t>((uint32_t)&this->UserName);

		char* defaultNames[40] = {
			"Donut", "Penguin", "Stumpy", "Whicker", "Shadow", "Howard", "Wilshire",
			"Darling", "Disco", "Jack", "The Bear", "Sneak", "The Big ", "Whisp",
			"Wheezy", "Crazy", "Goat", "Pirate", "Saucy", "Hambone", "Butcher",
			"Walla Walla", "Snake", "Caboose", "Sleepy", "Killer", "Stompy",
			"Mopey", "Dopey", "Wease", "Ghost", "Dasher", "Grumpy", "Hollywood",
			"Tooth", "Noodle", "King", "Cupid", "Prancer", "Pyong"
		};

		TODO("Make a SetVariableString function for this instead");

		srand((unsigned int)time(0));
		Modules::CommandMap::Instance().ExecuteCommand("Player.Name " + std::string(defaultNames[rand() % 40]));
	}
}

namespace
{
	std::string VariablePlayerNameUpdate(const std::vector<std::string>& Arguments)
	{
		std::string name;
		if (!Modules::ModuleGame::Instance().GetVariableString("Name", name))
			return "";

		std::wstring nameStr = Utils::String::WidenString(name);
		wcscpy_s(Modules::ModulePlayer::Instance().UserName, 16, nameStr.c_str());
		std::string actualName = Utils::String::ThinString(Modules::ModulePlayer::Instance().UserName);

		ElPreferences::Instance().setPlayerName(actualName);
		ElPreferences::Instance().save();

		return "Player name set to " + actualName + ".";
	}
}
