#pragma once

#define WM_RCON WM_USER + 1337
#define WM_INFOSERVER WM_RCON + 1
#include <functional>
#include "../Blam/BlamNetwork.hpp"

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

		// Callback for a pong handler function.
		// from - The address the pong was received from
		// timestamp - The timestamp on the original ping (from timeGetTime)
		// id - The ID on the original ping
		// latency - Round-trip time in milliseconds
		typedef std::function<void(const Blam::Network::NetworkAddress &from, uint32_t timestamp, uint16_t id, uint32_t latency)> PongCallback;

		// Registers a function to be called when a pong is received.
		void OnPong(PongCallback callback);
	}
}