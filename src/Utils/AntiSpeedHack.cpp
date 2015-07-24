#include <chrono>
#include <tchar.h>
#include <thread>
#include <Windows.h>
#include <Psapi.h>

#include "../CommandMap.hpp"

namespace Utils
{
	namespace AntiSpeedHack
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

		void OnTickCheck()
		{
			if (ticks > 1000)//Only check every so many ticks
			{
				ticks = 0;
				AntiSpeed();
			}
			else
				ticks++;
		}
	}
}