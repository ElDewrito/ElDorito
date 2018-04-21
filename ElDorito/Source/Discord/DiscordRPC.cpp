#include "DiscordRPC.h"

#include <time.h>

#include "../Blam/BlamNetwork.hpp"
#include "../Patches/Network.hpp"
#include "../Patches/Core.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModuleGame.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/HttpRequest.hpp"
#include "../Pointer.hpp"
#include "../Utils/String.hpp"
#include "../Utils/Cryptography.hpp"
#include "../Utils/VersionInfo.hpp"
#include <discord_register.h>

#include "../Utils/Logger.hpp"

static const char* APPLICATION_ID = "378984448022020112";

#define SITE_NUMBER 4
//address paired with a bool defining if a trailing /n must be removed
static const std::pair<std::wstring, bool> ipGetter[SITE_NUMBER] =
{
	std::make_pair(L"http://checkip.amazonaws.com/", true),
	std::make_pair(L"http://ipecho.net/plain", false),
	std::make_pair(L"http://myexternalip.com/raw", true),
	std::make_pair(L"http://diagnostic.opendns.com/myip", false)
};

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
		if (Modules::ModuleGame::Instance().VarDiscordAutoAccept->ValueInt == 1)
		{
			Discord_Respond(joinRequest->userId, DISCORD_REPLY_YES);
			return;
		}

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
				Discord::DiscordRPC::Instance().mapString = std::string((char*)Pointer(0x22AB018)(0x1A4));
				Discord::DiscordRPC::Instance().mapString[0] = toupper(Discord::DiscordRPC::Instance().mapString[0]);
				Discord::DiscordRPC::Instance().discordPresence.largeImageText = Discord::DiscordRPC::Instance().mapString.c_str();
				Discord::DiscordRPC::Instance().discordPresence.largeImageKey = (char*)Pointer(0x22AB018)(0x1A4);
				
				if (game && map)
				{
					std::stringstream ss;
					ss << "On " << Utils::String::ThinString(map->ContentHeader.Name);
					Discord::DiscordRPC::Instance().detailString = ss.str(); //Need a variable that will stay active
					Discord::DiscordRPC::Instance().discordPresence.details = Discord::DiscordRPC::Instance().detailString.c_str();

					Discord::DiscordRPC::Instance().gameTypeString = Blam::GameTypeNames[game->GameType];
					Discord::DiscordRPC::Instance().gameTypeString[0] = toupper(Discord::DiscordRPC::Instance().gameTypeString[0]);
					Discord::DiscordRPC::Instance().discordPresence.smallImageText = Discord::DiscordRPC::Instance().gameTypeString.c_str();
					Discord::DiscordRPC::Instance().discordPresence.smallImageKey = Blam::GameTypeNames[game->GameType].c_str();
				}
			}
			int players = 0;
			int plyIndex = session->MembershipInfo.FindFirstPlayer();
			while (plyIndex != -1)
			{
				players++;
				plyIndex = session->MembershipInfo.FindNextPlayer(plyIndex);
			}

			Discord::DiscordRPC::Instance().discordPresence.partyId = Modules::ModuleServer::Instance().VarServerNameClient->ValueString.c_str();
			Discord::DiscordRPC::Instance().discordPresence.matchSecret = Discord::DiscordRPC::Instance().matchSecret.c_str();
			Discord::DiscordRPC::Instance().discordPresence.partySize = players;
			Discord::DiscordRPC::Instance().discordPresence.partyMax = session->MembershipInfo.SessionMaxPlayers;
			if (game)
			{
				Discord::DiscordRPC::Instance().gameTypeString = Blam::GameTypeNames[game->GameType];
				Discord::DiscordRPC::Instance().gameTypeString[0] = toupper(Discord::DiscordRPC::Instance().gameTypeString[0]);
				Discord::DiscordRPC::Instance().discordPresence.state = Discord::DiscordRPC::Instance().gameTypeString.c_str();
			}
		}
		else
		{
			Discord::DiscordRPC::Instance().discordPresence.state = "At the Mainmenu";
			Discord::DiscordRPC::Instance().discordPresence.largeImageKey = "default";
			Discord::DiscordRPC::Instance().discordPresence.largeImageText = "Mainmenu";
			Discord::DiscordRPC::Instance().discordPresence.details = "Alone";
			Discord::DiscordRPC::Instance().discordPresence.partySize = 1;
			Discord::DiscordRPC::Instance().discordPresence.partyMax = 1;
			Discord::DiscordRPC::Instance().SetJoinString("");
		}
		Discord::DiscordRPC::Instance().joinMtx.lock();
		Discord::DiscordRPC::Instance().discordPresence.joinSecret = Discord::DiscordRPC::Instance().joinString.c_str();
		Discord::DiscordRPC::Instance().joinMtx.unlock();
		Discord_UpdatePresence(&Discord::DiscordRPC::Instance().discordPresence);
	}

	void handleDiscordReady()
	{
		Discord::DiscordRPC::Instance().Update();
	}

	DWORD WINAPI DiscordRetrieveExternalIP_Thread(LPVOID lpParam)
	{
		HttpRequest req(L"ElDewrito/" + Utils::String::WidenString(Utils::Version::GetVersionString()), L"", L"");
		
		for (int i = 0; i < SITE_NUMBER; i++)
		{
			if (!req.SendRequest(ipGetter[i].first, L"GET", L"", L"", L"", NULL, 0))
			{
				Discord::DiscordRPC::Instance().SetJoinString("");
				continue;
			}

			std::wstring expected = L"HTTP/1.1 200 OK";
			if (req.responseHeader.length() < expected.length())
			{
				Discord::DiscordRPC::Instance().SetJoinString("");
				continue;
			}

			auto respHdr = req.responseHeader.substr(0, expected.length());
			if (respHdr.compare(expected))
			{
				Discord::DiscordRPC::Instance().SetJoinString("");
				continue;
			}
			std::string extIp;
			if (ipGetter[i].second) //remove trailing \n
				extIp = std::string(req.responseBody.begin(), req.responseBody.end() - 1);
			else
				extIp = std::string(req.responseBody.begin(), req.responseBody.end());
			Discord::DiscordRPC::Instance().SetJoinString(extIp + ":" + Modules::ModuleServer::Instance().VarServerPort->ValueString + " " + Modules::ModuleServer::Instance().VarServerPassword->ValueString);
			return true;
		}

		Discord::DiscordRPC::Instance().SetJoinString("");
		return true;
	}

	void OnShutdown()
	{
		Discord::DiscordRPC::Instance().Shutdown();
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
			if (Modules::ModuleGame::Instance().VarDiscordEnable->ValueInt == 0)
			{
				Discord_ClearPresence();
				return;
			}

			lastUpdate = curTime;
			UpdateRichPresence();
		}
	}

	void DiscordRPC::UpdatePresence(int networkMode)
	{
		if (Modules::ModuleGame::Instance().VarDiscordEnable->ValueInt == 0)
		{
			Discord_ClearPresence();
			return;
		}

		auto session = Blam::Network::GetActiveSession();
		if (networkMode == 3 && session->IsHost())
		{
			auto thread = CreateThread(NULL, 0, DiscordRetrieveExternalIP_Thread, NULL, 0, NULL);
		}
		else
		{
			joinString = "";
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

		Patches::Core::OnShutdown(OnShutdown);
	}

	void DiscordRPC::ReplyToJoinRequest(const char* userId, int reply)
	{
		Discord_Respond(userId, reply);
	}

	void DiscordRPC::Shutdown()
	{
		Discord_Shutdown();
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