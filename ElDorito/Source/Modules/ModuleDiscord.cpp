#include "ModuleDiscord.hpp"
#include "../ElDorito.hpp"
#include "discord-rpc.h"
#include "../Utils/Logger.hpp"

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
		char buffer[256];
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));
		discordPresence.state = "Testing Rich Presence";
		discordPresence.details = "Learning the ED codebase";
		Discord_UpdatePresence(&discordPresence);
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
	}
	void ModuleDiscord::DiscordInit()
	{
		InitDiscord();
	}
}