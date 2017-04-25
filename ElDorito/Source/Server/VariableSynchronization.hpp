#pragma once

#include "../CommandMap.hpp"

namespace Server
{
	namespace VariableSynchronization
	{
		// Initializes the synchronization system.
		void Initialize();

		// Creates a synchronization binding between a server variable and a
		// client variable. If the local machine is the host, then changing the
		// server variable will update the client variable and send out change
		// notification packets. Otherwise, if the local machine is a client,
		// then the value of the client variable will be set based on the value
		// of the host's server variable.
		void Synchronize(Modules::Command *serverVariable, Modules::Command *clientVariable);

		// Updates the synchronization system.
		void Tick();
	}
}