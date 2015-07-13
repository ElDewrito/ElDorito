#include "Menu.hpp"
#include "Console\GameConsole.hpp"
#include "Modules\ModuleGame.hpp"
#include <fstream>
#include <TlHelp32.h>

int(__cdecl * Menu::loadFinished)(void*) = (int(__cdecl *) (void*)) 0x5312C0;
char(__cdecl * Menu::loadMap)(void*) = (char(__cdecl *) (void*)) 0x566EF0;
char* Menu::mapLoading = nullptr;

Menu::Menu()
{
	// Hook loadFinished & loadMap
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&loadFinished, &hookedLoadFinished);
	DetourAttach((PVOID*)&loadMap, &hookedLoadMap);
}

int __cdecl Menu::hookedLoadFinished(void* a1) {
	if (!strstr(mapLoading, "mainmenu"))
	{
		Menu::Instance().setEnabled(false);
	}

	return (*loadFinished)(a1);
}

char __cdecl Menu::hookedLoadMap(void* a1) {
	mapLoading = (char*)((uint8_t*)a1 + 36);
	return (*loadMap)(a1);
}

bool Menu::doesFileExist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

bool Menu::doesDirExist(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

void Menu::setEnabled(bool enable)
{
	bool running = isMenuRunning();
	if (enable && !running)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
	
		std::string arg("custom_menu.exe -hwnd=");
		arg.append(std::to_string((int)hWnd));
		arg.append(" -url=");
		arg.append(Modules::ModuleGame::Instance().VarMenuURL->ValueString);
		
		CreateProcess(0, (LPSTR)arg.c_str(), 0, 0, false, 0, 0, 0, &si, &pi);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		pid = pi.dwProcessId;

		// The custom menu will automatically hide the game window. I do it there instead of here to minimize the switch delay.
	}

	if (!enable && running)
	{
		ShowWindow(hWnd, SW_SHOW);
		TerminateProcess(OpenProcess(PROCESS_TERMINATE, false, pid), 0);
	}
}

bool Menu::isMenuRunning()
{
	if (!pid)
	{
		return false;
	}
	
	if (isRunning(pid))
	{
		return true;
	}
	else
	{
		return false;
		pid = 0;
	}
}

bool Menu::isRunning(int pid)
{
	HANDLE pss = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(pe);

	if (Process32First(pss, &pe))
	{
		do
		{
			if (pe.th32ProcessID == pid)
				return true;
		} while (Process32Next(pss, &pe));
	}

	CloseHandle(pss);

	return false;
}