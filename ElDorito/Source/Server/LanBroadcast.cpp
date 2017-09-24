#include "LanBroadcast.hpp"

#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <string>
#include <iostream>
#include <atlbase.h>

#include "../Patches/CustomPackets.hpp"
#include "../Patches/Network.hpp"
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Blam/BlamEvents.hpp"
#include "../Utils/Utils.hpp"
#include "../Modules/ModuleServer.hpp"

namespace
{
	DWORD WINAPI LanBroadcastThread(LPVOID);
	bool is_broadcasting = false;
}

namespace Server::LanBroadcast
{

	void StartServer()
	{
		if (!is_broadcasting)
			CreateThread(nullptr, 0, LanBroadcastThread, nullptr, 0, nullptr);
	}

	void StopServer()
	{
		is_broadcasting = false;
	}
}

namespace
{

	std::string InfoServerPortJSON()
	{
		auto session = Blam::Network::GetActiveSession();
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		buffer.Clear();

		writer.StartObject();
		writer.Key("info_server_port");
		std::string info_port = Modules::ModuleServer::Instance().VarServerPort->ValueString;
		writer.String(info_port.c_str());

		writer.EndObject();
		return buffer.GetString();
	}

	void build_packet(char *packet, std::string data)
	{
		std::string packet_data = HEADER;
		packet_data = packet_data + (char)data.size() + data;
		memset(packet, 0x00, packet_size);
		strncpy(packet, packet_data.c_str(), packet_data.size());
	}

	//websocket server
	DWORD WINAPI LanBroadcastThread(LPVOID)
	{
		is_broadcasting = true;
		char p[packet_size];

		std::string data = InfoServerPortJSON();
		if (data.size() > MAX_DATA_SIZE)
			data = data.substr(0, MAX_DATA_SIZE);

		build_packet(p, data);

		WORD w = MAKEWORD(1, 1);
		WSADATA wsadata;
		::WSAStartup(w, &wsadata);


		SOCKET broadcast_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (broadcast_socket == -1)
		{
			is_broadcasting = false;
			return 0;
		}
		char broadcasting = true;
		setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, &broadcasting, sizeof(broadcasting));
		SOCKADDR_IN brdcastaddr;
		memset(&brdcastaddr, 0, sizeof(brdcastaddr));
		brdcastaddr.sin_family = AF_INET;
		int port = Modules::ModuleServer::Instance().VarServerBroadcastPort->ValueInt;
		brdcastaddr.sin_port = htons(port);
		brdcastaddr.sin_addr.s_addr = INADDR_BROADCAST;
		while (is_broadcasting) {
			sendto(broadcast_socket, p, strlen(p), 0, (sockaddr*)&brdcastaddr, sizeof(brdcastaddr));
			Sleep(1500);
		}
		::closesocket(broadcast_socket);
		return 0;
	}

}
