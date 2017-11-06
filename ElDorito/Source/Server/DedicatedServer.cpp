#include <WS2tcpip.h>
#include "DedicatedServer.hpp"
#include "../Modules/ModuleBase.hpp"
#include "../Blam/BlamNetwork.hpp"

namespace Server::DedicatedServer
{
	bool needToSetMode = false;

	void Init()
	{
		Modules::CommandMap::Instance().ExecuteCommand("Server.LobbyType 2");
		needToSetMode = true;
	}
	void Tick()
	{
		if (!needToSetMode)
			return;

		if (Blam::Network::GetLobbyType() == 2) {
			if (Blam::Network::SetNetworkMode(3)) {
				needToSetMode = false;
			}
		}
	}
}