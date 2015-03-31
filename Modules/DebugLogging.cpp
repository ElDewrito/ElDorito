#include "DebugLogging.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

DebugLogging::DebugLogging() : enabledFlags(0)
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
		Info += (enabledFlags & 32) ? "Game1 " : "";
	}
	Info += "\nUsage: debug (network|ssl|ui|game1|game2|all|off)\n"
		"Enables hooks for debug output\n"
		"Writes debug messages to dorito.log when enabled.\n";

	return Info;
}

void DebugLogging::Tick(const std::chrono::duration<double>& Delta)
{
}

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
	memset(buff, 0, 4096);

	vsprintf_s(buff, 4096, format, ap);
	va_end(ap);

	std::ofstream outfile;
	outfile.open("dorito.log", std::ios_base::app);
	outfile << "[" << module << "] " << buff << "\n";
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

bool DebugLogging::Run(const std::vector<std::string>& Args)
{
	const size_t OffsetNetworkLog = 0x9858D0; // todo: investigate this func and find if theres a better place to hook
	const size_t OffsetSSL = 0xA7FE10;
	const size_t OffsetUI = 0xAED600;// this patches UiWidgetManager.UiLog, tags.dat also needs a patch to make root.UiLog call UiWidgetManager.UiLog
	// unless we get DebugPanel in root to show somehow
	const size_t OffsetGame1 = 0x106FB0;

	const size_t OffsetDebugLogFloat = 0x2189F0;
	const size_t OffsetDebugLogInt = 0x218A10;
	const size_t OffsetDebugLogString = 0x218A30;

	// Set
	const uint8_t networkReset[] = { 0x55, 0x8B, 0xEC, 0x80, 0x3D };
	const uint8_t sslReset[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC };
	const uint8_t uiReset[] = { 0x55, 0x8B, 0xEC, 0x8D, 0x4D };
	const uint8_t game1Reset[] = { 0x55, 0x8B, 0xEC, 0x8B, 0x45 };
	const uint8_t game2Reset[] = { 0xC2, 0x08, 0x00, 0xCC, 0xCC };

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

			Pointer::Base(OffsetNetworkLog).Write(networkReset, sizeof(networkReset));
			Pointer::Base(OffsetSSL).Write(sslReset, sizeof(sslReset));
			Pointer::Base(OffsetUI).Write(uiReset, sizeof(uiReset));
			Pointer::Base(OffsetGame1).Write(game1Reset, sizeof(game1Reset));
			Pointer::Base(OffsetDebugLogFloat).Write(game2Reset, sizeof(game2Reset));
			Pointer::Base(OffsetDebugLogInt).Write(game2Reset, sizeof(game2Reset));
			Pointer::Base(OffsetDebugLogString).Write(game2Reset, sizeof(game2Reset));

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

				Pointer::Base(OffsetNetworkLog).WriteJump(&networkLogHook);
			}

			if( hookSSL )
			{
				std::cout << "Hooking SSL debug output..." << std::endl;
				enabledFlags |= 4;

				Pointer::Base(OffsetSSL).WriteJump(&sslLogHook);
			}

			if( hookUI )
			{
				std::cout << "Hooking UI debug output..." << std::endl;
				enabledFlags |= 8;

				Pointer::Base(OffsetUI).WriteJump(&uiLogHook);
			}

			if ( hookGame1 )
			{
				std::cout << "Hooking Game1 debug output..." << std::endl;
				enabledFlags |= 16;

				Pointer::Base(OffsetGame1).WriteJump(&dbglog);
			}

			if ( hookGame2 )
			{
				std::cout << "Hooking Game2 debug output..." << std::endl;
				enabledFlags |= 32;

				Pointer::Base(OffsetDebugLogFloat).WriteJump(&debuglog_float);
				Pointer::Base(OffsetDebugLogInt).WriteJump(&debuglog_int);
				Pointer::Base(OffsetDebugLogString).WriteJump(&debuglog_string);
			}

			if( hookNetwork || hookSSL || hookUI || hookGame1 )
				return true;
		}
	}

	return false;
}