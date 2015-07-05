#pragma once

#define WM_RCON WM_USER + 1337
#define WM_INFOSERVER WM_RCON + 1

namespace Patches
{
	namespace Network
	{
		void ApplyAll();
		void ForceDedicated();

		bool StartRemoteConsole();

		bool StartInfoServer();
		bool StopInfoServer();

		bool IsInfoSocketOpen();
	}
}