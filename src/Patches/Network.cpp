#include "Network.hpp"
#include "PlayerPropertiesExtension.hpp"
#include "../Patch.hpp"
#include "../Utils/VersionInfo.hpp"
#include "../Modules/ModuleServer.hpp"

#include <cstdio>

#include "../ElDorito.hpp"

#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../VoIP/TeamspeakServer.hpp"
#include "../VoIP/TeamspeakClient.hpp"
#include "../Blam/BlamNetwork.hpp"

namespace
{
	char* Network_GetIPStringFromInAddr(void* inaddr);
	char Network_XnAddrToInAddrHook(void* pxna, void* pxnkid, void* in_addr);
	char Network_InAddrToXnAddrHook(void* ina, void * pxna, void * pxnkid);
	char __cdecl Network_transport_secure_key_createHook(void* xnetInfo);
	DWORD __cdecl Network_managed_session_create_session_internalHook(int a1, int a2);
	bool __fastcall Network_leader_request_boot_machineHook(void* thisPtr, void* unused, Blam::Network::PeerInfo* peer, int reason);

	bool __fastcall PeerRequestPlayerDesiredPropertiesUpdateHook(uint8_t *thisPtr, void *unused, uint32_t arg0, uint32_t arg4, void *properties, uint32_t argC);
	void __fastcall ApplyPlayerPropertiesExtended(uint8_t *thisPtr, void *unused, int playerIndex, uint32_t arg4, uint32_t arg8, uint8_t *properties, uint32_t arg10);
	void __fastcall RegisterPlayerPropertiesPacketHook(void *thisPtr, void *unused, int packetId, const char *packetName, int arg8, int size1, int size2, void *serializeFunc, void *deserializeFunc, int arg1C, int arg20);
	void SerializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag);
	bool DeserializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag);

	char __fastcall Network_state_end_game_write_stats_enterHook(void* thisPtr, int unused, int a2, int a3, int a4);
	char __fastcall Network_state_leaving_enterHook(void* thisPtr, int unused, int a2, int a3, int a4);

	std::vector<Patches::Network::PongCallback> pongCallbacks;
	void PongReceivedHook();

	std::vector<Patches::Network::LifeCycleStateChangedCallback> lifeCycleStateChangedCallbacks;
	void LifeCycleStateChangedHook();
}

namespace Patches
{
	namespace Network
	{
		SOCKET rconSocket;
		SOCKET infoSocket;
		bool rconSocketOpen = false;
		bool infoSocketOpen = false;
		time_t lastAnnounce = 0;
		const time_t serverContactTimeLimit = 30 + (2 * 60);

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
			// TODO: move this somewhere better, it seems fine here though, always announces every "serverContactTimeLimit" seconds even when window isn't focused
			if (infoSocketOpen && Modules::ModuleServer::Instance().VarServerShouldAnnounce->ValueInt)
			{
				time_t curTime;
				time(&curTime);
				if (curTime - lastAnnounce > serverContactTimeLimit) // re-announce every "serverContactTimeLimit" seconds
				{
					lastAnnounce = curTime;
					Modules::CommandMap::Instance().ExecuteCommand("Server.Announce");
				}
			}

			if (msg != WM_RCON && msg != WM_INFOSERVER)
			{
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
			char inDataBuffer[1024];
			bool isValidAscii = true;

			switch (WSAGETSELECTEVENT(lParam))
			{
			case FD_ACCEPT:
				// accept the connection and send our motd
				clientSocket = accept(wParam, NULL, NULL);
				WSAAsyncSelect(clientSocket, hWnd, msg, FD_READ | FD_WRITE | FD_CLOSE);
				if (msg == WM_RCON)
				{
					std::string motd = "ElDewrito " + Utils::Version::GetVersionString() + " Remote Console\r\n";
					send(clientSocket, motd.c_str(), motd.length(), 0);
				}
				break;
			case FD_READ:
				ZeroMemory(inDataBuffer, sizeof(inDataBuffer));
				inDataLength = recv((SOCKET)wParam, (char*)inDataBuffer, sizeof(inDataBuffer) / sizeof(inDataBuffer[0]), 0);

				// check if text is proper ascii, because sometimes it's not
				for (int i = 0; i < inDataLength; i++)
				{
					char buf = inDataBuffer[i];
					if ((buf < 0x20 || buf > 0x7F) && buf != 0xD && buf != 0xA)
					{
						isValidAscii = false;
						break;
					}
				}

				if (isValidAscii)
				{
					if (msg == WM_RCON)
					{
						auto ret = Modules::CommandMap::Instance().ExecuteCommand(inDataBuffer, true);
						if (ret.length() > 0)
						{
							Utils::String::ReplaceString(ret, "\n", "\r\n");
							ret = ret + "\r\n";
							send((SOCKET)wParam, ret.c_str(), ret.length(), 0);
						}
					}
					else if (msg == WM_INFOSERVER)
					{
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
								variantName = std::wstring((wchar_t*)Pointer(0x179254));
								variantType = Pointer(0x179250).Read<uint32_t>();
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

						if(authenticated)
						{
							writer.Key("xnkid");
							writer.String(xnkid.c_str());
							writer.Key("xnaddr");
							writer.String(xnaddr.c_str());
							writer.Key("players");

							writer.StartArray();
							uint32_t playerScoresBase = 0x23F1724;
							//uint32_t playerInfoBase = 0x2162DD0;
							uint32_t playerInfoBase = 0x2162E08;
							uint32_t menuPlayerInfoBase = 0x1863B58;
							uint32_t playerStatusBase = 0x2161808;
							for (int i = 0; i < 16; i++)
							{
								uint16_t score = Pointer(playerScoresBase + (1080 * i)).Read<uint16_t>();
								uint16_t kills = Pointer(playerScoresBase + (1080 * i) + 4).Read<uint16_t>();
								uint16_t assists = Pointer(playerScoresBase + (1080 * i) + 6).Read<uint16_t>();
								uint16_t deaths = Pointer(playerScoresBase + (1080 * i) + 8).Read<uint16_t>();

								wchar_t* name = Pointer(playerInfoBase + (5696 * i));
								std::string nameStr = Utils::String::ThinString(name);

								wchar_t* menuName = Pointer(menuPlayerInfoBase + (0x1628 * i));
								std::string menuNameStr = Utils::String::ThinString(menuName);

								uint32_t ipAddr = Pointer(playerInfoBase + (5696 * i) - 88).Read<uint32_t>();
								uint16_t team = Pointer(playerInfoBase + (5696 * i) + 32).Read<uint16_t>();
								uint16_t num7 = Pointer(playerInfoBase + (5696 * i) + 36).Read<uint16_t>();

								uint8_t alive = Pointer(playerStatusBase + (176 * i)).Read<uint8_t>();

								uint64_t uid = Pointer(playerInfoBase + (5696 * i) - 8).Read<uint64_t>();
								std::string uidStr;
								Utils::String::BytesToHexString(&uid, sizeof(uint64_t), uidStr);

								if (menuNameStr.empty() && nameStr.empty() && ipAddr == 0)
									continue;

								writer.StartObject();
								writer.Key("name");
								writer.String(menuNameStr.c_str());
								writer.Key("score");
								writer.Int(score);
								writer.Key("kills");
								writer.Int(kills);
								writer.Key("assists");
								writer.Int(assists);
								writer.Key("deaths");
								writer.Int(deaths);
								writer.Key("team");
								writer.Int(team);
								writer.Key("isAlive");
								writer.Bool(alive == 1);
								writer.Key("uid");
								writer.String(uidStr.c_str());
								writer.EndObject();
							}
							writer.EndArray();
						}
						else
						{
							writer.Key("passworded");
							writer.Bool(true);
						}

						writer.Key("gameVersion");
						writer.String(gameVersion.c_str());
						writer.Key("eldewritoVersion");
						writer.String(Utils::Version::GetVersionString().c_str());
						writer.EndObject();

						std::string replyData = s.GetString();
						std::string reply = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nServer: ElDewrito/" + Utils::Version::GetVersionString() + "\r\nContent-Length: " + std::to_string(replyData.length()) + "\r\nConnection: close\r\n\r\n" + replyData;
						send((SOCKET)wParam, reply.c_str(), reply.length(), 0);
					}
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
		}

		void ForceDedicated()
		{
			// Put the game into dedicated server mode
			Patch(0x2E600, { 0xB0, 0x01 }).Apply();

			// Force syslink to still work
			Patch(0x12D62E, { 0xEB }).Apply();
			Patch(0x12D67A, { 0xEB }).Apply();
			Patch(0x5A8BB, { 0xEB }).Apply();
		}

		bool StartRemoteConsole()
		{
			if (rconSocketOpen)
				return true;

			HWND hwnd = Pointer::Base(0x159C014).Read<HWND>();
			if (hwnd == 0)
				return false;

			rconSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			SOCKADDR_IN bindAddr;
			bindAddr.sin_family = AF_INET;
			bindAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			bindAddr.sin_port = htons(2448);

			// open our listener socket
			bind(rconSocket, (PSOCKADDR)&bindAddr, sizeof(bindAddr));
			WSAAsyncSelect(rconSocket, hwnd, WM_RCON, FD_ACCEPT | FD_CLOSE);
			listen(rconSocket, 5);
			rconSocketOpen = true;

			return true;
		}

		bool StartInfoServer()
		{
			if (infoSocketOpen)
				return true;

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

		void OnPong(PongCallback callback)
		{
			pongCallbacks.push_back(callback);
		}

		void OnLifeCycleStateChanged(LifeCycleStateChangedCallback callback)
		{
			lifeCycleStateChangedCallbacks.push_back(callback);
		}
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
			auto& voipvars = Modules::ModuleVoIP::Instance();
			if (voipvars.VarVoIPServerEnabled->ValueInt == 1){
				//Start the Teamspeak VoIP Server since this is the host
				CreateThread(0, 0, StartTeamspeakServer, 0, 0, 0);

				if (voipvars.VarVoIPEnabled->ValueInt == 1)
				{
					//Make sure teamspeak is stopped before we try to start it.
					StopTeamspeakClient();
					//Join the Teamspeak VoIP Server so the host can talk
					CreateThread(0, 0, StartTeamspeakClient, 0, 0, 0);
				}
			}
			// TODO: give output if StartInfoServer fails
			Patches::Network::StartInfoServer();
		}
		else
		{
			Patches::Network::StopInfoServer();
			//Stop the VoIP Server and client
			StopTeamspeakClient();
			StopTeamspeakServer();
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

			if (memcmp(pxna, xnaddrPtr, 0x10) == 0 && memcmp(pxnkid, xnkidPtr, 0x10) == 0)
			{
				// in_addr struct:
				// 0x0 - 0x10: IP (first 4 bytes for IPv4, 0x10 for IPv6)
				// 0x10 - 0x12: Port number
				// 0x12 - 0x14: IP length (4 for IPv4, 0x10 for IPv6)

				memset(in_addr, 0, 0x14);
				memcpy(in_addr, ipPtr, 4);

				*(uint16_t*)((uint8_t*)in_addr + 0x10) = 11774;
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

	// Applies player properties data including extended properties
	void __fastcall ApplyPlayerPropertiesExtended(uint8_t *thisPtr, void *unused, int playerIndex, uint32_t arg4, uint32_t arg8, uint8_t *properties, uint32_t arg10)
	{
		// Apply the base properties
		typedef void (__thiscall *ApplyPlayerPropertiesPtr)(void *thisPtr, int playerIndex, uint32_t arg4, uint32_t arg8, void *properties, uint32_t arg10);
		const ApplyPlayerPropertiesPtr ApplyPlayerProperties = reinterpret_cast<ApplyPlayerPropertiesPtr>(0x450890);
		ApplyPlayerProperties(thisPtr, playerIndex, arg4, arg8, properties, arg10);

		// Apply the extended properties
		uint8_t *sessionData = thisPtr + 0x10A8 + playerIndex * 0x1648;
		Patches::Network::PlayerPropertiesExtender::Instance().ApplyData(playerIndex, sessionData, properties + PlayerPropertiesSize);
	}

	bool __fastcall Network_leader_request_boot_machineHook(void* thisPtr, void* unused, Blam::Network::PeerInfo* peer, int reason)
	{
		auto session = Blam::Network::GetActiveSession();
		auto membership = &session->MembershipInfo;
		auto peerIndex = peer - membership->Peers;
		auto playerIndex = membership->GetPeerPlayer(peerIndex);
		std::string playerName;
		if (playerIndex >= 0)
			playerName = Utils::String::ThinString(membership->PlayerSessions[playerIndex].DisplayName);
		
		typedef bool(__thiscall *Network_leader_request_boot_machineFunc)(void *thisPtr, void* peerAddr, int reason);
		auto Network_leader_request_boot_machine = reinterpret_cast<Network_leader_request_boot_machineFunc>(0x45D4A0);
		if (!Network_leader_request_boot_machine(thisPtr, peer, reason))
			return false;
		
		// Boot the player from VoIP
		if (playerIndex >= 0)
			kickTeamspeakClient(playerName);
		return true;
	}

	// This completely replaces c_network_session::peer_request_player_desired_properties_update
	// Editing the existing function doesn't allow for a lot of flexibility
	bool __fastcall PeerRequestPlayerDesiredPropertiesUpdateHook(uint8_t *thisPtr, void *unused, uint32_t arg0, uint32_t arg4, void *properties, uint32_t argC)
	{
		int unk0 = *reinterpret_cast<int*>(thisPtr + 0x25B870);
		if (unk0 == 3)
			return false;

		// Get the player index
		int unk1 = *reinterpret_cast<int*>(thisPtr + 0x1A3D40);
		uint8_t *unk2 = thisPtr + 0x20;
		uint8_t *unk3 = unk2 + unk1 * 0xF8 + 0x118;
		typedef int (*GetPlayerIndexPtr)(void *arg0, int arg4);
		GetPlayerIndexPtr GetPlayerIndex = reinterpret_cast<GetPlayerIndexPtr>(0x52E280);
		int playerIndex = GetPlayerIndex(unk3, 0x10);
		if (playerIndex == -1)
			return false;

		// Copy the player properties to a new array and add the extension data
		size_t packetSize = GetPlayerPropertiesPacketSize();
		size_t extendedSize = packetSize - PlayerPropertiesPacketHeaderSize - PlayerPropertiesPacketFooterSize;
		auto extendedProperties = std::make_unique<uint8_t[]>(extendedSize);
		memcpy(&extendedProperties[0], properties, PlayerPropertiesSize);
		Patches::Network::PlayerPropertiesExtender::Instance().BuildData(playerIndex, &extendedProperties[PlayerPropertiesSize]);

		if (unk0 == 6 || unk0 == 7)
		{
			// Apply player properties locally
			ApplyPlayerPropertiesExtended(unk2, NULL, playerIndex, arg0, arg4, &extendedProperties[0], argC);
		}
		else
		{
			// Send player properties across the network
			int unk5 = *reinterpret_cast<int*>(thisPtr + 0x30);
			int unk6 = *reinterpret_cast<int*>(thisPtr + 0x1A3D4C + unk5 * 0xC);
			if (unk6 == -1)
				return true;

			// Allocate the packet
			auto packet = std::make_unique<uint8_t[]>(packetSize);
			memset(&packet[0], 0, packetSize);

			// Initialize it
			int id = *reinterpret_cast<int*>(thisPtr + 0x25BBF0);
			typedef void (*InitPacketPtr)(int id, void *packet);
			InitPacketPtr InitPacket = reinterpret_cast<InitPacketPtr>(0x482040);
			InitPacket(id, &packet[0]);

			// Set up the header and footer
			*reinterpret_cast<int*>(&packet[0x10]) = arg0;
			*reinterpret_cast<uint32_t*>(&packet[0x14]) = arg4;
			*reinterpret_cast<uint32_t*>(&packet[packetSize - PlayerPropertiesPacketFooterSize]) = argC;

			// Copy the player properties structure in
			memcpy(&packet[PlayerPropertiesPacketHeaderSize], &extendedProperties[0], extendedSize);

			// Send!
			int unk7 = *reinterpret_cast<int*>(thisPtr + 0x10);
			void *networkObserver = *reinterpret_cast<void**>(thisPtr + 0x8);

			typedef void (__thiscall *ObserverChannelSendMessagePtr)(void *thisPtr, int arg0, int arg4, int arg8, int messageType, int messageSize, void *data);
			ObserverChannelSendMessagePtr ObserverChannelSendMessage = reinterpret_cast<ObserverChannelSendMessagePtr>(0x4474F0);
			ObserverChannelSendMessage(networkObserver, unk7, unk6, 0, 0x1A, packetSize, &packet[0]);
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
		// TODO: check if the user is hosting/joined a game (ie. make sure the game isn't just an offline game)
		// TODO: make sure the game has had 2 or more players during gameplay
		// TODO: get a game/match ID
		// TODO: make sure we haven't announced stats already for this game ID
		// TODO: make Server.AnnounceStats only callable in code, not via the console (once we've finished debugging it etc

		Modules::CommandMap::Instance().ExecuteCommand("Server.AnnounceStats");

		typedef char(__thiscall *Network_state_end_game_write_stats_enterFunc)(void* thisPtr, int a2, int a3, int a4);
		Network_state_end_game_write_stats_enterFunc Network_state_end_game_write_stats_enter = reinterpret_cast<Network_state_end_game_write_stats_enterFunc>(0x492B50);
		return Network_state_end_game_write_stats_enter(thisPtr, a2, a3, a4);
	}

	char __fastcall Network_state_leaving_enterHook(void* thisPtr, int unused, int a2, int a3, int a4)
	{
		Patches::Network::StopInfoServer();

		StopTeamspeakClient();
		StopTeamspeakServer();

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
}