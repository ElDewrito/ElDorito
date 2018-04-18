#include "ModuleGame.hpp"
#include <sstream>
#include <fstream>
#include <type_traits>
#include <algorithm>
#include "../ElDorito.hpp"
#include "../Patches/Ui.hpp"
#include "../Patches/Logging.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamGraphics.hpp"
#include "../Blam/Tags/Game/GameEngineSettings.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"
#include "../Patches/Core.hpp"
#include "../Patches/Forge.hpp"
#include "../Patches/Maps.hpp"
#include "../Web/WebRenderer.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "ModuleServer.hpp"
#include "../Patch.hpp"
#include "boost/filesystem.hpp"
#include "../Utils/Logger.hpp"
#include "../ThirdParty/rapidjson/rapidjson.h"
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include <unordered_map>
#include <codecvt>
#include "../Blam/Tags/Camera/AreaScreenEffect.hpp"

namespace
{
	bool CommandGameLogLevel(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		for (auto arg : Arguments)
		{
			if (arg.compare("none") == 0)
			{
				Utils::Logger::Instance().Level = Utils::LogLevel::None;
				break;
			}
			else if (arg.compare("trace") == 0)
			{
				Utils::Logger::Instance().Level = Utils::LogLevel::Trace;
				break;
			}
			else if (arg.compare("info") == 0)
			{
				Utils::Logger::Instance().Level = Utils::LogLevel::Info;
				break;
			}
			else if (arg.compare("warning") == 0)
			{
				Utils::Logger::Instance().Level = Utils::LogLevel::Warning;
				break;
			}
			else if (arg.compare("error") == 0)
			{
				Utils::Logger::Instance().Level = Utils::LogLevel::Error;
				break;
			}
		}

		returnInfo = "Log Level: " + Utils::LogLevelToString(Utils::Logger::Instance().Level);

		return true;
	}

	bool CommandGameLogTypes(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		uint32_t types = Utils::Logger::Instance().Types;

		for (auto arg : Arguments)
		{
			if (arg.find("all") != std::string::npos)
			{
				types = Utils::All;
				break;
			}
			else if (arg.find("none") != std::string::npos)
			{
				types = Utils::None;
				break;
			}
			else types = Utils::None;

			if (arg.find("game") != std::string::npos)
			{
				types |= Utils::Game;
			}
			if (arg.find("network") != std::string::npos)
			{
				types |= Utils::Network;
			}
			if (arg.find("graphics") != std::string::npos)
			{
				types |= Utils::Graphics;
			}
			if (arg.find("memory") != std::string::npos)
			{
				types |= Utils::Memory;
			}
			if (arg.find("sound") != std::string::npos)
			{
				types |= Utils::Sound;
			}
			if (arg.find("input") != std::string::npos)
			{
				types |= Utils::Input;
			}
			if (arg.find("debug") != std::string::npos)
			{
				types |= Utils::Debug;
			}
		}

		Utils::Logger::Instance().Types = static_cast<Utils::LogTypes>(types);
		returnInfo = "Log Types: " + Utils::LogTypesToString(Utils::Logger::Instance().Types);

		return true;
	}

	bool CommandGameLogFilter(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::stringstream ss;
		if (Arguments.size() != 3)
		{
			ss << "Usage: Game.LogFilter <include/exclude> <add/remove> <string>" << std::endl << std::endl;
		}
		else
		{
			bool exclude = (Arguments[0].compare("exclude") == 0);
			bool remove = Arguments[1].compare("remove") == 0;
			auto vect = exclude ? &Modules::ModuleGame::Instance().FiltersExclude : &Modules::ModuleGame::Instance().FiltersInclude;
			auto str = Arguments[2];
			bool exists = std::find(vect->begin(), vect->end(), str) != vect->end();
			if (remove && !exists)
			{
				ss << "Failed to find string \"" << str << "\" inside " << (exclude ? "exclude" : "include") << " filters list" << std::endl << std::endl;
			}
			else if (remove)
			{
				auto pos = std::find(vect->begin(), vect->end(), str);
				vect->erase(pos);
				ss << "Removed \"" << str << "\" from " << (exclude ? "exclude" : "include") << " filters list" << std::endl << std::endl;
			}
			else
			{
				vect->push_back(str);
				ss << "Added \"" << str << "\" to " << (exclude ? "exclude" : "include") << " filters list" << std::endl << std::endl;
			}
		}

		ss << "Include filters (message must contain these strings):";

		for (auto filter : Modules::ModuleGame::Instance().FiltersInclude)
			ss << std::endl << filter;

		ss << std::endl << std::endl << "Exclude filters (message must not contain these strings):";

		for (auto filter : Modules::ModuleGame::Instance().FiltersExclude)
			ss << std::endl << filter;

		returnInfo = ss.str();
		return true;
	}

	bool CommandGameInfo(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::stringstream ss;
		std::string ArgList((char*)Pointer(0x199C0A4)[0]);
		std::string LocalSecureKey((char*)Pointer(0x50CCDB4 + 1));
		std::string Xnkid;
		std::string Xnaddr;
		std::string Build((char*)Pointer(0x199C0F0));
		std::string SystemID((char*)Pointer(0x199C130));
		std::string SessionID((char*)Pointer(0x199C1D0));
		std::string DevKitName((char*)Pointer(0x160C8C8));
		std::string DevKitVersion((char*)Pointer(0x199C350));
		std::string FlashVersion((char*)Pointer(0x199C350));
		std::string MapName((char*)Pointer(0x22AB018)(0x1A4));
		std::wstring VariantName((wchar_t*)Pointer(0x23DAF4C));

		Utils::String::BytesToHexString((char*)Pointer(0x2247b80), 0x10, Xnkid);
		Utils::String::BytesToHexString((char*)Pointer(0x2247b90), 0x10, Xnaddr);

		ss << std::hex << "ThreadLocalStorage: 0x" << std::hex << (size_t)(void*)ElDorito::GetMainTls() << std::endl;

		ss << "Command line args: " << (ArgList.empty() ? "(null)" : ArgList) << std::endl;
		ss << "Local Secure Key: " << (LocalSecureKey.empty() ? "(null)" : LocalSecureKey) << std::endl;
		ss << "XNKID: " << Xnkid << std::endl;
		ss << "XNAddr: " << Xnaddr << std::endl;
		ss << "Server Port: " << std::dec << Modules::ModuleServer::Instance().VarServerPort->ValueInt << std::endl;
		ss << "Server Endpoint Port: " << std::dec << Pointer(0x1860454).Read<uint32_t>() << std::endl;
		ss << "Build: " << (Build.empty() ? "(null)" : Build) << std::endl;
		ss << "SystemID: " << (SystemID.empty() ? "(null)" : SystemID) << std::endl;
		ss << "SessionID: " << (SessionID.empty() ? "(null)" : SessionID) << std::endl;
		ss << "SDK Info: " << (DevKitName.empty() ? "(null)" : DevKitName) << '|';
		ss << (DevKitVersion.empty() ? "(null)" : DevKitVersion) << std::endl;

		ss << "Flash Version: " << (FlashVersion.empty() ? "(null)" : FlashVersion) << std::endl;
		ss << "Current Map: " << (MapName.empty() ? "(null)" : MapName) << std::endl;
		ss << "Current Map Cache Size: " << std::dec << Pointer(0x22AB018)(0x8).Read<int32_t>() << std::endl;
		ss << "Loaded Game Variant: " << (VariantName.empty() ? "(null)" : Utils::String::ThinString(VariantName)) << std::endl;
		ss << "Loaded Game Type: 0x" << std::hex << Pointer(0x023DAF18).Read<int32_t>() << std::endl;
		ss << "Tag Table Offset: 0x" << std::hex << Pointer(0x22AAFF4).Read<uint32_t>() << std::endl;
		ss << "Tag Bank Offset: 0x" << std::hex << Pointer(0x22AAFF8).Read<uint32_t>() << std::endl;
		ss << "Players global addr: 0x" << std::hex << ElDorito::GetMainTls(0x40).Read<uint32_t>() << std::endl;

		returnInfo = ss.str();
		return true;
	}

	bool CommandGameExit(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Core::ExecuteShutdownCallbacks();
		std::exit(0);
		return true;
	}

	bool CommandGameRestart(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Core::ExecuteShutdownCallbacks();

		char *cmd = GetCommandLine();
		int offset = 0;
		//Seperate the *.exe from the arguments
		if (cmd[offset] == '"')
			while (cmd[++offset] != '"');
		while (cmd[++offset] != ' ');

		//Relaunch the game with the same arguments
		auto str = static_cast<std::string>(GetCommandLine());
		ShellExecuteA(nullptr, nullptr, str.substr(0, offset).c_str(), str.substr(offset).c_str(), nullptr, SW_SHOWNORMAL);

		//Close the current instance
		std::exit(0);
		return true;
	}

	bool CommandGameForceLoad(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::stringstream ss;
		if (Arguments.size() <= 0)
		{
			ss << "Current map: " << std::string((char*)(0x2391824)) << std::endl;
			ss << "Usage: Game.ForceLoad <mapname> [gametype] [maptype]" << std::endl;
			ss << "Available maps:";

			for (auto map : Modules::ModuleGame::Instance().MapList)
				ss << std::endl << "\t" << map;

			ss << std::endl << std::endl << "Valid gametypes:";
			for (size_t i = 0; i < Blam::GameTypeCount; i++)
				ss << std::endl << "\t" << "[" << i << "] " << Blam::GameTypeNames[i];

			ss << std::endl << std::endl << "Valid gamemodes:";
			for (size_t i = 0; i < Blam::GameModeCount; i++)
				ss << std::endl << "\t" << "[" << i << "] " << Blam::GameModeNames[i];

			returnInfo = ss.str();
			return false;
		}

		auto mapName = Arguments[0];

		auto gameTypeStr = "none";
		Blam::GameType gameType = Blam::GameType::None;
		Blam::GameMode gameMode = Blam::GameMode::Multiplayer;

		if (std::find(Modules::ModuleGame::Instance().MapList.begin(), Modules::ModuleGame::Instance().MapList.end(), mapName) == Modules::ModuleGame::Instance().MapList.end())
		{
			returnInfo = "Unable to find map " + mapName;
			return false;
		}

		mapName = ElDorito::Instance().GetMapsFolder() + mapName;

		if (Arguments.size() >= 2)
		{
			//Look up gametype string.
			size_t i;
			for (i = 0; i < Blam::GameTypeCount; i++)
			{
				// Todo: case insensiive
				if (!Blam::GameTypeNames[i].compare(Arguments[1]))
				{
					gameType = Blam::GameType(i);
					break;
				}
			}

			if (i == Blam::GameTypeCount)
				gameType = (Blam::GameType)std::atoi(Arguments[1].c_str());

			if (gameType > Blam::GameTypeCount) // only valid gametypes are 1 to 10
				gameType = Blam::GameType::Slayer;
		}

		if (Arguments.size() >= 3)
		{
			//Look up gamemode string.
			size_t i;
			for (i = 0; i < Blam::GameModeCount; i++)
			{
				// Todo: case insensiive
				if (!Blam::GameModeNames[i].compare(Arguments[2]))
				{
					gameMode = Blam::GameMode(i);
					break;
				}
			}

			if (i == Blam::GameModeCount)
				gameMode = (Blam::GameMode)std::atoi(Arguments[2].c_str());

			if (gameMode > Blam::GameModeCount) // only valid gametypes are 1 to 10
				gameMode = Blam::GameMode::Multiplayer;
		}

		ss << "Loading " << mapName << " gametype: " << Blam::GameTypeNames[gameType] << " gamemode: " << Blam::GameModeNames[gameMode];

		// Game Type
		Pointer(0x2391B2C).Write<uint32_t>(gameType);

		// Infinite play time
		Pointer(0x2391C51).Write<uint8_t>(0);

		// Game Mode
		Pointer(0x2391800).Write<uint32_t>(gameMode);

		// Map Name
		Pointer(0x2391824).Write(mapName.c_str(), mapName.length() + 1);

		// Map Reset
		Pointer(0x23917F0).Write<uint8_t>(0x1);

		returnInfo = ss.str();
		return true;
	}

	bool CommandGameShowUI(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() <= 0)
		{
			returnInfo = "You must specify at least a dialog ID!";
			return false;
		}

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
			returnInfo = "Invalid argument given.";
			return false;
		}
		catch (std::out_of_range)
		{
			returnInfo = "Invalid argument given.";
			return false;
		}

		Patches::Ui::ShowDialog(dialogStringId, dialogArg1, dialogFlags, dialogParentStringId);

		returnInfo = "Sent Show_UI notification to game.";
		return true;
	}

	size_t GetFileSize(std::ifstream &file)
	{
		auto originalPos = file.tellg();
		file.seekg(0, std::ios::end);
		auto size = static_cast<size_t>(file.tellg());
		file.seekg(originalPos);
		return size;
	}

	bool LoadMapVariant(std::ifstream &file, uint8_t *out)
	{
		// TODO: Would it be better to figure out how to use the game's file
		// functions here?

		// Verify file size
		const auto MapVariantBlfSize = 0xE1F0;
		if (GetFileSize(file) < MapVariantBlfSize)
			return false;

		// Load it into a buffer and have the game parse it
		uint8_t blfData[MapVariantBlfSize];
		file.read(reinterpret_cast<char*>(blfData), MapVariantBlfSize);

		typedef bool(__thiscall *ParseMapVariantBlfPtr)(void *blf, uint8_t *outVariant, bool *result);
		auto ParseMapVariant = reinterpret_cast<ParseMapVariantBlfPtr>(0x573250);
		return ParseMapVariant(blfData, out, nullptr);
	}

	int GetMapId(const std::string &mapName)
	{
		// Open the .map file
		auto mapPath = ElDorito::Instance().GetMapsFolder() + mapName + ".map";
		std::ifstream mapFile(mapPath, std::ios::binary);
		if (!mapFile.is_open())
			return -1;

		// Verify file size
		const auto MapHeaderSize = 0x3390;
		if (GetFileSize(mapFile) < MapHeaderSize)
			return -1;

		// Verify file header
		const int32_t FileHeaderMagic = 'head';
		int32_t actualMagic = 0;
		mapFile.read(reinterpret_cast<char*>(&actualMagic), sizeof(actualMagic));
		if (actualMagic != FileHeaderMagic)
			return -1;

		// Read map ID
		int32_t mapId = 0;
		mapFile.seekg(0x2DEC);
		mapFile.read(reinterpret_cast<char*>(&mapId), sizeof(mapId));
		return mapId;
	}

	bool LoadDefaultMapVariant(const std::string &mapName, uint8_t *out)
	{
		int mapId = GetMapId(mapName);
		if (mapId < 0)
			return false;

		// Initialize an empty variant for the map
		Patches::Maps::InitializeMapVariant((Blam::MapVariant*)out, mapId);

		// Make sure it actually loaded the map correctly by verifying that the
		// variant is valid for the map
		int32_t firstMapId = *reinterpret_cast<int32_t*>(out + 0xE0);
		int32_t secondMapId = *reinterpret_cast<int32_t*>(out + 0x100);
		return (firstMapId == mapId && secondMapId == mapId);
	}

	// TODO: Might be useful to map out this enum
	// 2 = customs, 3 = forge
	int GetUiGameMode()
	{
		typedef int(__thiscall *GetUiGameModePtr)();
		auto GetUiGameModeImpl = reinterpret_cast<GetUiGameModePtr>(0x435640);
		return GetUiGameModeImpl();
	}

	void SaveMapVariantToPreferences(const uint8_t *data)
	{
		// Check the lobby type so we know where to save the variant
		size_t variantOffset;
		switch (GetUiGameMode())
		{
		case 2: // Customs
			variantOffset = 0x7F0;
			break;
		case 3: // Forge
			variantOffset = 0xEA98;
			break;
		default:
			return;
		}

		// Copy the data in
		auto savedVariant = reinterpret_cast<uint8_t*>(0x22C0130 + variantOffset);
		memcpy(savedVariant, data, 0xE090);

		// Mark preferences as dirty
		*reinterpret_cast<bool*>(0x22C0129) = true;
	}

	void SaveGameVariantToPreferences(const uint8_t *data)
	{
		if (GetUiGameMode() != 2)
			return; // Only allow doing this from a customs lobby

		// Copy the data in
		auto savedVariant = reinterpret_cast<uint8_t*>(0x22C0130 + 0x37C);
		memcpy(savedVariant, data, 0x264);

		// Mark preferences as dirty
		*reinterpret_cast<bool*>(0x22C0129) = true;
	}

	bool CommandGameLoadMap(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() != 1)
		{
			returnInfo = "You must specify an internal map or Forge map name!";
			return false;
		}
		auto lobbyType = GetUiGameMode();
		if (lobbyType != 2 && lobbyType != 3)
		{
			returnInfo = "You can only change maps from a Custom Games or Forge lobby.";
			return false;
		}
		auto mapName = Arguments[0];
		uint8_t variantData[0xE090];

		// If the name is the name of a valid map variant, load it
		auto variantFileName = "mods/maps/" + mapName + "/sandbox.map";
		std::ifstream mapVariant(variantFileName, std::ios::binary);
		if (mapVariant.is_open())
		{
			returnInfo = "Loading map variant " + variantFileName + "...";
			if (!LoadMapVariant(mapVariant, variantData))
			{
				returnInfo += "\nInvalid map variant file!";
				return false;
			}
		}
		else
		{
			returnInfo = "Loading built-in map maps/" + mapName + ".map...";
			if (!LoadDefaultMapVariant(mapName, variantData))
			{
				returnInfo += "\nInvalid map file!";
				return false;
			}
		}

		// Submit a request to load the variant
		typedef bool(*LoadMapVariantPtr)(uint8_t *variant, void *unknown);
		auto LoadMapVariant = reinterpret_cast<LoadMapVariantPtr>(0xA83AF0);
		if (!LoadMapVariant(variantData, nullptr))
		{
			returnInfo += "\nLoad failed.";
			return false;
		}
		SaveMapVariantToPreferences(variantData);
		returnInfo += "\nMap variant loaded successfully!";
		return true;
	}

	bool LoadGameVariant(std::ifstream &file, uint8_t *out)
	{
		// Verify file size
		const auto GameVariantBlfSize = 0x3BC;
		if (GetFileSize(file) < GameVariantBlfSize)
			return false;

		// Load it into a buffer and have the game parse it
		uint8_t blfData[GameVariantBlfSize];
		file.read(reinterpret_cast<char*>(blfData), GameVariantBlfSize);

		typedef bool(__thiscall *ParseGameVariantBlfPtr)(void *blf, uint8_t *outVariant, bool *result);
		auto ParseGameVariant = reinterpret_cast<ParseGameVariantBlfPtr>(0x573150);
		return ParseGameVariant(blfData, out, nullptr);
	}

	template<class T>
	int FindDefaultGameVariant(const Blam::Tags::TagBlock<T> &variants, const std::string &name)
	{
		static_assert(std::is_base_of<Blam::Tags::Game::GameVariant, T>::value, "T must be a GameVariantDefinition");
		if (!variants)
			return -1;
		for (auto i = 0; i < variants.Count; i++)
		{
			if (variants[i].Name[0] && strcmp(variants[i].Name, name.c_str()) == 0)
				return i;
		}
		return -1;
	}

	int FindDefaultGameVariant(Blam::Tags::Game::GameEngineSettingsDefinition *wezr, Blam::GameType type, const std::string &name)
	{
		switch (type)
		{
		case Blam::GameType::CTF:
			return FindDefaultGameVariant(wezr->CTFVariants, name);
		case Blam::GameType::Slayer:
			return FindDefaultGameVariant(wezr->SlayerVariants, name);
		case Blam::GameType::Oddball:
			return FindDefaultGameVariant(wezr->OddballVariants, name);
		case Blam::GameType::KOTH:
			return FindDefaultGameVariant(wezr->KOTHVariants, name);
		case Blam::GameType::VIP:
			return FindDefaultGameVariant(wezr->VIPVariants, name);
		case Blam::GameType::Juggernaut:
			return FindDefaultGameVariant(wezr->JuggernautVariants, name);
		case Blam::GameType::Territories:
			return FindDefaultGameVariant(wezr->TerritoriesVariants, name);
		case Blam::GameType::Assault:
			return FindDefaultGameVariant(wezr->AssaultVariants, name);
		case Blam::GameType::Infection:
			return FindDefaultGameVariant(wezr->InfectionVariants, name);
		default: // None, Forge
			return -1;
		}
	}

	bool LoadDefaultGameVariant(const std::string &name, uint8_t *out)
	{
		// Get a handle to the wezr tag
		typedef Blam::Tags::Game::GameEngineSettingsDefinition *(*GetWezrTagPtr)();
		auto GetWezrTag = reinterpret_cast<GetWezrTagPtr>(0x719290);
		auto *wezr = GetWezrTag();
		if (!wezr)
			return false;

		// Search through each variant type until something is found
		auto index = -1;
		int type;
		for (type = 1; type < Blam::GameType::GameTypeCount; type++)
		{
			index = FindDefaultGameVariant(wezr, static_cast<Blam::GameType>(type), name);
			if (index != -1)
				break;
		}
		if (type == Blam::GameType::GameTypeCount)
			return false;

		const auto VariantDataSize = 0x264;
		memset(out, 0, VariantDataSize);

		// Ask the game to generate the variant data
		typedef bool(*LoadBuiltInGameVariantPtr)(Blam::GameType type, int index, uint8_t *out);
		auto LoadBuiltInGameVariant = reinterpret_cast<LoadBuiltInGameVariantPtr>(0x572270);
		return LoadBuiltInGameVariant(static_cast<Blam::GameType>(type), index, out);
	}

	const std::string GameTypeExtensions[] =
	{
		"ctf",
		"slayer",
		"oddball",
		"koth",
		"jugg",
		"terries",
		"assault",
		"zombiez",
		"vip",
	};

	bool CommandGameType(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() != 1)
		{
			returnInfo = "You must specify a built-in gametype or custom gametype name!";
			return false;
		}
		if (GetUiGameMode() != 2)
		{
			returnInfo = "You can only change gametypes from a Custom Games lobby.";
			return false;
		}
		auto name = Arguments[0];
		uint8_t variantData[0x264];

		// Check if this is a custom gametype by searching for a file
		// corresponding to each supported game mode
		std::ifstream gameVariant;
		std::string variantFileName;
		for (auto &&extension : GameTypeExtensions)
		{
			variantFileName = "mods/variants/" + name + "/variant." + extension;
			gameVariant.open(variantFileName, std::ios::binary);
			if (gameVariant.is_open())
				break;
		}
		if (gameVariant.is_open())
		{
			returnInfo = "Loading game variant " + variantFileName + "...";
			if (!LoadGameVariant(gameVariant, variantData))
			{
				returnInfo += "\nInvalid game variant file!";
				return false;
			}
		}
		else
		{
			returnInfo = "Loading built-in game variant " + name + "...";
			if (!LoadDefaultGameVariant(name, variantData))
			{
				returnInfo += "\nInvalid game variant name!";
				return false;
			}
		}

		// Submit a request to load the variant
		typedef bool(*LoadGameVariantPtr)(uint8_t *variant);
		auto LoadGameVariant = reinterpret_cast<LoadGameVariantPtr>(0x439860);
		if (!LoadGameVariant(variantData))
		{
			returnInfo += "\nLoad failed.";
			return false;
		}
		SaveGameVariantToPreferences(variantData);
		returnInfo += "\nGame variant loaded successfully!";
		return true;
	}

	bool CommandGameStart(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->Parameters.SetSessionMode(2))
		{
			returnInfo = "Unable to start the game!";
			return false;
		}
		returnInfo = "Starting game...";
		return true;
	}

	bool CommandGameStop(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->Parameters.SetSessionMode(1))
		{
			returnInfo = "Unable to stop the game!";
			return false;
		}
		returnInfo = "Stopping game...";
		return true;
	}

	bool CommandGameLeave(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			returnInfo = "Unable to leave the game!";
			return false;
		}

		Blam::Network::LeaveGame();

		returnInfo = "Leaving game...";
		return true;
	}

	bool CommandGameEnd(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
		{
			returnInfo = "Unable to end the game!";
			return false;
		}

		Blam::Network::EndGame();

		returnInfo = "Ending game...";
		return true;
	}

	bool CommandGameVersion(const std::vector<std::string>& arguments, std::string& returnInfo) {
		returnInfo = Utils::Version::GetVersionString();
		return true;
	}

	bool CommandGameSetMenuEnabled(const std::vector<std::string>& arguments, std::string& returnInfo) {
		bool enabled = arguments.size() < 1; //defaults to true
		if (!enabled) {
			std::string e(arguments[0].size(), NULL);
			std::transform(arguments[0].begin(), arguments[0].end(), e.begin(), ::tolower);
			enabled = e != "0" && e != "false";
		}
		if (enabled)
			Web::Ui::ScreenLayer::Show("browser", "{}");
		else
			Web::Ui::ScreenLayer::Hide("browser");
		returnInfo = (enabled) ? "Enabling menu..." : "Disabling menu...";
		return true;
	}

	bool CommandListMaps(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		// Return a comma-separated list
		for (auto&& name : Modules::ModuleGame::Instance().MapList)
		{
			if (returnInfo.length() > 0)
				returnInfo += ',';
			returnInfo += name;
		}
		return true;
	}

	bool CommandShowScreen(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		if (arguments.size() == 0)
		{
			returnInfo = "Usage: Game.ShowScreen <screen name> <opt json>";
			return false;
		}
		if (arguments[0].find(' ') == std::string::npos)
		{
			Web::Ui::ScreenLayer::Show(arguments[0], "{}");
			return true;
		}

		int pos = arguments[0].find(' ') + 1;
		std::string jsonArg = arguments[0].substr(pos);
		rapidjson::Document doc;
		doc.Parse(jsonArg.c_str());
		if (doc.HasParseError())
		{
			returnInfo = "The passed json argument was invalid: " + jsonArg;
			return false;
		}
		rapidjson::StringBuffer buff;
		buff.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
		doc.Accept(writer);
		Web::Ui::ScreenLayer::Show(arguments[0].substr(0, pos - 1), buff.GetString());
		return true;
	}
	
	bool CommandListMedalPacks(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		Modules::ModuleGame::Instance().MedalPackList.clear();
		boost::filesystem::path p("mods/medals/");
		boost::filesystem::directory_iterator end_itr;
		if (boost::filesystem::exists(p))
		{
			// cycle through the directory
			for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
			{
				if (!is_regular_file(itr->path()))
				{
					std::string file = std::string(itr->path().generic_string());
					file.append("\\events.json");
					auto path = boost::filesystem::path(file);
					if (boost::filesystem::exists(path))
						Modules::ModuleGame::Instance().MedalPackList.push_back(itr->path().filename().string());
				}
			}
		}
		// Return a comma-separated list
		for (auto&& name : Modules::ModuleGame::Instance().MedalPackList)
		{
			if (returnInfo.length() > 0)
				returnInfo += ',';
			returnInfo += name;
		}
		return true;
	}

	bool TryParseTagIndex(const std::string& str, uint32_t* value)
	{
		if (str.length() == 0)
			return false;

		auto c_str = str.c_str();
		char* endp;

		*value = std::strtol(c_str, &endp, 16);

		return endp != c_str;
	}

	bool CommandPlaySound(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		static auto Sound_PlaySoundEffect = (void(*)(uint32_t sndTagIndex, float volume))(0x5DE300);

		uint32_t tagIndex;
		if (arguments.size() < 1 || !TryParseTagIndex(arguments[0], &tagIndex))
		{
			returnInfo = "Invalid arguments";
			return false;
		}

		//Make sure the sound exists before playing
		if (Blam::Tags::TagInstance::IsLoaded('snd!', tagIndex))
			Sound_PlaySoundEffect(tagIndex, 1.0f);
		else
			returnInfo = "Invalid sound index: " + arguments[0];

		return true;
	}

	bool CommandPlayLoopingSound(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		static auto Sound_LoopingSound_Start = (void(*)(uint32_t sndTagIndex, int a2, int a3, int a4, char a5))(0x5DC530);

		uint32_t tagIndex;
		if (arguments.size() < 1 || !TryParseTagIndex(arguments[0], &tagIndex))
		{
			returnInfo = "Invalid arguments";
			return false;
		}

		//Make sure the sound exists before playing
		if (Blam::Tags::TagInstance::IsLoaded('lsnd', tagIndex))
			Sound_LoopingSound_Start(tagIndex, -1, 1065353216, 0, 0);
		else
			returnInfo = "Invalid sound index: " + arguments[0];

		return true;
	}

	bool CommandStopLoopingSound(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		static auto Sound_LoopingSound_Stop = (void(*)(uint32_t sndTagIndex, int a2))(0x5DC6B0);

		uint32_t tagIndex;
		if (arguments.size() < 1 || !TryParseTagIndex(arguments[0], &tagIndex))
		{
			returnInfo = "Invalid arguments";
			return false;
		}

		//Make sure the sound exists before playing
		if (Blam::Tags::TagInstance::IsLoaded('lsnd', tagIndex))
			Sound_LoopingSound_Stop(tagIndex, -1);
		else
			returnInfo = "Invalid sound index: " + arguments[0];

		return true;
	}

	bool CommandGameTakeScreenshot(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		wchar_t *path = Blam::Graphics::TakeScreenshot();
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;
		std::string screenshot_path = wstring_to_string.to_bytes(path);

		if (!Modules::ModuleGame::Instance().VarScreenshotNoticeDisabled->ValueInt)
		{
			rapidjson::StringBuffer jsonBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
			jsonWriter.StartObject();
			jsonWriter.Key("filepath");
			jsonWriter.String(screenshot_path.c_str());
			jsonWriter.EndObject();
			Web::Ui::ScreenLayer::Notify("screenshot_taken", jsonBuffer.GetString(), true);
		}

		returnInfo = "Screenshot saved to: " + screenshot_path;
		return true;
	}

	bool CommandExecuteScenarioScript(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		static auto Scenario_ExecuteScript = (uint32_t(*)(int16_t scriptIndex, int scriptType, bool a3))(0x598050);

		if (!Arguments.size())
		{
			returnInfo = "Expected script name";
			return false;
		}

		if (*(uint8_t*)0x023917F0) //is loading
		{
			returnInfo = "Can not run script while in a loading screen";
			return false;
		}

		auto scnr = Blam::Tags::Scenario::GetCurrentScenario();
		if (!scnr)
		{
			returnInfo = "No scenario loaded";
			return false;
		}

		for (auto i = 0; i < scnr->Scripts.Count; i++)
		{
			const auto& script = scnr->Scripts.Elements[i];

			if (!stricmp(script.ScriptName, Arguments[0].c_str()))
			{
				if (Scenario_ExecuteScript(i, script.ScriptType, 0) == -1)
				{
					returnInfo = "Failed execution failed";
					return false;
				}
				return true;
			}
		}

		returnInfo = "Script not found";
		return false;
	}


	bool CommandGetTagAddress(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() <= 0)
		{
			returnInfo = "You must specify a tag index!";
			return false;
		}

		int tagIndex = 0;
		try
		{
			tagIndex = std::stoi(Arguments[0], 0, 0);
		}
		catch (std::invalid_argument)
		{
			returnInfo = "Invalid argument given.";
			return false;
		}

		typedef void *(*GetTagAddressPtr)(int groupTag, uint32_t index);
		auto GetTagAddressImpl = reinterpret_cast<GetTagAddressPtr>(0x503370);

		void* address = GetTagAddressImpl(0, tagIndex);
		std::stringstream ss;
		ss << "Tag 0x" << std::hex << tagIndex << " is located at 0x" << std::hex << (int)address;
		returnInfo = ss.str();
		return true;
	}


	bool CommandScreenEffectRange(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 2)
		{
			returnInfo = "You must include an index and a range!";
			return false;
		}

		int index = 0;
		float range = 0;
		try
		{
			index = std::stoi(Arguments[0], 0, 0);
			range = std::stof(Arguments[1]);
		}
		catch (std::invalid_argument)
		{
			returnInfo = "Invalid argument given.";
			return false;
		}

		auto scnr = Blam::Tags::Scenario::GetCurrentScenario();
		if (!scnr)
		{
			returnInfo = "A scenario must be loaded!";
			return false;
		}

		auto sefcIndex = scnr->DefaultScreenFx;
		if(!sefcIndex)
		{
			returnInfo = "Current scenario does not have a default screen fx";
			return false;
		}

		auto sefc = Blam::Tags::TagInstance(sefcIndex.TagIndex).GetDefinition<Blam::Tags::Camera::AreaScreenEffect>();
		if(sefc)
		{
			sefc->ScreenEffects[index].MaximumDistance = range;
			return true;
		}

		returnInfo = "Failed to get sefc";
		return false;

	}

	bool VariableLanguageUpdated(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		if (arguments.size() < 1)
			return false;

		static const std::unordered_map<std::string, int> languageIds =
		{
			{ "english", 0 },
			{ "japanese", 1 },
			{ "german", 2 },
			{ "french", 3 },
			{ "spanish", 4 },
			{ "mexican", 5 },
			{ "italian", 6 },
			{ "korean", 7 },
			{ "chinese", 9 },
			{ "portuguese", 10 },
			{ "russian", 11 }
		};

		auto it = languageIds.find(Utils::String::ToLower(arguments[0]));
		if (it == languageIds.end())
			return false;

		*(uint32_t*)0x189DEE4 = it->second;
		return true;
	}

	bool ShowTickrate()
	{
		auto Tickrate = Pointer(0x22B47FC);
		Tickrate.Write(!Tickrate.Read<bool>());
		return Tickrate.Read<bool>();
	}

	bool CommandShowTickrate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		std::stringstream ss;
		ss << "Tickrate: " << (ShowTickrate() ? "shown." : "hidden.");
		returnInfo = ss.str();
		return true;
	}

	bool CommandGameUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto ret = int(ShellExecuteA(nullptr, nullptr, "updater.exe", "", nullptr, SW_SHOWNORMAL));
		if (ret <= 32)
		{
			if (ret == ERROR_FILE_NOT_FOUND || ret == ERROR_PATH_NOT_FOUND)
			{
				returnInfo = "\"updater.exe\" not found in current directory";
			}
			else
			{
				char error[256];
				sprintf_s(error, "failed to execute updater. error: %d %d", ret, GetLastError());
				returnInfo = error;
			}
			return false;
		}
		return CommandGameExit(Arguments, returnInfo);
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

namespace Modules
{
	ModuleGame::ModuleGame() : ModuleBase("Game")
	{
		AddCommand("LogLevel", "loglevel", "Debug log verbosity level", eCommandFlagsNone, CommandGameLogLevel, { "trace|info|warning|error|none The log verbosity level" });

		AddCommand("LogTypes", "logtypes", "Chooses which kinds of debug messages to print to the log file", eCommandFlagsNone, CommandGameLogTypes, { "game|network|graphics|memory|sound|input|debug|all|none The message types to log" });

		AddCommand("LogFilter", "debug_filter", "Allows you to set filters to apply to the debug messages", eCommandFlagsNone, CommandGameLogFilter, { "include/exclude The type of filter", "add/remove Add or remove the filter", "string The filter to add" });

		AddCommand("Info", "info", "Displays information about the game", eCommandFlagsNone, CommandGameInfo);

		AddCommand("Exit", "exit", "Ends the game process", eCommandFlagsNone, CommandGameExit);

		AddCommand("Restart", "restart", "Restart the game process", eCommandFlagsNone, CommandGameRestart);

		AddCommand("ForceLoad", "forceload", "Forces a map to load", eCommandFlagsNone, CommandGameForceLoad, { "mapname(string) The name of the map to load", "gametype(int) The gametype to load", "gamemode(int) The type of gamemode to play", });

		AddCommand("ShowUI", "show_ui", "Attempts to force a UI widget to open", eCommandFlagsNone, CommandGameShowUI, { "dialogID(int) The dialog ID to open", "arg1(int) Unknown argument", "flags(int) Unknown argument", "parentdialogID(int) The ID of the parent dialog" });

		AddCommand("Map", "map", "Loads a map or map variant", eCommandFlagsNone, CommandGameLoadMap, { "name(string) The internal name of the map or Forge map to load" });

		AddCommand("GameType", "gametype", "Loads a gametype", eCommandFlagsNone, CommandGameType, { "name(string) The internal name of the built-in gametype or custom gametype to load" });

		AddCommand("Start", "start", "Starts or restarts the game", eCommandFlagsNone, CommandGameStart);

		AddCommand("End", "end", "Ends the game", eCommandFlagsNone, CommandGameEnd);

		AddCommand("Stop", "stop", "Stops the game, goes back to lobby", eCommandFlagsNone, CommandGameStop);

		AddCommand("Leave", "leave", "Leaves the game, goes back to menu", eCommandFlagsNone, CommandGameLeave);

		AddCommand("Version", "version", "Displays the game's version", eCommandFlagsNone, CommandGameVersion);

		AddCommand("SetMenuEnabled", "set_menu", "Sets whether the server browser is currently open", eCommandFlagsRunOnMainMenu, CommandGameSetMenuEnabled);

		AddCommand("ListMaps", "maps", "List all available map files", eCommandFlagsNone, CommandListMaps);

		AddCommand("ShowScreen", "showscreen", "Displays the specified screen", eCommandFlagsArgsNoParse, CommandShowScreen);

		AddCommand("ListMedalPacks", "list_medals", "List all available medal packs", eCommandFlagsNone, CommandListMedalPacks);

		AddCommand("PlaySound", "play_sound", "Plays a sound effect", CommandFlags(eCommandFlagsHidden | eCommandFlagsOmitValueInList), CommandPlaySound);

		AddCommand("PlayLoopingSound", "play_looping_sound", "Plays a looping sound effect", CommandFlags(eCommandFlagsHidden | eCommandFlagsOmitValueInList), CommandPlayLoopingSound);

		AddCommand("StopLoopingSound", "stop_looping_sound", "Stops a looping sound effect", CommandFlags(eCommandFlagsHidden | eCommandFlagsOmitValueInList), CommandStopLoopingSound);

		AddCommand("TakeScreenshot", "take_screenshot", "Take a screenshot", eCommandFlagsNone, CommandGameTakeScreenshot);

		AddCommand("ScenarioScript", "scnr_script", "Executes a scenario script", eCommandFlagsNone, CommandExecuteScenarioScript);

		AddCommand("TagAddress", "tag_address", "Gets the address of a tag in memory", eCommandFlagsNone, CommandGetTagAddress);

		AddCommand("ScreenEffectRange", "sefc_range", "Set the range of the default screen FX in the current scnr", eCommandFlagsNone, CommandScreenEffectRange, { "Index(int) sefc effect index", "Range(float) effect range" });

		AddCommand("ShowTickrate", "show_tickrate", "Toggle the on-screen Tickrate", eCommandFlagsNone, CommandShowTickrate);

		AddCommand("Update", "update", "Update the game to the latest version", eCommandFlagsNone, CommandGameUpdate);

		VarMenuURL = AddVariableString("MenuURL", "menu_url", "url(string) The URL of the page you want to load inside the menu", eCommandFlagsArchived, "http://scooterpsu.github.io/");

		VarLanguage = AddVariableString("Language", "language", "The language to use", eCommandFlagsArchived, "english", VariableLanguageUpdated);

		VarSkipTitleSplash = AddVariableInt("SkipTitleSplash", "titlesplash", "Skip the ElDewrito splash screen and go straight to the main menu", eCommandFlagsArchived, 0);
		VarSkipTitleSplash->ValueIntMin = 0;
		VarSkipTitleSplash->ValueIntMax = 1;

		VarSkipIntroVideos = AddVariableInt("SkipIntroVideos", "skipintro", "Skip the game's intro videos.", eCommandFlagsArchived, 0);
		VarSkipIntroVideos->ValueIntMin = 0;
		VarSkipIntroVideos->ValueIntMax = 1;

		VarLogName = AddVariableString("LogName", "debug_logname", "Filename to store debug log messages", eCommandFlagsArchived, "dorito.log");

		VarRconPort = AddVariableInt("RconPort", "rcon_port", "The port to use for rcon in this instance of the game", eCommandFlagsArchived, 11776);

		VarMedalPack = AddVariableString("MedalPack", "medals", "The name of the medal pack to use", eCommandFlagsArchived, "default");

		VarIconSet = AddVariableString("IconSet", "icons", "The name of the controller button icon set to use", eCommandFlagsArchived, "360");

		VarFirstRun = AddVariableInt("FirstRun", "first_run", "Show the first run setup next time the game starts", eCommandFlagsArchived, 1);
		VarFirstRun->ValueIntMin = 0;
		VarFirstRun->ValueIntMax = 1;

		VarExpandedScoreboard = AddVariableInt("ExpandedScoreboard", "expanded_scoreboard", "Always show full scoreboard, instead of just while locked/postgame", eCommandFlagsArchived, 0);
		VarExpandedScoreboard->ValueIntMin = 0;
		VarExpandedScoreboard->ValueIntMax = 1;

		VarHideChat = AddVariableInt("HideChat", "hide_chat", "Hide in-game chat screen", eCommandFlagsArchived, 0);
		VarHideChat->ValueIntMin = 0;
		VarHideChat->ValueIntMax = 1;

		VarSuppressJuggling = AddVariableInt("SuppressJuggling", "Suppress_Juggling", "Suppress spam caused by flag/bomb juggling", eCommandFlagsArchived, 0);
		VarSuppressJuggling->ValueIntMin = 0;
		VarSuppressJuggling->ValueIntMax = 1;

		VarHideH3UI = AddVariableInt("HideH3UI", "hide_h3ui", "Hide the H3 UI", eCommandFlagsHidden, 0);
		VarHideH3UI->ValueIntMin = 0;
		VarHideH3UI->ValueIntMax = 1;

		VarScreenshotsFolder = AddVariableString("ScreenshotsFolder", "screenshots_folder", "The location where the game will save screenshots", eCommandFlagsArchived, "%userprofile%\\Pictures\\Screenshots\\blam");
		VarScreenshotNoticeDisabled = AddVariableInt("ScreenshotNoticeDisabled", "screenshot_notice_disabled", "Disables the screenshot notifications", eCommandFlagsArchived, 0);

		VarCefMedals = AddVariableInt("CefMedals", "cef_medals", "Enable/disable cef medals. When disabled fallback to the H3 medal system.", eCommandFlagsArchived, 0);

		VarFpsLimiter = AddVariableInt("FPSLimiter", "fps_limiter", "Enable/disable framerate limiter (improves frame timing at the cost of cpu usage)", eCommandFlagsArchived, 1);

		VarDiscordEnable = AddVariableInt("Discord.Enable", "discord.enable", "Enable/disable discord integration", eCommandFlagsArchived, 1);
		VarDiscordAutoAccept = AddVariableInt("Discord.AutoAccept", "discord.auto_accept", "Allow auto accepting join requests", eCommandFlagsArchived, 0);

		/*EXAMPLES: adds a variable "Game.Name", default value ElDewrito, calls VariableGameNameUpdate when value is updated
		AddVariableString("Name", "gamename", "Title of the game", "ElDewrito", VariableGameNameUpdate);

		// adds a variable "Game.Year", default value 2015, clamped to [1994..3030]
		auto cmd = AddVariableInt("Year", "gameyear", "The current year", 2015);
		cmd->ValueIntMin = 1994;
		cmd->ValueIntMax = 3030;

		// adds a variable "Game.Money", default value 1.86
		AddVariableFloat("Money", "gamemoney", "Your mothers hourly rate", 1.86f);*/
	}

	void ModuleGame::UpdateCustomMapList()
	{
		CustomMapList.clear();
		boost::filesystem::path p("mods/maps/");
		boost::filesystem::directory_iterator end_itr;
		if (boost::filesystem::exists(p))
		{
			// cycle through the directory
			for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
			{
				if (!is_regular_file(itr->path()))
					CustomMapList.push_back(itr->path().filename().string());
			}
		}

	}
	void ModuleGame::UpdateMapList()
	{
		auto searchPath = ElDorito::Instance().GetMapsFolder() + "*.map";
		WIN32_FIND_DATA find;
		auto handle = FindFirstFile(searchPath.c_str(), &find);
		if (handle != INVALID_HANDLE_VALUE)
		{
			// Get a list of all map names and sort them alphabetically
			MapList.clear();
			do
			{
				std::string name = find.cFileName;
				name = name.substr(0, name.length() - 4); // Remove .map extension
				MapList.push_back(name);
			} while (FindNextFile(handle, &find));
			FindClose(handle);
			std::sort(MapList.begin(), MapList.end());
		}
	}
}
