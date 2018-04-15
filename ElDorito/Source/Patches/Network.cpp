#include "Network.hpp"

#define _WINSOCK_DEPRECATED_NO_WARNINGS // TODO: Remove this and fix the deprecated calls
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>

#include "PlayerPropertiesExtension.hpp"
#include "../Patch.hpp"
#include "../Utils/VersionInfo.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../ElDorito.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../Blam/BlamNetwork.hpp"
#include "../Server/BanList.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModuleUPnP.hpp"
#include "../Server/Voting.hpp"
#include "../Server/Stats.hpp"
#include "../Utils/Logger.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Server/Signaling.hpp"

namespace
{
	char* Network_GetIPStringFromInAddr(void* inaddr);
	char Network_XnAddrToInAddrHook(void* pxna, void* pxnkid, void* in_addr);
	char Network_InAddrToXnAddrHook(void* ina, void * pxna, void * pxnkid);
	char __cdecl Network_transport_secure_key_createHook(void* xnetInfo);
	DWORD __cdecl Network_managed_session_create_session_internalHook(int a1, int a2);
	bool __fastcall Network_leader_request_boot_machineHook(void* thisPtr, void* unused, Blam::Network::PeerInfo* peer, int reason);
	bool __fastcall Network_session_handle_join_requestHook(Blam::Network::Session *thisPtr, void *unused, const Blam::Network::NetworkAddress &address, void *request);
	int Network_GetMaxPlayersHook();
	bool __fastcall Network_GetEndpointHook(char *thisPtr, void *unused);

	bool __fastcall PeerRequestPlayerDesiredPropertiesUpdateHook(Blam::Network::Session *thisPtr, void *unused, uint32_t arg0, uint32_t arg4, Blam::Players::ClientPlayerProperties *properties, uint32_t argC);
	void __fastcall ApplyPlayerPropertiesExtended(Blam::Network::SessionMembership *thisPtr, void *unused, int playerIndex, uint32_t arg4, uint32_t arg8, uint8_t *data, uint32_t arg10);
	void __fastcall RegisterPlayerPropertiesPacketHook(void *thisPtr, void *unused, int packetId, const char *packetName, int arg8, int size1, int size2, void *serializeFunc, void *deserializeFunc, int arg1C, int arg20);
	void SerializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag);
	bool DeserializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag);

	char __fastcall Network_state_end_game_write_stats_enterHook(void* thisPtr, int unused, int a2, int a3, int a4);
	char __fastcall Network_state_leaving_enterHook(void* thisPtr, int unused, int a2, int a3, int a4);
	char __fastcall Network_session_join_remote_sessionHook(void* thisPtr, int unused, char a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, void *a12, int a13, int a14);
	int __fastcall Network_session_initiate_leave_protocolHook(void* thisPtr, int unused, char forceClose);
	int __fastcall Network_session_parameters_clearHook(void* thisPtr, int unused);
	int __fastcall Network_session_remove_peerHook(Blam::Network::SessionMembership *membership, void *unused, int peerIndex);
	bool __fastcall Network_session_parameter_countdown_timer_request_change_hook(void* thisPtr, void* unused, int state, int value);
	bool __fastcall c_network_session_parameter_map_variant__request_change_hook(void *thisptr, void *unused, Blam::MapVariant *mapVariant);
	char __fastcall c_network_session__handle_session_boot_hook(void *thisPtr, void *unused, int a2, int a3);
	uint64_t local_user_get_identifier_hook();
	bool network_build_local_game_status_hook(uint8_t *data);

	std::vector<Patches::Network::PongCallback> pongCallbacks;
	std::vector<Patches::Network::MapVariantRequestChangeCallback> mapVariantRequestChangeCallbacks;
	void PongReceivedHook();

	std::vector<Patches::Network::LifeCycleStateChangedCallback> lifeCycleStateChangedCallbacks;
	void LifeCycleStateChangedHook();

	void GetTextureDimensionsHook();
}

namespace Patches::Network
{
	SOCKET infoSocket;
	bool infoSocketOpen = false;
	time_t lastAnnounce = 0;
	const time_t serverContactTimeLimit = 30 + (2 * 60);

	std::string httpServerCache;
	time_t timeSinceLastPoll = 0;

	bool IsInfoSocketOpen() { return infoSocketOpen; }

	int GetNumPlayers()
	{
		void* v2;

		typedef char(__cdecl *sub_454F20Func)(void** a1);
		sub_454F20Func sub_454F20 = (sub_454F20Func)0x454F20;
		if (!sub_454F20(&v2))
			return 0;

		typedef char*(__thiscall *sub_45C250Func)(void* thisPtr);
		sub_45C250Func sub_45C250 = (sub_45C250Func)0x45C250;

		return *(DWORD*)(sub_45C250(v2) + 0x10A0);
	}

	int __stdcall networkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		time_t curTime;
		time(&curTime);
		// TODO: move this somewhere better, it seems fine here though, always announces every "serverContactTimeLimit" seconds even when window isn't focused
		if (infoSocketOpen && Modules::ModuleServer::Instance().VarServerShouldAnnounce->ValueInt)
		{
			if (curTime - lastAnnounce > serverContactTimeLimit) // re-announce every "serverContactTimeLimit" seconds
			{
				lastAnnounce = curTime;
				Modules::CommandMap::Instance().ExecuteCommand("Server.Announce");
			}
		}

		if (msg != WM_INFOSERVER)
		{
			//TODO: Move WndProc logic out of Network.cpp
			if (msg == WM_XBUTTONDOWN && !ElDorito::Instance().IsDedicated())
			{
				int mouseXButton = GET_XBUTTON_WPARAM(wParam);

				rapidjson::StringBuffer jsonBuffer;
				rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
				jsonWriter.StartObject();
				jsonWriter.Key("xbutton");
				jsonWriter.Int(mouseXButton);
				jsonWriter.EndObject();

				Web::Ui::ScreenLayer::Notify("mouse-xbutton-event", jsonBuffer.GetString(), true);
			}

			typedef int(__stdcall *Game_WndProcFunc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			Game_WndProcFunc Game_WndProc = (Game_WndProcFunc)0x42E6A0;
			return Game_WndProc(hWnd, msg, wParam, lParam);
		}

		if (WSAGETSELECTERROR(lParam))
		{
			closesocket((SOCKET)wParam);
			return 0;
		}

		SOCKET clientSocket;
		int inDataLength;
		char inDataBuffer[4096];

		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:
			clientSocket = accept(wParam, NULL, NULL);
			WSAAsyncSelect(clientSocket, hWnd, msg, FD_READ | FD_WRITE | FD_CLOSE);
			break;
		case FD_READ:
			ZeroMemory(inDataBuffer, sizeof(inDataBuffer));
			inDataLength = recv((SOCKET)wParam, (char*)inDataBuffer, sizeof(inDataBuffer) / sizeof(inDataBuffer[0]), 0);

			if (msg == WM_INFOSERVER)
			{
				std::string replyData;
				if (!Modules::ModuleServer::Instance().VarServerPassword->ValueString.empty() || curTime - timeSinceLastPoll > Modules::ModuleServer::Instance().VarHttpServerCacheTime->ValueInt)
				{
					timeSinceLastPoll = curTime;

					std::string mapName((char*)Pointer(0x22AB018)(0x1A4));
					std::wstring mapVariantName((wchar_t*)Pointer(0x1863ACA));
					std::wstring variantName((wchar_t*)Pointer(0x23DAF4C));
					std::string xnkid;
					std::string xnaddr;
					std::string gameVersion((char*)Pointer(0x199C0F0));
					std::string status = "InGame";
					Utils::String::BytesToHexString((char*)Pointer(0x2247b80), 0x10, xnkid);
					Utils::String::BytesToHexString((char*)Pointer(0x2247b90), 0x10, xnaddr);

					Pointer &gameModePtr = ElDorito::GetMainTls(GameGlobals::GameInfo::TLSOffset)[0](GameGlobals::GameInfo::GameMode);
					uint32_t gameMode = gameModePtr.Read<uint32_t>();
					int32_t variantType = Pointer(0x023DAF18).Read<int32_t>();
					if (gameMode == 3)
					{
						if (mapName == "mainmenu")
						{
							status = "InLobby";
							// on mainmenu so we'll have to read other data
							mapName = std::string((char*)Pointer(0x19A5E49));
							variantName = L"none";
							variantType = 0;
						}
						else // TODO: find how to get the variant name/type while it's on the loading screen
							status = "Loading";
					}

					rapidjson::StringBuffer s;
					rapidjson::Writer<rapidjson::StringBuffer> writer(s);
					writer.StartObject();
					writer.Key("name");
					writer.String(Modules::ModuleServer::Instance().VarServerName->ValueString.c_str());
					writer.Key("port");
					writer.Int(Pointer(0x1860454).Read<uint32_t>());
					writer.Key("hostPlayer");
					writer.String(Modules::ModulePlayer::Instance().VarPlayerName->ValueString.c_str());
					writer.Key("sprintEnabled");
					writer.String(Modules::ModuleServer::Instance().VarServerSprintEnabled->ValueString.c_str());
					writer.Key("sprintUnlimitedEnabled");
					writer.String(Modules::ModuleServer::Instance().VarServerSprintUnlimited->ValueString.c_str());
					writer.Key("dualWielding");
					writer.String(Modules::ModuleServer::Instance().VarServerDualWieldEnabled->ValueString.c_str());
					writer.Key("assassinationEnabled");
					writer.String(Modules::ModuleServer::Instance().VarServerAssassinationEnabled->ValueString.c_str());
					writer.Key("votingEnabled");
					writer.Bool(Modules::ModuleServer::Instance().VarServerVotingEnabled->ValueInt == 1 || Modules::ModuleServer::Instance().VarVetoSystemEnabled->ValueInt == 1);

					auto session = Blam::Network::GetActiveSession();
					if (session && session->IsEstablished()) {
						writer.Key("teams");
						writer.Bool(session->HasTeams());
					}
					writer.Key("map");
					writer.String(Utils::String::ThinString(mapVariantName).c_str());
					writer.Key("mapFile");
					writer.String(mapName.c_str());
					writer.Key("variant");
					writer.String(Utils::String::ThinString(variantName).c_str());
					if (variantType >= 0 && variantType < Blam::GameTypeCount)
					{
						writer.Key("variantType");
						writer.String(Blam::GameTypeNames[variantType].c_str());
					}
					writer.Key("status");
					writer.String(status.c_str());
					writer.Key("numPlayers");
					writer.Int(GetNumPlayers());

					std::ifstream file("fmmRequired.dat");
					std::string temp;

					writer.Key("mods");
					writer.StartArray();
					while (std::getline(file, temp))
						writer.String(temp.c_str());
					writer.EndArray();

					// TODO: find how to get actual max players from the game, since our variable might be wrong
					writer.Key("maxPlayers");
					writer.Int(Modules::ModuleServer::Instance().VarServerMaxPlayers->ValueInt);

					bool authenticated = true;
					if (!Modules::ModuleServer::Instance().VarServerPassword->ValueString.empty())
					{
						std::string authString = "dorito:" + Modules::ModuleServer::Instance().VarServerPassword->ValueString;
						authString = "Authorization: Basic " + Utils::String::Base64Encode((const unsigned char*)authString.c_str(), authString.length()) + "\r\n";
						authenticated = std::string(inDataBuffer).find(authString) != std::string::npos;
					}

					if (authenticated)
					{
						writer.Key("xnkid");
						writer.String(xnkid.c_str());
						writer.Key("xnaddr");
						writer.String(xnaddr.c_str());
						if (session->HasTeams())
						{
							writer.Key("teamScores");
							writer.StartArray();
							uint32_t* scores = &Pointer(0x01879DA8).Read<uint32_t>();
							for (int t = 0; t < 8; t++)
							{
								writer.Int(scores[t]);
							}
							writer.EndArray();
						}

						writer.Key("players");

						writer.StartArray();
						uint32_t playerInfoBase = 0x2162E08;
						uint32_t playerStatusBase = 0x2161808;


						int peerIdx = session->MembershipInfo.FindFirstPeer();
						while (peerIdx != -1)
						{
							int playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
							if (playerIdx != -1)
							{
								auto playerStats = Blam::Players::GetStats(playerIdx);
								auto* player = &session->MembershipInfo.PlayerSessions[playerIdx];
								std::string name = Utils::String::ThinString(player->Properties.DisplayName);
								uint16_t team = Pointer(playerInfoBase + (5696 * playerIdx) + 32).Read<uint16_t>();

								uint8_t alive = Pointer(playerStatusBase + (176 * playerIdx)).Read<uint8_t>();

								writer.StartObject();
								writer.Key("name");
								writer.String(name.c_str());
								writer.Key("serviceTag");
								writer.String(Utils::String::ThinString(player->Properties.ServiceTag).c_str());
								writer.Key("team");
								writer.Int(team);
								char uid[17];
								Blam::Players::FormatUid(uid, player->Properties.Uid);
								writer.Key("uid");
								writer.String(uid);
								std::stringstream color;
								color << "#" << std::setw(6) << std::setfill('0') << std::hex << player->Properties.Customization.Colors[Blam::Players::ColorIndices::Primary];
								writer.Key("primaryColor");
								writer.String(color.str().c_str());
								writer.Key("isAlive");
								writer.Bool(alive == 1);
								writer.Key("score");
								writer.Int(playerStats.Score);
								writer.Key("kills");
								writer.Int(playerStats.Kills);
								writer.Key("assists");
								writer.Int(playerStats.Assists);
								writer.Key("deaths");
								writer.Int(playerStats.Deaths);
								writer.Key("betrayals");
								writer.Int(playerStats.Betrayals);
								writer.Key("timeSpentAlive");
								writer.Int(playerStats.TimeSpentAlive);
								writer.Key("suicides");
								writer.Int(playerStats.Suicides);
								writer.Key("bestStreak");
								writer.Int(playerStats.BestStreak);
								writer.EndObject();
							}
							peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
						}
						writer.EndArray();
					}
					else
					{
						writer.Key("passworded");
						writer.Bool(true);
					}
					writer.Key("isDedicated");
					writer.Bool(ElDorito::Instance().IsDedicated());
					writer.Key("gameVersion");
					writer.String(gameVersion.c_str());
					writer.Key("eldewritoVersion");
					writer.String(Utils::Version::GetVersionString().c_str());
					writer.EndObject();
					httpServerCache = s.GetString();
				}
				replyData = httpServerCache;
				std::string reply = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nServer: ElDewrito/" + Utils::Version::GetVersionString() + "\r\nContent-Length: " + std::to_string(replyData.length()) + "\r\nConnection: close\r\n\r\n" + replyData;
				send((SOCKET)wParam, reply.c_str(), reply.length(), 0);
			}

			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			break;
		}
		return 0;
	}

	void ApplyAll()
	{
		// Fix network debug strings having (null) instead of an IP address
		Hook(0x3F6F0, &Network_GetIPStringFromInAddr).Apply();

		// Fix for XnAddrToInAddr to try checking syslink-menu data for XnAddr->InAddr mapping before consulting XNet
		Hook(0x30B6C, &Network_XnAddrToInAddrHook, HookFlags::IsCall).Apply();
		Hook(0x30F51, &Network_InAddrToXnAddrHook, HookFlags::IsCall).Apply();

		// Hook call to Network_managed_session_create_session_internal so we can detect when an online game is started
		Hook(0x82AAC, &Network_managed_session_create_session_internalHook, HookFlags::IsCall).Apply();

		// Patch version subs to return version of this DLL, to make people with older DLLs incompatible
		uint32_t verNum = Utils::Version::GetVersionInt();
		Pointer::Base(0x101421).Write<uint32_t>(verNum);
		Pointer::Base(0x10143A).Write<uint32_t>(verNum);

		// Player-properties packet hooks
		Hook(0x5DD20, PeerRequestPlayerDesiredPropertiesUpdateHook).Apply();
		Hook(0xDAF4F, ApplyPlayerPropertiesExtended, HookFlags::IsCall).Apply();
		Hook(0xDFF7E, RegisterPlayerPropertiesPacketHook, HookFlags::IsCall).Apply();
		Hook(0xDFD53, SerializePlayerPropertiesHook, HookFlags::IsCall).Apply();
		Hook(0xDE178, DeserializePlayerPropertiesHook, HookFlags::IsCall).Apply();

		// Set the games wndproc to our one, so we can receive async network messages
		Pointer::Base(0x2EB63).Write<uint32_t>((uint32_t)&networkWndProc);

		// Hook leader_request_boot_machine so we can do some extra things if the boot succeeded
		Hook(0x37E17, Network_leader_request_boot_machineHook, HookFlags::IsCall).Apply();

		// Pong hook
		Hook(0x9D9DB, PongReceivedHook).Apply();

		// Lifecycle state change hook
		Hook(0x8E527, LifeCycleStateChangedHook, HookFlags::IsCall).Apply();
		Hook(0x8E10F, LifeCycleStateChangedHook, HookFlags::IsCall).Apply();

		// Hook the join request handler to check the user's IP address against the ban list
		Hook(0x9D0F7, Network_session_handle_join_requestHook, HookFlags::IsCall).Apply();

		// Hook c_life_cycle_state_handler_end_game_write_stats's vftable ::entry method
		DWORD temp;
		DWORD temp2;
		auto writeAddr = Pointer(0x16183A0);
		if (!VirtualProtect(writeAddr, 4, PAGE_READWRITE, &temp))
		{
			std::stringstream ss;
			ss << "Failed to set protection on memory address " << std::hex << (void*)writeAddr;
			OutputDebugString(ss.str().c_str());
		}
		else
		{
			writeAddr.Write<uint32_t>((uint32_t)&Network_state_end_game_write_stats_enterHook);
			VirtualProtect(writeAddr, 4, temp, &temp2);
		}

		// Hook c_life_cycle_state_handler_leaving's vftable ::entry method
		writeAddr = Pointer(0x16183BC);
		if (!VirtualProtect(writeAddr, 4, PAGE_READWRITE, &temp))
		{
			std::stringstream ss;
			ss << "Failed to set protection on memory address " << std::hex << (void*)writeAddr;
			OutputDebugString(ss.str().c_str());
		}
		else
		{
			writeAddr.Write<uint32_t>((uint32_t)&Network_state_leaving_enterHook);
			VirtualProtect(writeAddr, 4, temp, &temp2);
		}

		// Set the max player count to Server.MaxPlayers when hosting a lobby
		Hook(0x67FA0D, Network_GetMaxPlayersHook, HookFlags::IsCall).Apply();

		Hook(0x3BAFB, Network_GetEndpointHook, HookFlags::IsCall).Apply();

		Hook(0x7F5B9, Network_session_join_remote_sessionHook, HookFlags::IsCall).Apply();

		// Hook Network_session_initiate_leave_protocol in Network_session_idle_peer_joining's error states
		// "peer join timed out waiting for secure connection to become established"
		Hook(0x9BFCA, Network_session_initiate_leave_protocolHook, HookFlags::IsCall).Apply();
		// "peer join timed out waiting for initial updates"
		Hook(0x9C0BE, Network_session_initiate_leave_protocolHook, HookFlags::IsCall).Apply();

		// "received initial update, clearing"
		Hook(0x899AF, Network_session_parameters_clearHook, HookFlags::IsCall).Apply();
		Hook(0x683D45, Network_session_parameter_countdown_timer_request_change_hook, HookFlags::IsCall).Apply();

		//Hook(0x4F16F, Network_session_remove_peerHook, HookFlags::IsCall).Apply(); //Client on connect calls own peer index of server
		Hook(0x4FF3E, Network_session_remove_peerHook, HookFlags::IsCall).Apply(); //server

		Hook(0x00039AE4, c_network_session_parameter_map_variant__request_change_hook, HookFlags::IsCall).Apply();

		Hook(0x9DA1A, c_network_session__handle_session_boot_hook, HookFlags::IsCall).Apply();

		Hook(0x67E01F, local_user_get_identifier_hook, HookFlags::IsCall).Apply();
		// fix network broadcast search sending back the wrong address due to the above patch (fixes local game browser)
		Hook(0x9C389, network_build_local_game_status_hook, HookFlags::IsCall).Apply();

		// skip over end_game_write_stats. stops host from getting booted due to people with bad connections
		Patch(0x92F7B, { 0xB8, 0x01, 0x00, 0x00, 0x00 }).Apply();
		Patch(0x92E3B, { 0x90, 0x90 }).Apply();
	}


	void ForceDedicated()
	{

		//pit
		Hook(0x2D0EAC, reinterpret_cast<void*>(0x6D0EBC)).Apply();

		// Put the game into dedicated server mode
		Patch(0x2E600, { 0xB0, 0x01 }).Apply();

		// Force syslink to still work
		Patch(0x12D62E, { 0xEB }).Apply();
		Patch(0x12D67A, { 0xEB }).Apply();
		Patch(0x5A8BB, { 0xEB }).Apply();

		// Crash fixes
		Patch(0xC9C5E0, { 0xC2, 0x08, 0x00 }).Apply();

		//Allows for switching of teams
		Patch(0x378C0, { 0xB0, 0x00, 0x90, 0x90, 0x90 }).Apply();

		// Fixes dedicated host crash caused accessing uninitialized player mapping globals by forcing a player datum index of 0 to always be used instead
		Patch(0x62E636, { 0x33, 0xFF }).Apply();
		// Prevents dedicated hosts from crashing due to invalid texture datum lookup
		Hook(0x66E982, GetTextureDimensionsHook).Apply();

		// force windowed (temporarily for now) with null d3d reference device
		Patch(0x620380, { 0x6A, 0x01, 0x6A, 0x00, 0x90, 0x90 }).Apply();
		// Fixes the game being stuck in some d3d-related while loop
		Patch(0x622290, { 0xC3 }).Apply();

		*reinterpret_cast<uint8_t*>(0x1917CED) = 0;	// fix dedi crash @ 0xA22325 (pairs with patch above)

													// Fixes an exception that happens with null d3d
		Patch(0x675E30, { 0xC3 }).Apply();

		*(uint8_t*)0x0244F970 = 1; // g_IsDedicatedServer
		*(uint8_t*)0x0244F971 = 1; // g_SoundDisabled

		// dedicated server server optimization
		Patch(0x1075A0, { 0xC3 }).Apply(); // main_render_thread_proc
		Patch(0x24E190, { 0xC3 }).Apply(); // disable sound halo system/fmod (prevents output thread from being created up)
		Patch(0x112690, { 0xC3 }).Apply(); // xinput, mouse, keyboard
		Patch(0x20D880, { 0xC3 }).Apply(); // input bindings handler
		Patch(0x215BA0, { 0xC3 }).Apply(); // controller feedback
		Patch(0x726710, { 0xC3 }).Apply(); // saber
		Patch(0x2173A0, { 0xC3 }).Apply(); // bink
		Patch(0x68AAE0, { 0xC3 }).Apply(); // chud state
		Patch(0x1D4990, { 0xC3 }).Apply(); // local input    
		Patch(0x219770, { 0xC3 }).Apply(); // fileshare ui
		Patch(0x219C00, { 0xC3 }).Apply(); // spartan rank/milestone ui
		Patch(0x151780, { 0xC3 }).Apply(); // betrayal boot ui
		Patch::NopFill(Pointer::Base(0x00684F3F), 5); // ui input
		Patch::NopFill(Pointer::Base(0x00684F44), 5); // display adapter preferences
		Patch::NopFill(Pointer::Base(0x00684F49), 5); // null_sub
		Patch::NopFill(Pointer::Base(0x00684F4E), 5); // scoreboard ui
		Patch(0x0067FE50, { 0xC3 }).Apply(); // campaign ui
		Patch(0x1926C0, { 0xC3 }).Apply(); // director update
		Patch(0x213A60, { 0xC3 }).Apply(); // observer update
		Patch(0x69EDF0, { 0xC3 }).Apply(); // fp weapon transform
		// bitmap resource allocation
		Patch(0x5DFBC0, { 0xC3 }).Apply();
		Patch(0x5DFAF0, { 0xC3 }).Apply();

	}

	bool StartInfoServer()
	{
		if (infoSocketOpen)
			return true;

		Server::Voting::StartNewVote();
		HWND hwnd = Pointer::Base(0x159C014).Read<HWND>();
		if (hwnd == 0)
			return false;

		infoSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN bindAddr;
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		unsigned long port = Modules::ModuleServer::Instance().VarServerPort->ValueInt;
		if (port == Pointer(0x1860454).Read<uint32_t>()) // make sure port isn't the same as game port
			port++;
		bindAddr.sin_port = htons((u_short)port);

		// open our listener socket
		while (bind(infoSocket, (PSOCKADDR)&bindAddr, sizeof(bindAddr)) != 0)
		{
			port++;
			if (port == Pointer(0x1860454).Read<uint32_t>()) // make sure port isn't the same as game port
				port++;
			bindAddr.sin_port = htons((u_short)port);
			if (port > (Modules::ModuleServer::Instance().VarServerPort->ValueInt + 10))
				return false; // tried 10 ports, lets give up
		}
		Modules::CommandMap::Instance().SetVariable(Modules::ModuleServer::Instance().VarServerPort, std::to_string(port), std::string());

		//Setup UPnP
		if (Modules::ModuleUPnP::Instance().VarUPnPEnabled->ValueInt)
		{
			Modules::ModuleUPnP::Instance().UPnPForwardPort(true, port, port, "ElDewrito InfoServer");
			Modules::ModuleUPnP::Instance().UPnPForwardPort(false, Pointer(0x1860454).Read<uint32_t>(), Pointer(0x1860454).Read<uint32_t>(), "ElDewrito Game");
		}

		WSAAsyncSelect(infoSocket, hwnd, WM_INFOSERVER, FD_ACCEPT | FD_CLOSE);
		listen(infoSocket, 5);
		infoSocketOpen = true;

		return true;
	}

	bool StopInfoServer()
	{
		if (!infoSocketOpen)
			return true;

		closesocket(infoSocket);
		int istrue = 1;
		setsockopt(infoSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&istrue, sizeof(int));

		Modules::CommandMap::Instance().ExecuteCommand("Server.Unannounce");

		infoSocketOpen = false;
		lastAnnounce = 0;

		return true;
	}

	uint16_t OnPong(PongCallback callback)
	{
		static uint16_t id = 0;
		pongCallbacks.push_back(callback);
		return id++;
	}

	void OnLifeCycleStateChanged(LifeCycleStateChangedCallback callback)
	{
		lifeCycleStateChangedCallbacks.push_back(callback);
	}

	void OnMapVariantRequestChange(MapVariantRequestChangeCallback callback)
	{
		mapVariantRequestChangeCallbacks.push_back(callback);
	}
}

namespace
{
	DWORD __cdecl Network_managed_session_create_session_internalHook(int a1, int a2)
	{
		DWORD isOnline = *(DWORD*)a2;
		bool isHost = (*(uint16_t *)(a2 + 284) & 1);

		typedef DWORD(__cdecl *Network_managed_session_create_session_internalFunc)(int a1, int a2);
		Network_managed_session_create_session_internalFunc Network_managed_session_create_session_internal = (Network_managed_session_create_session_internalFunc)0x481550;
		auto retval = Network_managed_session_create_session_internal(a1, a2);

		if (!isHost)
			return retval;

		if (isOnline == 1)
		{
			Patches::Network::StartInfoServer();
			Server::Signaling::StartServer();
		}
		else
		{
			Patches::Network::StopInfoServer();
			Server::Signaling::StopServer();
		}

		return retval;
	}

	char* Network_GetIPStringFromInAddr(void* inaddr)
	{
		static char ipAddrStr[64];
		memset(ipAddrStr, 0, 64);

		uint8_t ip3 = *(uint8_t*)inaddr;
		uint8_t ip2 = *((uint8_t*)inaddr + 1);
		uint8_t ip1 = *((uint8_t*)inaddr + 2);
		uint8_t ip0 = *((uint8_t*)inaddr + 3);

		uint16_t port = *(uint16_t*)((uint8_t*)inaddr + 0x10);
		uint16_t type = *(uint16_t*)((uint8_t*)inaddr + 0x12);

		sprintf_s(ipAddrStr, 64, "%hd.%hd.%hd.%hd:%hd (%hd)", ip0, ip1, ip2, ip3, port, type);

		return ipAddrStr;
	}

	char Network_XnAddrToInAddrHook(void* pxna, void* pxnkid, void* in_addr)
	{
		//printf("XnAddrToInAddrHook...");
		uint32_t maxMachines = *(uint32_t*)(0x228E6D8);
		uint8_t* syslinkDataPtr = (uint8_t*)*(uint32_t*)(0x228E6D8 + 0x4);

		for (uint32_t i = 0; i < maxMachines; i++)
		{
			uint8_t* entryPtr = syslinkDataPtr;
			syslinkDataPtr += 0x164F8;
			uint8_t entryStatus = *entryPtr;
			if (entryStatus == 0)
				continue;

			uint8_t* xnkidPtr = entryPtr + 0x9E;
			uint8_t* xnaddrPtr = entryPtr + 0xAE;
			uint8_t* ipPtr = entryPtr + 0x170;
			uint8_t* portPtr = entryPtr + 0x174;

			if (memcmp(pxna, xnaddrPtr, 0x10) == 0 && memcmp(pxnkid, xnkidPtr, 0x10) == 0)
			{
				// in_addr struct:
				// 0x0 - 0x10: IP (first 4 bytes for IPv4, 0x10 for IPv6)
				// 0x10 - 0x12: Port number
				// 0x12 - 0x14: IP length (4 for IPv4, 0x10 for IPv6)

				memset(in_addr, 0, 0x14);
				memcpy(in_addr, ipPtr, 4);
				memcpy((uint8_t*)in_addr + 0x10, portPtr, 2);

				*(uint16_t*)((uint8_t*)in_addr + 0x12) = 4;

				return 1;
			}
		}

		typedef char(*Network_XnAddrToInAddrFunc)(void* pxna, void* pxnkid, void* in_addr);
		Network_XnAddrToInAddrFunc XnAddrToInAddr = (Network_XnAddrToInAddrFunc)0x52D840;
		return XnAddrToInAddr(pxna, pxnkid, in_addr);
	}


	char Network_InAddrToXnAddrHook(void* ina, void * pxna, void * pxnkid)
	{
		uint32_t maxMachines = *(uint32_t*)(0x228E6D8);
		uint8_t* syslinkDataPtr = (uint8_t*)*(uint32_t*)(0x228E6DC);

		for (uint32_t i = 0; i < maxMachines; i++)
		{
			uint8_t* entryPtr = syslinkDataPtr;
			syslinkDataPtr += 0x164F8;
			uint8_t entryStatus = *entryPtr;
			if (entryStatus == 0)
				continue;

			uint8_t* xnkidPtr = entryPtr + 0x9E;
			uint8_t* xnaddrPtr = entryPtr + 0xAE;
			uint8_t* ipPtr = entryPtr + 0x170;

			if (memcmp(ipPtr, ina, 0x4) == 0)
			{
				memcpy(pxna, xnaddrPtr, 0x10);
				memcpy(pxnkid, xnkidPtr, 0x10);

				return 1;
			}
		}

		typedef char(*Network_InAddrToXnAddrFunc)(void* ina, void * pxna, void * pxnkid);
		Network_InAddrToXnAddrFunc InAddrToXnAddr = (Network_InAddrToXnAddrFunc)0x52D840;
		return InAddrToXnAddr(ina, pxna, pxnkid);
	}

	// Packet size constants
	const size_t PlayerPropertiesPacketHeaderSize = 0x18;
	const size_t PlayerPropertiesSize = 0x30;
	const size_t PlayerPropertiesPacketFooterSize = 0x4;

	size_t GetPlayerPropertiesPacketSize()
	{
		static size_t size;
		if (size == 0)
		{
			size_t extensionSize = Patches::Network::PlayerPropertiesExtender::Instance().GetTotalSize();
			size = PlayerPropertiesPacketHeaderSize + PlayerPropertiesSize + extensionSize + PlayerPropertiesPacketFooterSize;
		}
		return size;
	}

	// Changes the size of the player-properties packet to include extension data
	void __fastcall RegisterPlayerPropertiesPacketHook(void *thisPtr, void *unused, int packetId, const char *packetName, int arg8, int size1, int size2, void *serializeFunc, void *deserializeFunc, int arg1C, int arg20)
	{
		size_t newSize = GetPlayerPropertiesPacketSize();
		typedef void (__thiscall *RegisterPacketPtr)(void *thisPtr, int packetId, const char *packetName, int arg8, int size1, int size2, void *serializeFunc, void *deserializeFunc, int arg1C, int arg20);
		RegisterPacketPtr RegisterPacket = reinterpret_cast<RegisterPacketPtr>(0x4801B0);
		RegisterPacket(thisPtr, packetId, packetName, arg8, newSize, newSize, serializeFunc, deserializeFunc, arg1C, arg20);
	}
	bool IsNameNotAllowed(std::string name)
	{
		for (std::string s : Modules::ModuleServer::Instance().NonAllowedNames) {
			if (name.find(s) != std::string::npos)
				return true;
		}
		return false;
	}
	void SanitizePlayerName(wchar_t *name)
	{
		int i, dest = 0;
		auto space = false;
		for (i = 0; i < 15 && dest < 15 && name[i]; i++)
		{
			auto allowed = name[i] == '_' ||
				(name[i] >= '0' && name[i] <= '9') ||
				(name[i] >= 'A' && name[i] <= 'Z') ||
				(name[i] >= 'a' && name[i] <= 'z');

			if (!allowed)
			{
				// If this isn't at the beginning of the string, indicate that
				// a space should be inserted before the next allowed character
				if (name[i] == ' ' && dest > 0)
					space = true;
			}
			else
			{
				if (space && dest < 14)
				{
					name[dest++] = ' ';
					space = false;
				}
				name[dest++] = name[i];
			}
		}
		memset(&name[dest], 0, (16 - dest) * sizeof(wchar_t));
		if (dest == 0)
			wcscpy_s(name, 16, L"Forgot");
		else {
			std::string lowercasename = Utils::String::ThinString(name);
			std::transform(lowercasename.begin(), lowercasename.end(), lowercasename.begin(), ::tolower);
			if (IsNameNotAllowed(lowercasename))
				wcscpy_s(name, 16, L"Filtered");
		}
	}

	// Applies player properties data including extended properties
	void __fastcall ApplyPlayerPropertiesExtended(Blam::Network::SessionMembership *thisPtr, void *unused, int playerIndex, uint32_t arg4, uint32_t arg8, uint8_t *data, uint32_t arg10)
	{
		auto properties = &thisPtr->PlayerSessions[playerIndex].Properties;
		bool isNewMember = false;
		auto session = Blam::Network::GetActiveSession();

		// If the player already has a name, then use that instead of the one in the packet
		// This prevents people from changing their name mid-game by forcing a player properties update
		// The player name is stored at the beginning of the player-properties packet (TODO: Map it out properly!)
		if (properties->DisplayName[0])
			memcpy(reinterpret_cast<wchar_t*>(data), properties->DisplayName, sizeof(properties->DisplayName));
		else
		{
			SanitizePlayerName(reinterpret_cast<wchar_t*>(data));
			Server::Signaling::SendPeerPassword(playerIndex);
			Server::Voting::PlayerJoinedVoteInProgress(playerIndex);
			Server::Stats::NewPlayerJoined();
			isNewMember = true;
		}

		auto packetProperties = reinterpret_cast<Blam::Players::ClientPlayerProperties*>(data);
		if (session->HasTeams() && session->MembershipInfo.PlayerSessions[playerIndex].Properties.TeamIndex != packetProperties->TeamIndex)
		{
			uint8_t teamSizes[8];
			auto numPlayers = 0;
			int playerIdx = session->MembershipInfo.FindFirstPlayer();
			while (playerIdx > -1)
			{
				teamSizes[session->MembershipInfo.PlayerSessions[playerIdx].Properties.TeamIndex]++;
				numPlayers++;
				playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
			}

			auto currentTeamIndex = session->MembershipInfo.PlayerSessions[playerIndex].Properties.TeamIndex;

			auto &moduleServer = Modules::ModuleServer::Instance();
			auto maxPlayers = moduleServer.VarServerMaxPlayers->ValueInt;
			auto minTeamSize = moduleServer.VarMinTeamSize->ValueInt;
			auto minTeams = int(std::ceil(numPlayers / float(minTeamSize)));
			auto numTeams = std::min<int>(minTeams, moduleServer.VarNumTeams->ValueInt);

			if (packetProperties->TeamIndex >= 8)
				packetProperties->TeamIndex = currentTeamIndex;
		}

		if (isNewMember)
		{
			//team balancing
			auto prop = &session->MembershipInfo.PlayerSessions[playerIndex].Properties;
			if (session->HasTeams())
			{
				int teamSizes[8] = { 0 };
				int playerIdx = session->MembershipInfo.FindFirstPlayer();
				auto numPlayers = 0;
				while (playerIdx > -1)
				{
					teamSizes[session->MembershipInfo.PlayerSessions[playerIdx].Properties.TeamIndex]++;
					numPlayers++;
					playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
				}
				teamSizes[prop->TeamIndex]--; //ignore one place for the default team they were placed on

				auto &moduleServer = Modules::ModuleServer::Instance();
				auto maxPlayers = moduleServer.VarServerMaxPlayers->ValueInt;
				auto minTeamSize = moduleServer.VarMinTeamSize->ValueInt;
				auto minTeams = int(std::ceil(numPlayers / float(minTeamSize)));
				auto numTeams = std::min<int>(minTeams, moduleServer.VarNumTeams->ValueInt);

				//check joining teams size
				int smallest = Blam::Network::MaxPlayers + 1;
				int smallIndex = 0;
				for (int i = 0; i < numTeams; i++)
				{
					if (teamSizes[i] < smallest)
					{
						smallest = teamSizes[i];
						smallIndex = i;
					}
				}
				packetProperties->TeamIndex = smallIndex;
			}
		}

		// Apply the base properties
		typedef void(__thiscall *ApplyPlayerPropertiesPtr)(void *thisPtr, int playerIndex, uint32_t arg4, uint32_t arg8, void *data, uint32_t arg10);
		const ApplyPlayerPropertiesPtr ApplyPlayerProperties = reinterpret_cast<ApplyPlayerPropertiesPtr>(0x450890);
		ApplyPlayerProperties(thisPtr, playerIndex, arg4, arg8, data, arg10);

		// Apply the extended properties
		Patches::Network::PlayerPropertiesExtender::Instance().ApplyData(playerIndex, properties, data + PlayerPropertiesSize);
	}

	bool __fastcall Network_leader_request_boot_machineHook(void* thisPtr, void* unused, Blam::Network::PeerInfo* peer, int reason)
	{
		auto session = Blam::Network::GetActiveSession();
		auto membership = &session->MembershipInfo;
		auto peerIndex = peer - membership->Peers;
		auto playerIndex = membership->GetPeerPlayer(peerIndex);
		std::string playerName;
		if (playerIndex >= 0)
			playerName = Utils::String::ThinString(membership->PlayerSessions[playerIndex].Properties.DisplayName);

		typedef bool(__thiscall *Network_leader_request_boot_machineFunc)(void *thisPtr, void* peerAddr, int reason);
		auto Network_leader_request_boot_machine = reinterpret_cast<Network_leader_request_boot_machineFunc>(0x45D4A0);
		if (!Network_leader_request_boot_machine(thisPtr, peer, reason))
			return false;

		return true;
	}

	bool __fastcall Network_session_handle_join_requestHook(Blam::Network::Session *thisPtr, void *unused, const Blam::Network::NetworkAddress &address, void *request)
	{
		// Convert the IP to a string
		struct in_addr inAddr;
		inAddr.S_un.S_addr = address.ToInAddr();
		char ipStr[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &inAddr, ipStr, sizeof(ipStr)))
		{
			// Check if the IP is in the ban list
			auto banList = Server::LoadDefaultBanList();
			if (banList.ContainsIp(ipStr) || Server::TempBanList::Instance().ContainsIp(ipStr))
			{
				// Send a join refusal
				typedef void(__thiscall *Network_session_acknowledge_join_requestFunc)(Blam::Network::Session *thisPtr, const Blam::Network::NetworkAddress &address, int reason);
				auto Network_session_acknowledge_join_request = reinterpret_cast<Network_session_acknowledge_join_requestFunc>(0x45A230);
				Network_session_acknowledge_join_request(thisPtr, address, 0); // TODO: Use a special code for bans and hook the join refusal handler so we can display a message to the player

				Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Info, "Refused join request from banned IP %s", ipStr);
				return true;
			}
		}

		// Continue the join process
		typedef bool(__thiscall *Network_session_handle_join_requestFunc)(Blam::Network::Session *thisPtr, const Blam::Network::NetworkAddress &address, void *request);
		auto Network_session_handle_join_request = reinterpret_cast<Network_session_handle_join_requestFunc>(0x4DA410);
		return Network_session_handle_join_request(thisPtr, address, request);
	}

	int Network_GetMaxPlayersHook()
	{
		int maxPlayers = Modules::ModuleServer::Instance().VarServerMaxPlayers->ValueInt;
		return Utils::Clamp(maxPlayers, 1, 16);
	}

	// This completely replaces c_network_session::peer_request_player_desired_properties_update
	// Editing the existing function doesn't allow for a lot of flexibility
	bool __fastcall PeerRequestPlayerDesiredPropertiesUpdateHook(Blam::Network::Session *thisPtr, void *unused, uint32_t arg0, uint32_t arg4, Blam::Players::ClientPlayerProperties *properties, uint32_t argC)
	{
		if (thisPtr->Type == 3)
			return false;

		// Ensure that there is a player associated with the local peer
		auto membership = &thisPtr->MembershipInfo;
		auto playerIndex = thisPtr->MembershipInfo.GetPeerPlayer(membership->LocalPeerIndex);
		if (playerIndex == -1)
			return false;

		wcscpy_s(properties->DisplayName, 16, Modules::ModulePlayer::Instance().UserName);

		// Copy the player properties to a new array and add the extension data
		auto packetSize = GetPlayerPropertiesPacketSize();
		auto extendedSize = packetSize - PlayerPropertiesPacketHeaderSize - PlayerPropertiesPacketFooterSize;
		auto extendedProperties = std::make_unique<uint8_t[]>(extendedSize);
		memcpy(&extendedProperties[0], properties, PlayerPropertiesSize);
		Patches::Network::PlayerPropertiesExtender::Instance().BuildData(playerIndex, &extendedProperties[PlayerPropertiesSize]);

		if (thisPtr->Type == 6 || thisPtr->Type == 7)
		{
			// Apply player properties locally
			ApplyPlayerPropertiesExtended(membership, nullptr, playerIndex, arg0, arg4, &extendedProperties[0], argC);
		}
		else
		{
			// Send player properties across the network
			auto hostPeer = membership->HostPeerIndex;
			auto channelIndex = membership->PeerChannels[hostPeer].ChannelIndex;
			if (channelIndex == -1)
				return true;

			// Allocate the packet
			auto packet = std::make_unique<uint8_t[]>(packetSize);
			memset(&packet[0], 0, packetSize);

			// Initialize it
			typedef void (*InitPacketPtr)(int id, void *packet);
			InitPacketPtr InitPacket = reinterpret_cast<InitPacketPtr>(0x482040);
			InitPacket(thisPtr->AddressIndex, &packet[0]);

			// Set up the header and footer
			*reinterpret_cast<int*>(&packet[0x10]) = arg0;
			*reinterpret_cast<uint32_t*>(&packet[0x14]) = arg4;
			*reinterpret_cast<uint32_t*>(&packet[packetSize - PlayerPropertiesPacketFooterSize]) = argC;

			// Copy the player properties structure in
			memcpy(&packet[PlayerPropertiesPacketHeaderSize], &extendedProperties[0], extendedSize);

			// Send!
			thisPtr->Observer->ObserverChannelSendMessage(thisPtr->Unknown10, channelIndex, 0, 0x1A, packetSize, &packet[0]);
		}
		return true;
	}

	// Serializes extended player-properties data
	void SerializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag)
	{
		// Serialize base data
		typedef void(*SerializePlayerPropertiesPtr)(Blam::BitStream *stream, uint8_t *buffer, bool flag);
		SerializePlayerPropertiesPtr SerializePlayerProperties = reinterpret_cast<SerializePlayerPropertiesPtr>(0x4433C0);
		SerializePlayerProperties(stream, buffer, flag);

		// Serialize extended data
		Patches::Network::PlayerPropertiesExtender::Instance().SerializeData(stream, buffer + PlayerPropertiesSize);
	}

	// Deserializes extended player-properties data
	bool DeserializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag)
	{
		// Deserialize base data
		typedef bool (*DeserializePlayerPropertiesPtr)(Blam::BitStream *stream, uint8_t *buffer, bool flag);
		DeserializePlayerPropertiesPtr DeserializePlayerProperties = reinterpret_cast<DeserializePlayerPropertiesPtr>(0x4432E0);
		bool succeeded = DeserializePlayerProperties(stream, buffer, flag);

		// Deserialize extended data
		if (succeeded)
			Patches::Network::PlayerPropertiesExtender::Instance().DeserializeData(stream, buffer + PlayerPropertiesSize);
		return succeeded;
	}

	char __fastcall Network_state_end_game_write_stats_enterHook(void* thisPtr, int unused, int a2, int a3, int a4)
	{
		// There used to be a stats upload here, which is why this hook does nothing

		typedef char(__thiscall *Network_state_end_game_write_stats_enterFunc)(void* thisPtr, int a2, int a3, int a4);
		Network_state_end_game_write_stats_enterFunc Network_state_end_game_write_stats_enter = reinterpret_cast<Network_state_end_game_write_stats_enterFunc>(0x492B50);
		return Network_state_end_game_write_stats_enter(thisPtr, a2, a3, a4);
	}

	char __fastcall Network_state_leaving_enterHook(void* thisPtr, int unused, int a2, int a3, int a4)
	{
		Patches::Network::StopInfoServer();
		Server::Signaling::StopServer();

		typedef char(__thiscall *Network_state_leaving_enterFunc)(void* thisPtr, int a2, int a3, int a4);
		Network_state_leaving_enterFunc Network_state_leaving_enter = reinterpret_cast<Network_state_leaving_enterFunc>(0x4933E0);
		return Network_state_leaving_enter(thisPtr, a2, a3, a4);
	}

	void PongReceivedHookImpl(const Blam::Network::NetworkAddress &from, const Blam::Network::PongPacket &pong, uint32_t latency)
	{
		for (auto &&callback : pongCallbacks)
			callback(from, pong.Timestamp, pong.ID, latency);
	}

	__declspec(naked) void PongReceivedHook()
	{
		__asm
		{
			push esi // Latency
			push edi // Pong packet
			push dword ptr [ebp + 8] // Sender
			call PongReceivedHookImpl
			add esp, 12
			push 0x49D9FA
			ret
		}
	}

	void LifeCycleStateChangedHookImpl(Blam::Network::LifeCycleState newState)
	{
		for (auto &&callback : lifeCycleStateChangedCallbacks)
			callback(newState);
	}

	__declspec(naked) void LifeCycleStateChangedHook()
	{
		__asm
		{
			pop esi // HACK: esi = return address

			// Execute replaced code
			mov ecx, edi // ecx = New lifecycle state object
			call dword ptr [eax + 8] // lifecycle->enter()

			push dword ptr [ebx] // Lifecycle state type
			call LifeCycleStateChangedHookImpl
			add esp, 4
			jmp esi
		}
	}

	// hook @ 0xA6E982 - http://pastebin.com/ZcsHYsiD
	__declspec(naked) void GetTextureDimensionsHook()
	{
		__asm
		{
			; check datum index against array count
			cmp		ecx, [eax + 34h]
			jl		resume

			; fake success if datum does not exist
			xor		edx, edx
			push	0A6E9B0h
			ret

			resume:
			mov		eax, [eax + 44h]
			lea		eax, [eax + ecx * 8]
			push	0A6E988h
			ret
		}
	}

	// __thiscall
	bool __fastcall Network_GetEndpointHook(char *thisPtr, void *unused)
	{
		char* socket = thisPtr + 12;
		uint32_t port = Modules::ModuleServer::Instance().VarServerGamePort->ValueInt;
		bool success = false;

		//bool __cdecl Network_c_network_link::create_endpoint(int a1, __int16 GamePort, char a3, _DWORD *a4)
		typedef bool(__cdecl *Network_link_create_endpointFunc)(int a1, __int16 GamePort, char a3, void *a4);
		Network_link_create_endpointFunc Network_link_create_endpoint = reinterpret_cast<Network_link_create_endpointFunc>(0x43B6F0);

		//LPVOID __cdecl sub_43FED0(SOCKET socket)
		typedef LPVOID(__cdecl *sub_43FED0Func)(SOCKET socket);
		sub_43FED0Func sub_43FED0 = reinterpret_cast<sub_43FED0Func>(0x43FED0);

		while (true)
		{
			Pointer(0x1860454).WriteFast<uint32_t>(port);
			success = Network_link_create_endpoint(0, (short)port, 1, socket);

			if (success)
				break;

			if (*socket)
			{
				sub_43FED0(*socket);
				*socket = 0;
			}

			if (++port - Modules::ModuleServer::Instance().VarServerGamePort->ValueInt >= 1000)
			{
				Pointer(0x1860454).WriteFast<uint32_t>(Modules::ModuleServer::Instance().VarServerGamePort->ValueInt);
				return success;
			}
		}
		return success;
	}


	char __fastcall Network_session_join_remote_sessionHook(void* thisPtr, int unused, char a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, void *a12, int a13, int a14)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("connecting");
		jsonWriter.Bool(true);
		jsonWriter.Key("success");
		jsonWriter.Bool(false);
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("serverconnect", jsonBuffer.GetString(), true);

		typedef char(__fastcall *Network_session_join_remote_sessionFunc)(void* thisPtr, int unused, char a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, void *a12, int a13, int a14);
		Network_session_join_remote_sessionFunc Network_session_join_remote_session = reinterpret_cast<Network_session_join_remote_sessionFunc>(0x45D1E0);
		return Network_session_join_remote_session(thisPtr, unused, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);;
	}

	// This is only hooked on error states
	int __fastcall Network_session_initiate_leave_protocolHook(void* thisPtr, int unused, char forceClose)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("connecting");
		jsonWriter.Bool(false);
		jsonWriter.Key("success");
		jsonWriter.Bool(false);
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("serverconnect", jsonBuffer.GetString(), true);

		typedef int(__fastcall *Network_session_initiate_leave_protocolFunc)(void* thisPtr, int unused, char forceClose);
		Network_session_initiate_leave_protocolFunc Network_session_initiate_leave_protocol = reinterpret_cast<Network_session_initiate_leave_protocolFunc>(0x45CB80);
		return Network_session_initiate_leave_protocol(thisPtr, unused, forceClose);
	}

	// Hooked on the initial update that fires on server connect
	int __fastcall Network_session_parameters_clearHook(void* thisPtr, int unused)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();
		jsonWriter.Key("connecting");
		jsonWriter.Bool(false);
		jsonWriter.Key("success");
		jsonWriter.Bool(true);
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("serverconnect", jsonBuffer.GetString(), true);

		typedef int(__fastcall *Network_session_parameters_clearFunc)(void* thisPtr, int unused);
		Network_session_parameters_clearFunc Network_session_parameters_clear = reinterpret_cast<Network_session_parameters_clearFunc>(0x486580);
		return Network_session_parameters_clear(thisPtr, unused);
	}

	int __fastcall Network_session_remove_peerHook(Blam::Network::SessionMembership *membership, void *unused, int peerIndex)
	{
		if (membership->HostPeerIndex == membership->LocalPeerIndex)
		{
			Server::Signaling::RemovePeer(peerIndex);
		}

		typedef int(__thiscall *Network_session_remove_peerFunc)(Blam::Network::SessionMembership *membership, int peerIndex);
		Network_session_remove_peerFunc Network_session_remove_peer = reinterpret_cast<Network_session_remove_peerFunc>(0x4500D0);
		return Network_session_remove_peer(membership, peerIndex);
	}

	bool __fastcall Network_session_parameter_countdown_timer_request_change_hook(void* thisPtr, void* unused, int state, int value)
	{
		if (state == 2) // start
			value = Modules::ModuleServer::Instance().VarServerCountdownLobby->ValueInt;

		static auto Network_session_parameter_countdown_timer_request_change = (bool(__thiscall*)(void *thisPtr, int state, int newValue))(0x00453740);
		return Network_session_parameter_countdown_timer_request_change(thisPtr, state, value);
	}

	bool __fastcall c_network_session_parameter_map_variant__request_change_hook(void *thisptr, void *unused, Blam::MapVariant *mapVariant)
	{
		const auto c_network_session_parameter_map_variant__request_change = (bool(__thiscall *)(void *thisptr, Blam::MapVariant *mapVariant))(0x00456480);
		
		auto ret = c_network_session_parameter_map_variant__request_change(thisptr, mapVariant);
		if (ret)
		{
			for (auto &&callback : mapVariantRequestChangeCallbacks)
				callback(mapVariant);
		}
		return ret;
	}

	char __fastcall c_network_session__handle_session_boot_hook(void *thisPtr, void *unused, int a2, int a3)
	{
		Blam::Network::LeaveGame();
		Web::Ui::ScreenLayer::ShowAlert("Booted", "You were booted from the game", Web::Ui::ScreenLayer::AlertIcon::None);
		return 0;
	}

	uint64_t local_user_get_identifier_hook()
	{
		return *(uint64_t*)0x199FAB2; // use the first 64 bits of the xnkaddr (128 bit guid in eldorado)
	}

	bool network_build_local_game_status_hook(uint8_t *data)
	{
		const auto network_build_local_game_status = (bool(*)(uint8_t *data))(0x437EA0);
		if (network_build_local_game_status(data))
		{
			// hax
			*(uint32_t*)(data+0x138) = *(uint32_t*)0x199FAC4;
			*(uint32_t*)(data+0x13C) = *(uint32_t*)0x199FAD4;
			return true;
		}
		return false;
	}
}
