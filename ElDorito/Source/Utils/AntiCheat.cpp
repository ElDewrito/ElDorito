#include <chrono>
#include <tchar.h>
#include <thread>
#include <Windows.h>
#include <Psapi.h>

#include "../Blam/BlamNetwork.hpp"
#include "../CommandMap.hpp"

namespace Utils::AntiCheat
{
	static int ticks = 0;
	void AntiSpeed()
	{
		HMODULE hMods[1024];
		DWORD cbNeeded;
		unsigned int i;

		EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded);
		{
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				if (GetModuleFileNameEx(GetCurrentProcess(), hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
				{
					if (std::strstr(szModName, "speedhack-i386.dll") != NULL)//found speedhack dll from cheat engine.
					{
						Modules::CommandMap::Instance().ExecuteCommand("exit"); //exit game
					}
				}
			}
		}
	}

	//Should keep the script kiddies away.
	//TODO: Replace with a better check
	BOOL CALLBACK AntiTrainer(HWND hwnd, LPARAM lParam)
	{
		char title[80];
		GetWindowText(hwnd, title, sizeof(title));
		if (strcmp(title, "Halo Online Trainer") == 0)
		{
			auto session = Blam::Network::GetActiveSession();
			if (session && session->IsEstablished() && !session->IsHost()) 
			{
				MessageBox(NULL, "Detected Halo Online Trainer!\nOnly hosts are allowed to use a trainer!", "AntiCheat", MB_OK); //Tell the user what they did wrong.
				Modules::CommandMap::Instance().ExecuteCommand("exit"); //exit game
			}
		}

		return TRUE;
	}

	void OnTickCheck()
	{
		if (ticks > 1000)//Only check every so many ticks
		{
			ticks = 0;
			AntiSpeed();
			EnumWindows(AntiTrainer, NULL);
		}
		else
			ticks++;
	}
}
