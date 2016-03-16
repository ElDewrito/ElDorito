#pragma once

#include <map>

#include "Utils/Utils.hpp"
#include "Pointer.hpp"

class ElDorito : public Utils::Singleton < ElDorito >
{
public:
	bool GameHasMenuShown = false;

	static void SetMainThreadID(size_t ThreadID)
	{
		MainThreadID = ThreadID;
	}
	static size_t GetMainThreadID()
	{
		return MainThreadID;
	}

	static Pointer GetMainTls(size_t Offset = 0);

	ElDorito();
	~ElDorito() = default;

	std::string GetDirectory();

	void Initialize();
	void Tick();
	void OnMainMenuShown();
	std::string GetMapsFolder() const { return mapsFolder; }
	bool IsWebDebuggingEnabled() const { return webDebugging; }

private:
	static size_t MainThreadID; // Thread
	bool executeCommandQueue = false;
	bool isDedicated = false;
	std::string mapsFolder;
	bool webDebugging = false;
	//static bool(__cdecl * Video_InitD3D)(bool, bool);

	void setWatermarkText(const std::string& Message);
	void killProcessByName(const char *filename, int ourProcessID);
	//static bool __cdecl hooked_Video_InitD3D(bool windowless, bool nullRefDevice);
};