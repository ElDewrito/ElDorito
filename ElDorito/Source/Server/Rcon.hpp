#pragma once
#include <string>
namespace Server
{
	namespace Rcon
	{
		void SendMessageToClients(std::string message);
		void Initialize();
	}
}