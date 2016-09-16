#pragma once

namespace Voip
{
	namespace Server
	{
		// Shuts down the VoIP server subsystem. Only call this when quitting.
		void Shutdown();

		// Starts the VoIP server asynchronously.
		void Start();

		// Stops the VoIP server.
		void Stop();

		// Returns true if a peer is connected to the VoIP server.
		bool IsPeerConnected(int peerIndex);
	}
}