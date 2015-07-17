#pragma once
#include <windows.h>
#include <detours.h>
#include <shlwapi.h>
#include <string>
#include "Utils/Singleton.hpp"

#pragma comment(lib, "Shlwapi.lib")

class Menu : public Utils::Singleton<Menu>
{
private:
	int pid = 0;
	static char* mapLoading;
	static int(__cdecl * loadFinished)(void*);
	static char(__cdecl * loadMap)(void*);

	bool doesFileExist(const char *fileName);
	bool doesDirExist(const std::string& dirName_in);
	bool isRunning(int pid);
	static int __cdecl hookedLoadFinished(void* a1);
	static char __cdecl hookedLoadMap(void* a1);

public:
	void setEnabled(bool enable);
	bool isMenuRunning();

	Menu();
};