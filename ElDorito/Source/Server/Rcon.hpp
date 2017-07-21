#pragma once
#include <string>
namespace Server::Rcon
{
	void SendMessageToClients(std::string message);
	void Initialize();
}
