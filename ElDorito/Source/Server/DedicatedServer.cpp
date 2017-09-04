#include <WS2tcpip.h>
#include "DedicatedServer.hpp"
#include "../Modules/ModuleBase.hpp"

namespace Server::DedicatedServer
{
	bool needToSetMode = false;
	auto get_lobby_type = (int(__cdecl*)())(0x00435640);
	auto set_game_mode = (bool(__cdecl*)(int))(0x00A7F950);

	void Init()
	{
		Modules::CommandMap::Instance().ExecuteCommand("Server.LobbyType 2");
		needToSetMode = true;
	}
	void Tick()
	{
		if (!needToSetMode)
			return;

		if (get_lobby_type() == 2) {
			if (set_game_mode(3)) {
				needToSetMode = false;
			}
		}
	}
}