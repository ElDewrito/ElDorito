#pragma once
#include <cstdint>

namespace Voip
{
	// An authentication token which is used to connect to a server.
	typedef uint8_t AuthToken[16];

	// Initializes the VoIP subsystem.
	void Initialize();
}
