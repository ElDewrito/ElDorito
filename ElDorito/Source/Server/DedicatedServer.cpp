#include <WS2tcpip.h>
#include "DedicatedServer.hpp"
#include "../Modules/ModuleBase.hpp"
#include "../Blam/BlamNetwork.hpp"

namespace Server::DedicatedServer
{
	bool needToSetMode = false;

	void Init()
	{
		Blam::Network::SetLobbyType(2);
		needToSetMode = true;
	}
	void Tick()
	{
		if (!needToSetMode) {
			if (Blam::Network::GetLobbyType() != 2) {
				Blam::Network::SetLobbyType(2);
				needToSetMode = true;
			}
			return;
		}

		if (Blam::Network::GetLobbyType() == 2) {
			if (Blam::Network::SetNetworkMode(3)) {
				needToSetMode = false;
			}
		}
	}
}