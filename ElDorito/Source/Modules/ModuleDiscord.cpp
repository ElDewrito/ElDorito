#include "ModuleDiscord.hpp"
#include "../ElDorito.hpp"
#include "discord-rpc.h"
#include "../Utils/Logger.hpp"
#include "../Utils/String.hpp"
#include "ModuleServer.hpp"
#include "../Patches/Core.hpp"

static const char* APPLICATION_ID = "378684431830876170";

namespace
{
	void DiscordReady()
	{
#ifdef _DEBUG
		printf("Discord Rich Presence is ready!");
#endif 
	}

	void handleDiscordError(int errorCode, const char* message)
	{
		std::string output = "-Discord Error-\nCode: " + std::to_string(errorCode) + "\nMessage: " + message;
		Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Error, output);
	}

	void handleDiscordDisconnected(int errorCode, const char* message)
	{
		std::string output = "-Discord Error-\nCode: " + std::to_string(errorCode) + "\nMessage: " + message;
		Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Warning, output);
	}

	/* This handles joining games. handleDiscordJoinRequest sends discord a secret,
	   which is then sent to the other game and is handled by this message. */
	void handleDiscordJoinGame(const char* joinSecret)
	{
		/* Not yet implemented. You need to be approved to use this feature, but it 
		   is feasable. Probably can just send the IP in clear unless lobbies are
		   implemented someday. */
	}

	void handleDiscordSpectateGame(const char* spectateSecret)
	{
		//Unimplemented, won't ever be implemented.
	}
	
	/* This handles recieving requests from discord. It replies to discord with a 
		secret, which is then accepted by the other game's handleDiscordJoinGame. */
	void handleDiscordJoinRequest(const DiscordJoinRequest* request) {
		//Unimplemented yet
	}

	void InitDiscord()
	{
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = DiscordReady;
		handlers.errored = handleDiscordError;
		handlers.disconnected = handleDiscordDisconnected;
		handlers.joinGame = handleDiscordJoinGame;
		handlers.spectateGame = handleDiscordSpectateGame;
		handlers.joinRequest = handleDiscordJoinRequest;
		Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
	}

	void UpdatePresence()
	{
		std::string BaseMapName((char*)Pointer(0x22AB018)(0x1A4));
		std::string VariantName(Utils::String::ThinString((wchar_t*)Pointer(0x23DAF4C)));

		//Cant do this in the if or the memory will be invalidated when leaving if scope.
		std::string detstr = ("Playing " + VariantName + " on " + BaseMapName);
		std::string multiplayer = "Multiplayer";
		std::string multiplayerWithServer = "Multiplayer on " + Modules::ModuleServer::Instance().VarServerNameClient->ValueString;
		std::string mainmenu = "At the Main Menu";

		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));
		if (!(BaseMapName == "mainmenu" || VariantName.empty() || VariantName == "")) {
			discordPresence.state = multiplayerWithServer.c_str();
			discordPresence.details = detstr.c_str();
			discordPresence.largeImageKey = BaseMapName.c_str();
		}
		else {
			discordPresence.state = mainmenu.c_str();
			discordPresence.details = NULL;
			discordPresence.largeImageKey = NULL;
		}
		Discord_UpdatePresence(&discordPresence);
	}

	void UpdatePresenceMap(const char* mapPath) {
		UpdatePresence();
	}

	bool CommandTestPresence(const std::vector<std::string>& Arguments, std::string& returnInfo) {
		UpdatePresence();
		return true;
	}
}

namespace Modules
{
	ModuleDiscord::ModuleDiscord() : ModuleBase("Discord")
	{
		TestPresence = AddCommand("TestDiscordRichPresence", "testpres", "Tests the Rich Presence Integration with Discord.", eCommandFlagsNone, CommandTestPresence);
		Patches::Core::OnGameStart(UpdatePresence);
		Patches::Core::OnShutdown(UpdatePresence);
		Patches::Core::OnMapLoaded(UpdatePresenceMap);
	}
	void ModuleDiscord::DiscordInit()
	{
		::InitDiscord();
	}
	void ModuleDiscord::PresenceUpdate()
	{
		::UpdatePresence();
	}
}