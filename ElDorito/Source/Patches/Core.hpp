#pragma once
#include <functional>
#include <string>

namespace Patches::Core
{
	void ApplyAll();
	void SetMapsFolder(const std::string &path);
		
	typedef std::function<void()> ShutdownCallback;

	// Registers a function to be called when the game shuts down.
	void OnShutdown(ShutdownCallback callback);

	void ExecuteShutdownCallbacks();

	// A function that should be called when a map finishes loading.
	// mapPath is the path to the map file without the extension, e.g. "maps\mainmenu".
	typedef std::function<void(const char *mapPath)> MapLoadedCallback;

	// Registers a function to be called when a map finishes loading.
	void OnMapLoaded(MapLoadedCallback callback);

	typedef std::function<void()> GameStartCallback;

	// Registers a function to be called when a game/round is about to start.
	void OnGameStart(GameStartCallback callback);
}