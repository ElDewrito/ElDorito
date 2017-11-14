#include "DiscordRPC.h"

#include <string.h>
#include <time.h>

#include "../Blam/BlamNetwork.hpp"
#include "../Patches/Core.hpp"
#include "../Patches/Network.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Pointer.hpp"
#include "../CommandMap.hpp"
#include "../Utils/String.hpp"

#include "../Utils/Logger.hpp"

namespace
{
	void handleDiscordReady()
	{
		memset(&Discord::DiscordRPC::Instance().discordPresence, 0, sizeof(Discord::DiscordRPC::Instance().discordPresence));
		Discord::DiscordRPC::Instance().discordPresence.state = "ElDewrito";
		Discord::DiscordRPC::Instance().discordPresence.startTimestamp = time(0);
		Discord::DiscordRPC::Instance().discordPresence.endTimestamp = time(0);
		Discord::DiscordRPC::Instance().discordPresence.largeImageKey = "unknown";
		Discord::DiscordRPC::Instance().discordPresence.smallImageKey = "unk-small";
		Discord::DiscordRPC::Instance().discordPresence.partyId = "party1234";
		Discord::DiscordRPC::Instance().discordPresence.joinSecret = ""; //TODO: cache/generate this after joining or hosting a session
		Discord::DiscordRPC::Instance().discordPresence.spectateSecret = "";
		Discord::DiscordRPC::Instance().discordPresence.matchSecret = "";
		Discord::DiscordRPC::Instance().discordPresence.partySize = 1;
		Discord::DiscordRPC::Instance().discordPresence.partyMax = 16;
		Discord::DiscordRPC::Instance().discordPresence.instance = 1;
		Discord_UpdatePresence(&Discord::DiscordRPC::Instance().discordPresence);
	}

	void LifeCycle(Blam::Network::LifeCycleState state)
	{
		handleDiscordReady();
	}

	void handleDiscordDisconnected(int errorCode, const char *message)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Discord-RPC: %s", std::string(message));
	}

	void handleDiscordError(int errorCode, const char *message)
	{
		Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Discord-RPC: %s", std::string(message));
	}

	void handleDiscordJoin(const char *joinSecret)
	{
		Modules::CommandMap::Instance().ExecuteCommand("connect " + std::string(joinSecret));
	}

	void handleDiscordSpectate(const char *spectateSecret)
	{
		//rip
	}

	void handleDiscordJoinRequest(const DiscordJoinRequest *joinRequest)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(jsonBuffer);
		
		writer.StartObject();
		writer.Key("userId");
		writer.String(joinRequest->userId);
		writer.Key("username");
		writer.String(joinRequest->username);
		writer.Key("avatar");
		writer.String(joinRequest->avatar);
		writer.EndObject();

		Web::Ui::ScreenLayer::Notify("discord-joinrequest", jsonBuffer.GetString(), true);
	}
}

namespace Discord
{
	DiscordRPC::DiscordRPC()
	{
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = handleDiscordReady;
		handlers.disconnected = handleDiscordDisconnected;
		handlers.errored = handleDiscordError;
		handlers.joinGame = handleDiscordJoin;
		handlers.spectateGame = handleDiscordSpectate;
		handlers.joinRequest = handleDiscordJoinRequest;
		Discord_Initialize("378984448022020112", &handlers, 1, NULL);

		Patches::Core::OnShutdown(Discord_Shutdown);
		Patches::Network::OnLifeCycleStateChanged(LifeCycle);
	}

	void DiscordRPC::UpdatePresence()
	{
		Discord_UpdatePresence(&Discord::DiscordRPC::Instance().discordPresence);
	}

	void DiscordRPC::ReplyToJoinRequest(const char* userId, int reply)
	{
		Discord_Respond(userId, reply);
	}

	void DiscordRPC::Update()
	{
#ifdef DISCORD_DISABLE_IO_THREAD
		Discord_UpdateConnection();
#endif
		Discord_RunCallbacks();
	}
}