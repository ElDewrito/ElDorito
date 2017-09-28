#pragma once

#define HEADER "\x1B\x1B ED"
#define HEADER_SIZE sizeof(HEADER)
#define MAX_DATA_SIZE 255
#define packet_size HEADER_SIZE + MAX_DATA_SIZE + sizeof(char)

namespace Server::LanBroadcast
{
	void StartServer();
	void StopServer();
}

