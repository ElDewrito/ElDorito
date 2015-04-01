#include "DebugLogging.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

void dbglog(const char* module, char* format, ...)
{
	char* backupFormat = "";
	if (!format)
		format = backupFormat;

	if (module != 0 && strcmp(module, "game_tick") == 0)
		return; // filter game_tick spam

	va_list ap;
	va_start(ap, format);

	char buff[4096];
	vsprintf_s(buff, 4096, format, ap);
	va_end(ap);

	std::ofstream outfile;
	outfile.open("dorito.log", std::ios_base::app);
	outfile << '[' << module << "] " << buff << '\n';
	outfile.close();
}

void debuglog_string(char* name, char* value)
{
	dbglog("Debug", "%s: %s", name, value);
}

void debuglog_int(char* name, int value)
{
	dbglog("Debug", "%s: %d", name, value);
}

void debuglog_float(char* name, float value)
{
	dbglog("Debug", "%s: %f", name, value);
}

int networkLogHook(char* format, ...)
{
	char dstBuf[4096];
	memset(dstBuf, 0, 4096);

	va_list args;
	va_start(args, format);
	vsnprintf_s(dstBuf, 4096, 4096, format, args);
	//vsnprintf(dstBuf, 4095, format, args);
	va_end(args);

	dbglog("Network", "%s", dstBuf);

	return 1;
}

void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5)
{
	char* logData1 = (*(char**)(a3));
	char* logData2 = (*(char**)((DWORD_PTR)a3 + 0x8));
	if( logData1 != 0 )
		logData1 += 0xC;
	if( logData2 != 0 )
		logData2 += 0xC;
	if( logData1 == 0 )
		logData1 = "";
	if( logData2 == 0 )
		logData2 = "";
	dbglog((const char*)logData1, (char*)logData2);
	return;
}

void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5)
{
	char* logData1 = (*(char**)(a3));
	if( logData1 != 0 )
		logData1 += 0xC;
	if( logData1 == 0 )
		logData1 = "";
	dbglog("UiLog", (char*)logData1);
	return;
}

DebugLogging::DebugLogging() : enabledFlags(0),
	NetworkLogHook(0x9858D0, false, 
	&networkLogHook,
	{ 0x55, 0x8B, 0xEC, 0x80, 0x3D }),

	SSLHook(0xA7FE10, false,
	&sslLogHook,
	{ 0x55, 0x8B, 0xEC, 0x83, 0xEC }),

	UIHook(0xAED600, false,
	&uiLogHook,
	{ 0x55, 0x8B, 0xEC, 0x8D, 0x4D }),

	Game1Hook(0x106FB0, false,
	&dbglog,
	{ 0x55, 0x8B, 0xEC, 0x8B, 0x45 }),

	DebugLogFloatHook(0x2189F0, false,
	&debuglog_float,
	{ 0xC2, 0x08, 0x00, 0xCC, 0xCC }),

	DebugLogIntHook(0x218A10, false,
	&debuglog_int,
	{ 0xC2, 0x08, 0x00, 0xCC, 0xCC }),

	DebugLogStringHook(0x218A30, false,
	&debuglog_string,
	{ 0xC2, 0x08, 0x00, 0xCC, 0xCC })
{
}

DebugLogging::~DebugLogging()
{
}

std::string DebugLogging::Info()
{
	std::string Info = "Debug logging: ";
	if( enabledFlags == 0 )
		Info += "Disabled";
	else
	{
		Info += "Enabled - ";
		Info += (enabledFlags & 2) ? "Network " : "";
		Info += (enabledFlags & 4) ? "SSL " : "";
		Info += (enabledFlags & 8) ? "UI " : "";
		Info += (enabledFlags & 16) ? "Game1 " : "";
		Info += (enabledFlags & 32) ? "Game2 " : "";
	}
	Info += "\nUsage: debug (network|ssl|ui|game1|game2|all|off)\n"
		"Enables hooks for debug output\n"
		"Writes debug messages to dorito.log when enabled.\n";

	return Info;
}

void DebugLogging::Tick(const std::chrono::duration<double>& Delta)
{
}

bool DebugLogging::Run(const std::vector<std::string>& Args)
{
	bool hookNetwork = false;
	bool hookSSL = false;
	bool hookUI = false;
	bool hookGame1 = false;
	bool hookGame2 = false;

	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			// Disable it.
			std::cout << "Disabling hooks" << std::endl;
			enabledFlags = 0;

			NetworkLogHook.Reset();
			SSLHook.Reset();
			UIHook.Reset();
			Game1Hook.Reset();
			DebugLogFloatHook.Reset();
			DebugLogIntHook.Reset();
			DebugLogStringHook.Reset();

			return true;
		}
		else
		{
			hookNetwork = Args[1].compare("network") == 0;
			hookSSL = Args[1].compare("ssl") == 0;
			hookUI = Args[1].compare("ui") == 0;
			hookGame1 = Args[1].compare("game1") == 0;
			hookGame2 = Args[1].compare("game2") == 0;
			if( Args[1].compare("all") == 0 || Args[1].compare("on") == 0 )
				hookNetwork = hookSSL = hookUI = hookGame1 = hookGame2 = true;

			if( hookNetwork )
			{
				std::cout << "Hooking network debug output..." << std::endl;
				enabledFlags |= 2;

				NetworkLogHook.Apply();
			}

			if( hookSSL )
			{
				std::cout << "Hooking SSL debug output..." << std::endl;
				enabledFlags |= 4;

				SSLHook.Apply();
			}

			if( hookUI )
			{
				std::cout << "Hooking UI debug output..." << std::endl;
				enabledFlags |= 8;

				UIHook.Apply();
			}

			if ( hookGame1 )
			{
				std::cout << "Hooking Game1 debug output..." << std::endl;
				enabledFlags |= 16;

				Game1Hook.Apply();
			}

			if ( hookGame2 )
			{
				std::cout << "Hooking Game2 debug output..." << std::endl;
				enabledFlags |= 32;

				DebugLogFloatHook.Apply();
				DebugLogIntHook.Apply();
				DebugLogStringHook.Apply();
			}

			if( hookNetwork || hookSSL || hookUI || hookGame1 || hookGame2 )
				return true;
		}
	}

	return false;
}
