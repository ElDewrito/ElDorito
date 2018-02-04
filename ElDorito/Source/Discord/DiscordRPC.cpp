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
#include "../Modules/ModuleGame.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/HttpRequest.hpp"
#include "../Pointer.hpp"
#include "../CommandMap.hpp"
#include "../Utils/String.hpp"
#include <discord_register.h>

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
		writer.Key("discriminator");
		writer.String(joinRequest->discriminator);
		writer.EndObject();

		Web::Ui::ScreenLayer::Notify("discord-joinrequest", jsonBuffer.GetString(), true);
	}

	void UpdateRichPresence()
	{
		auto get_multiplayer_scoreboard = (Blam::MutiplayerScoreboard*(*)())(0x00550B80);
		Discord::DiscordRPC::Instance().detailString.clear();
		memset(&Discord::DiscordRPC::Instance().discordPresence, 0, sizeof(Discord::DiscordRPC::discordPresence));

		auto* session = Blam::Network::GetActiveSession();
		if (session && session->IsEstablished())
		{
			auto game = session->Parameters.GameVariant.Get();
			auto map = session->Parameters.MapVariant.Get();

			if (strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") == 0)
			{
				Discord::DiscordRPC::Instance().discordPresence.largeImageKey = "default";
				Discord::DiscordRPC::Instance().discordPresence.largeImageText = "Mainmenu";
				if (Blam::Network::GetNetworkMode() == 3)
					Discord::DiscordRPC::Instance().discordPresence.details = "In an Online Lobby";
				else
					Discord::DiscordRPC::Instance().discordPresence.details = "In an Offline Lobby";
			}
			else
			{
				Discord::DiscordRPC::Instance().discordPresence.largeImageKey = (char*)Pointer(0x22AB018)(0x1A4);
				Discord::DiscordRPC::Instance().discordPresence.largeImageText = (char*)Pointer(0x22AB018)(0x1A4);
				
				if (game && map)
				{
					std::stringstream ss;
					ss << "On " << Utils::String::ThinString(map->ContentHeader.Name);
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

			std::stringstream uidStream;
			uidStream << session->MembershipInfo.GetLocalPlayerSession().Properties.Uid;
			Discord::DiscordRPC::Instance().partyId = uidStream.str();
			Discord::DiscordRPC::Instance().discordPresence.partyId = Modules::ModuleServer::Instance().VarServerNameClient->ValueString.c_str();
			Discord::DiscordRPC::Instance().discordPresence.matchSecret = Discord::DiscordRPC::Instance().matchSecret.c_str();
			Discord::DiscordRPC::Instance().discordPresence.partySize = players;
			Discord::DiscordRPC::Instance().discordPresence.partyMax = session->MembershipInfo.SessionMaxPlayers;
			if (game)
				Discord::DiscordRPC::Instance().discordPresence.state = Blam::GameTypeNames[game->GameType].c_str();
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
		Discord::DiscordRPC::Instance().joinMtx.lock();
		Discord::DiscordRPC::Instance().discordPresence.joinSecret = Discord::DiscordRPC::Instance().joinString.c_str();
		Discord::DiscordRPC::Instance().joinMtx.unlock();
		Discord_UpdatePresence(&Discord::DiscordRPC::Instance().discordPresence);
	}

	void handleDiscordReady()
	{
		UpdateRichPresence();
	}

	DWORD WINAPI DiscordRetrieveExternalIP_Thread(LPVOID lpParam)
	{
		HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");

		if (!req.SendRequest(L"http://myexternalip.com/raw", L"GET", L"", L"", L"", NULL, 0))
		{
			Discord::DiscordRPC::Instance().SetJoinString("");
			return true;
		}

		std::wstring expected = L"HTTP/1.1 200 OK";
		if (req.responseHeader.length() < expected.length())
		{
			Discord::DiscordRPC::Instance().SetJoinString("");
			return true;
		}

		auto respHdr = req.responseHeader.substr(0, expected.length());
		if (respHdr.compare(expected))
		{
			Discord::DiscordRPC::Instance().SetJoinString("");
			return true;
		}

		std::string extIp = std::string(req.responseBody.begin(), req.responseBody.end()).substr(0, req.responseBody.size() - 1); //remove trailing \n
		Discord::DiscordRPC::Instance().SetJoinString(extIp + ":" + Modules::ModuleServer::Instance().VarServerPort->ValueString + " " + Modules::ModuleServer::Instance().VarServerPassword->ValueString);
		return true;
	}
}

namespace Discord
{
	void DiscordRPC::SetJoinString(std::string join)
	{
		joinMtx.lock();
		joinString.assign(join);
		joinMtx.unlock();
	}

	void DiscordRPC::UpdatePresence()
	{
		time_t curTime;
		time(&curTime);
		if (curTime - lastUpdate > discordUpdateTime)
		{
			lastUpdate = curTime;
			UpdateRichPresence();
		}
	}

	void DiscordRPC::UpdatePresence(int networkMode)
	{
		auto session = Blam::Network::GetActiveSession();
		if (networkMode == 3 && session->IsHost())
		{
			auto thread = CreateThread(NULL, 0, DiscordRetrieveExternalIP_Thread, NULL, 0, NULL);
		}
		else
		{
			Discord::DiscordRPC::Instance().joinString = "";
		}
		UpdatePresence();
	}

	DiscordRPC::DiscordRPC()
	{
		joinString = "";
		detailString = "";
		Utils::Cryptography::RandomPassword(32, matchSecret);

		HMODULE hModule = GetModuleHandleW(NULL);
		WCHAR appPath[MAX_PATH];
		GetModuleFileNameW(hModule, appPath, MAX_PATH);
		Discord_Register("ElDewrito", Utils::String::ThinString(appPath).c_str());

		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = handleDiscordReady;
		handlers.disconnected = handleDiscordDisconnected;
		handlers.errored = handleDiscordError;
		handlers.joinGame = handleDiscordJoin;
		handlers.spectateGame = handleDiscordSpectate;
		handlers.joinRequest = handleDiscordJoinRequest;
		Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
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
		UpdatePresence();
		Discord_RunCallbacks();
	}
}