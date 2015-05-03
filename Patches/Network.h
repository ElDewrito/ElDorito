#pragma once

#define WM_RCON WM_USER + 1337

namespace Patches
{
	namespace Network
	{
		void ApplyAll();

		bool StartRemoteConsole();
	}
}