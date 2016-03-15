#pragma once
#include <functional>

namespace Patches
{
	namespace Core
	{
		void ApplyAll();

		typedef std::function<void()> ShutdownCallback;

		// Registers a function to be called when the game shuts down.
		void OnShutdown(ShutdownCallback callback);
	}
}
