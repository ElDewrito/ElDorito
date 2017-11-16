#include "DiscordRPC.h"

#include <time.h>

#include "../Blam/BlamEvents.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Patches/Core.hpp"
#include "../Patches/Events.hpp"
#include "../Patches/Network.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Pointer.hpp"
#include "../CommandMap.hpp"
#include "../Utils/String.hpp"

#include "../Utils/Logger.hpp"

static const char* APPLICATION_ID = "378984448022020112";

namespace
{
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

	void PresenceUpdate()
	{
		auto get_multiplayer_scoreboard = (Blam::MutiplayerScoreboard*(*)())(0x00550B80);
		Discord::DiscordRPC::Instance().detailString.clear();
		memset(&Discord::DiscordRPC::Instance().discordPresence, 0, sizeof(Discord::DiscordRPC::discordPresence));

		auto* session = Blam::Network::GetActiveSession();
		if (session && session->IsEstablished())
		{
			if (strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") == 0)
			{
				Discord::DiscordRPC::Instance().discordPresence.largeImageKey = "default";
				Discord::DiscordRPC::Instance().discordPresence.largeImageText = "Mainmenu";
				Discord::DiscordRPC::Instance().discordPresence.details = "In an Online Lobby";
			}
			else
			{
				Discord::DiscordRPC::Instance().discordPresence.largeImageKey = (char*)Pointer(0x22AB018)(0x1A4);
				Discord::DiscordRPC::Instance().discordPresence.largeImageText = (char*)Pointer(0x22AB018)(0x1A4);
				auto game = session->Parameters.GameVariant.Get();
				auto map = session->Parameters.MapVariant.Get();

				if (game && map)
				{
					std::stringstream ss;
					ss << Utils::String::ThinString(game->Name) << " on " << Utils::String::ThinString(map->ContentHeader.Name);
					Discord::DiscordRPC::Instance().detailString = ss.str(); //Need a variable that will stay active
					Discord::DiscordRPC::Instance().discordPresence.details = Discord::DiscordRPC::Instance().detailString.c_str();
					Discord::DiscordRPC::Instance().discordPresence.smallImageKey = Blam::GameTypeNames[game->GameType].c_str();
					Discord::DiscordRPC::Instance().discordPresence.smallImageText = Blam::GameTypeNames[game->GameType].c_str();
				}
			}
			int players = 0;
			int plyIndex = session->MembershipInfo.FindFirstPlayer();
			while (plyIndex != -1)
			{
				players++;
				plyIndex = session->MembershipInfo.FindNextPlayer(plyIndex);
			}

			Discord::DiscordRPC::Instance().discordPresence.partySize = players;
			Discord::DiscordRPC::Instance().discordPresence.partyMax = session->MembershipInfo.SessionMaxPlayers;
			Discord::DiscordRPC::Instance().discordPresence.state = Modules::ModuleServer::Instance().VarServerNameClient->ValueString.c_str();
		}
		else
		{
			Discord::DiscordRPC::Instance().discordPresence.state = "At the Mainmenu";
			Discord::DiscordRPC::Instance().discordPresence.largeImageKey = "default";
			Discord::DiscordRPC::Instance().discordPresence.largeImageText = (char*)Pointer(0x22AB018)(0x1A4);
			Discord::DiscordRPC::Instance().discordPresence.details = "Alone";
			Discord::DiscordRPC::Instance().discordPresence.partySize = 1;
			Discord::DiscordRPC::Instance().discordPresence.partyMax = 1;
		}

		Discord_UpdatePresence(&Discord::DiscordRPC::Instance().discordPresence);
	}

	void handleDiscordReady()
	{
		PresenceUpdate();
	}

	void MapLoaded(const char* mappath)
	{
		PresenceUpdate();
	}

	void Event(Blam::DatumIndex player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)
	{
		PresenceUpdate();
	}

	void Lifecycle(Blam::Network::LifeCycleState newState)
	{
		PresenceUpdate();
	}
}

namespace Discord
{

	void DiscordRPC::UpdatePresence()
	{
		PresenceUpdate();
	}

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
		Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

		Patches::Core::OnShutdown(Discord_Shutdown);
		Patches::Core::OnGameStart(PresenceUpdate);
		Patches::Core::OnMapLoaded(MapLoaded);
		Patches::Events::OnEvent(Event);
		Patches::Network::OnLifeCycleStateChanged(Lifecycle);
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