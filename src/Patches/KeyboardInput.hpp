#pragma once

#include "../Blam/BlamTypes.hpp"
#include <functional>

namespace Patches
{
	namespace KeyboardInput
	{
		void ApplyAll();

		typedef std::function<void()> KeyboardUpdatedCallback;

		// Register a function to run every time the game refreshes the
		// keyboard state.
		void RegisterUpdateCallback(KeyboardUpdatedCallback callback);

		// Gets the number of ticks that a key has been held down for.
		// Will always be nonzero if the key is down.
		uint8_t GetKeyTicks(Blam::KeyCodes key, Blam::InputType type);

		// Gets the number of milliseconds that a key has been held down for.
		// Will always be nonzero if the key is down.
		uint16_t GetKeyMs(Blam::KeyCodes key, Blam::InputType type);

		// Blocks or unblocks an input type
		void BlockInput(Blam::InputType type, bool block);

		// "Swallows" a key, preventing it from being read again until the
		// keyboard state is refreshed.
		void Swallow(Blam::KeyCodes key);
	}
}