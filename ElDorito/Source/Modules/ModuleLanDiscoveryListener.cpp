#include "ModuleLanDiscoveryListener.hpp"
#include "ModuleServer.hpp"
#include <sstream>
#include "../ElDorito.hpp"
#include "../Server/LanBroadcast.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../Utils/Logger.hpp"

namespace
{
	std::string process_raw_packet(char *packet)
	{
		char header[HEADER_SIZE];
		char content[packet_size];
		char data[MAX_DATA_SIZE + sizeof(char)];
		strncpy(content, &packet[HEADER_SIZE - 1], packet_size);
		strncpy_s(header, HEADER_SIZE, packet, HEADER_SIZE - 1);
		if (strcmp(header, HEADER) != 0)
			return "";
		int data_size = content[0];
		strncpy_s(data, sizeof(data), &content[1], data_size);
		return std::string(data);
	}

	bool is_listening = false;
	DWORD WINAPI LanListenerThread(LPVOID)
	{
		is_listening = true;
		SOCKET listener_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (listener_socket == -1)
		{
			is_listening = false;
			return 0;
		}

		char broadcast_value = 1;
		setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &broadcast_value, sizeof(char));
		SOCKADDR_IN ListnerAddr;
		memset(&ListnerAddr, 0, sizeof(ListnerAddr));
		ListnerAddr.sin_family = AF_INET;
		int port = Modules::ModuleServer::Instance().VarServerBroadcastPort->ValueInt;
		ListnerAddr.sin_port = htons(port);
		ListnerAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(listener_socket, (SOCKADDR *)&ListnerAddr, sizeof(SOCKADDR_IN)) < 0)
		{
			is_listening = false;
			return 0;
		}
		while (is_listening)
		{
			char rbuf[packet_size];
			SOCKADDR_IN broadcaster_addr;
			int len = sizeof(broadcaster_addr);
			if (recvfrom(listener_socket, rbuf, sizeof(rbuf), 0, (sockaddr*)&broadcaster_addr, &len) > 0)
			{
				std::string data = process_raw_packet(rbuf);
				std::string ip_addr = inet_ntoa(broadcaster_addr.sin_addr);
				rapidjson::Document parsed_data;
				parsed_data.Parse(data.c_str());
				if (parsed_data.HasParseError()) {
					Utils::Logger::Instance().Log(Utils::LogTypes::Network, Utils::LogLevel::Error, "LanDiscovery: Error Processing string:'" + data + "' sent by " + ip_addr);
					continue;
				}

				auto& allocator = parsed_data.GetAllocator();
				rapidjson::Value key("server_address", allocator);
				rapidjson::Value value(ip_addr, allocator);
				parsed_data.AddMember(key, value, allocator);

				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				parsed_data.Accept(writer);

				Web::Ui::ScreenLayer::Notify("received_broadcast", buffer.GetString(), true);
			}
		}
		return 0;
	}

	bool CommandStart(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!is_listening)
			CreateThread(nullptr, 0, LanListenerThread, nullptr, 0, nullptr);
		returnInfo = "Started Lan Listener";
		return true;
	}

	bool CommandStop(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		is_listening = false;
		returnInfo = "Stopped Lan Listener";
		return true;
	}
}

namespace Modules
{
	ModuleLanDiscoveryListener::ModuleLanDiscoveryListener() : ModuleBase("LanDiscoveryListener")
	{
		AddCommand("Start", "lan_listener_start", "Starts the lan listener", eCommandFlagsNone, CommandStart);
		AddCommand("Stop", "lan_listener_stop", "Stops the lan listener", eCommandFlagsNone, CommandStop);
	}
}
