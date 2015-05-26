#include "ModuleGame.h"
#include <sstream>
#include "../ElDorito.h"
#include "../Patches/Ui.h"

namespace Modules
{
	ModuleGame::ModuleGame() : ModuleBase("Game")
	{
		AddVariableString("DebugMode", "debug", "Allows you to output game debug messages to a log file", "off", VariableGameDebugModeUpdate);

		AddCommand("Info", "info", "Displays information about the game", CommandGameInfo);

		AddCommand("Exit", "exit", "Kills the game process", CommandGameExit);

		AddCommand("LoadMap", "map", "Loads a map", CommandGameLoadMap, { "mapname(string) The name of the map to load", "maptype(int) The type of engine to use", "gametype(int) The gamemode to load" });

		AddCommand("ShowUI", "show_ui", "Allows you to force a UI widget to open", CommandGameShowUI, { "dialogstringID(int) The dialog ID to open", "arg1(int) Unknown argument", "flags(int) Unknown argument", "parentdialogstringID(int) The ID of the parent dialog" });

		/*EXAMPLES: adds a variable "Game.Name", default value ElDewrito, calls VariableGameNameUpdate when value is updated
		AddVariableString("Name", "gamename", "Title of the game", "ElDewrito", VariableGameNameUpdate);

		// adds a variable "Game.Year", default value 2015, clamped to [1994..3030]
		auto cmd = AddVariableInt("Year", "gameyear", "The current year", 2015);
		cmd->ValueIntMin = 1994;
		cmd->ValueIntMax = 3030;

		// adds a variable "Game.Money", default value 1.86
		AddVariableFloat("Money", "gamemoney", "Your mothers hourly rate", 1.86f);*/
	}
}

namespace
{
	std::string VariableGameDebugModeUpdate(const std::vector<std::string>& Arguments)
	{
		return "";
	}

	std::string CommandGameInfo(const std::vector<std::string>& Arguments)
	{
		std::stringstream ss;
		ss << std::hex << "ThreadLocalStorage: " << std::hex << (size_t)(void*)ElDorito::GetMainTls() << std::endl;

		std::string ArgList((char*)Pointer(0x199C0A4)[0]);
		ss << "Command line args: " << (ArgList.empty() ? "(null)" : ArgList) << std::endl;

		std::string LocalSecureKey((char*)Pointer(0x50CCDB4 + 1));
		ss << "Local Secure Key: " << (LocalSecureKey.empty() ? "(null)" : LocalSecureKey) << std::endl;

		std::string Xnkid;
		Utils::String::BytesToHexString((char*)Pointer(0x2247b80), 0x10, Xnkid);
		ss << "XNKID: " << Xnkid << std::endl;

		std::string Xnaddr;
		Utils::String::BytesToHexString((char*)Pointer(0x2247b90), 0x10, Xnaddr);
		ss << "XNAddr: " << Xnaddr << std::endl;

		std::string Build((char*)Pointer(0x199C0F0));
		ss << "Build: " << (Build.empty() ? "(null)" : Build) << std::endl;

		std::string SystemID((char*)Pointer(0x199C130));
		ss << "SystemID: " << (SystemID.empty() ? "(null)" : SystemID) << std::endl;

		std::string SessionID((char*)Pointer(0x199C1D0));
		ss << "SessionID: " << (SessionID.empty() ? "(null)" : SessionID) << std::endl;

		std::string DevKitName((char*)Pointer(0x160C8C8));
		std::string DevKitVersion((char*)Pointer(0x199C350));
		ss << "SDK Info: " << (DevKitName.empty() ? "(null)" : DevKitName) << '|';
		ss << (DevKitVersion.empty() ? "(null)" : DevKitVersion) << std::endl;

		std::string FlashVersion((char*)Pointer(0x199C350));
		ss << "Flash Version: " << (FlashVersion.empty() ? "(null)" : FlashVersion) << std::endl;

		std::string MapName((char*)Pointer(0x22AB018)(0x1A4));
		ss << "Current Map: " << (MapName.empty() ? "(null)" : MapName) << std::endl;

		ss << "Current Map Cache Size: " << Pointer(0x22AB018)(0x8).Read<int32_t>() << std::endl;

		ss << "Tag Table Offset: " << std::hex << Pointer(0x22AAFF4).Read<uint32_t>() << std::endl;

		ss << "Tag Bank Offset: " << std::hex << Pointer(0x22AAFF8).Read<uint32_t>() << std::endl;

		return ss.str();
	}

	std::string CommandGameExit(const std::vector<std::string>& Arguments)
	{
		std::exit(0);
		return "";
	}

	std::string CommandGameLoadMap(const std::vector<std::string>& Arguments)
	{
		return "";
	}

	std::string CommandGameShowUI(const std::vector<std::string>& Arguments)
	{
		if (Arguments.size() <= 0)
			return "You must specify at least a dialog ID!";

		unsigned int dialogStringId = 0;
		int dialogArg1 = 0; // todo: figure out a better name for this
		int dialogFlags = 4;
		unsigned int dialogParentStringId = 0x1000C;
		try
		{
			dialogStringId = std::stoul(Arguments[0], 0, 0);
			if (Arguments.size() > 1)
				dialogArg1 = std::stoi(Arguments[1], 0, 0);
			if (Arguments.size() > 2)
				dialogFlags = std::stoi(Arguments[2], 0, 0);
			if (Arguments.size() > 3)
				dialogParentStringId = std::stoul(Arguments[3], 0, 0);
		}
		catch (std::invalid_argument)
		{
			return "Invalid argument given.";
		}
		catch (std::out_of_range)
		{
			return "Invalid argument given.";
		}

		Patches::Ui::DialogStringId = dialogStringId;
		Patches::Ui::DialogArg1 = dialogArg1;
		Patches::Ui::DialogFlags = dialogFlags;
		Patches::Ui::DialogParentStringId = dialogParentStringId;
		Patches::Ui::DialogShow = true;

		return "Sent Show_UI notification to game.";
	}

	//EXAMPLE:
	/*std::string VariableGameNameUpdate(const std::vector<std::string>& Arguments)
	{
		// arguments are only passed to variable update funcs in case they need them for something
		// CommandMap already updated the value of the variable so we'll just use that
		std::string name;
		if (!Modules::ModuleGame::Instance().GetVariableString("Name", name))
			// we could also call Modules::CommandMap::Instance()->GetVariableString("Game.Name", name);
			return ""; // should never happen

		return std::string("Our name is ") + name;
	}*/
}
