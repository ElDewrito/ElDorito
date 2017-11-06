#pragma once

#include <string>

namespace Server::Signaling
{
	void Initialize();
	void StartServer();
	void StopServer();
	void SendPeerPassword(int playerIndex);
	void RemovePeer(int peerIndex);
	std::string GetPassword();
	uint16_t GetPort();
}

