#pragma once
#include <functional>
#include <string>

namespace Patches
{
	namespace Core
	{
		void ApplyAll();
		void SetMapsFolder(const std::string &path);
		
		typedef std::function<void()> ShutdownCallback;

		// Registers a function to be called when the game shuts down.
		void OnShutdown(ShutdownCallback callback);
	}
}
