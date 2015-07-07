#pragma once
#include "Callbacks.hpp"
#include <windows.h>
#include <detours.h>
#include <shlwapi.h>
#include <string>
#include "../Utils/Singleton.hpp"

#pragma comment(lib, "Shlwapi.lib")

class Menu : public Utils::Singleton<Menu>
{
private:
	bool menuEnabled = false;
	int pid = 0;

	HWND hWnd = *((HWND*)0x199C014);

	// Callbacks callbacks{};

	static void startMenu();
	void bindCallbacks();
	bool doesFileExist(const char *fileName);
	bool doesDirExist(const std::string& dirName_in);

public:
	Menu();
	~Menu();

	void toggleMenu();
};