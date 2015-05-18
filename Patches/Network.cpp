#include "Network.h"
#include "PlayerPropertiesExtension.h"
#include "../Patch.h"
#include "../Utils/VersionInfo.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>

#include "../ElDorito.h"
#include "../ElPreferences.h"

namespace
{
	char* Network_GetIPStringFromInAddr(void* inaddr);
	char Network_XnAddrToInAddrHook(void* pxna, void* pxnkid, void* in_addr);
	char Network_InAddrToXnAddrHook(void* ina, void * pxna, void * pxnkid);

	bool __fastcall PeerRequestPlayerDesiredPropertiesUpdateHook(uint8_t *thisPtr, void *unused, uint32_t arg0, uint32_t arg4, void *properties, uint32_t argC);
	void __fastcall ApplyPlayerPropertiesExtended(uint8_t *thisPtr, void *unused, int playerIndex, uint32_t arg4, uint32_t arg8, uint8_t *properties, uint32_t arg10);
	void __fastcall RegisterPlayerPropertiesPacketHook(void *thisPtr, void *unused, int packetId, const char *packetName, int arg8, int size1, int size2, void *serializeFunc, void *deserializeFunc, int arg1C, int arg20);
	void SerializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag);
	bool DeserializePlayerPropertiesHook(Blam::BitStream *stream, uint8_t *buffer, bool flag);
}

namespace Patches
{
	namespace Network
	{
		char motd[100];
		int __stdcall networkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (msg != WM_RCON)
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
				WSAAsyncSelect(clientSocket, hWnd, WM_RCON, FD_READ | FD_WRITE | FD_CLOSE);
				send(clientSocket, motd, strlen(motd), 0);
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
					ElDorito::Instance().ParseCommand(std::string(inDataBuffer));

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
		}

		bool StartRemoteConsole()
		{
			HWND hwnd = Pointer::Base(0x159C014).Read<HWND>();
			if (hwnd == 0)
				return false;

			sprintf_s(motd, 100, "ElDewrito %s Remote Console\r\n", Utils::Version::GetVersionString().c_str());

			SOCKET rconSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			SOCKADDR_IN bindAddr;
			bindAddr.sin_family = AF_INET;
			bindAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			bindAddr.sin_port = htons(2448);

			// open our listener socket
			bind(rconSocket, (PSOCKADDR)&bindAddr, sizeof(bindAddr));
			WSAAsyncSelect(rconSocket, hwnd, WM_RCON, FD_ACCEPT | FD_CLOSE);
			listen(rconSocket, 5);

			return true;
		}
	}
}

namespace
{
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
#pragma endregion

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
}