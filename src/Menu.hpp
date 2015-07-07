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
	HWND hWnd = *((HWND*)0x199C014);

	bool doesFileExist(const char *fileName);
	bool doesDirExist(const std::string& dirName_in);
	bool isMenuRunning();
	bool isRunning(int pid);

public:
	void toggleMenu();
};