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
	}
	Info += "\nUsage: debug (network|ssl|ui|all|off)\n"
		"Enables hooks for debug output\n"
		"Writes debug messages to dorito.log when enabled.\n";

	return Info;
}

void DebugLogging::Tick(const std::chrono::duration<double>& Delta)
{
}

void dbglog(const char* module, char* format, ...)
{
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

	// Set
	const uint8_t networkReset[] = { 0x55, 0x8B, 0xEC, 0x80, 0x3D };
	const uint8_t sslReset[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC };
	const uint8_t uiReset[] = { 0x55, 0x8B, 0xEC, 0x8D, 0x4D };

	bool hookNetwork = false;
	bool hookSSL = false;
	bool hookUI = false;

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

			return true;
		}
		else
		{
			hookNetwork = Args[1].compare("network") == 0;
			hookSSL = Args[1].compare("ssl") == 0;
			hookUI = Args[1].compare("ui") == 0;
			if( Args[1].compare("all") == 0 || Args[1].compare("on") == 0 )
				hookNetwork = hookSSL = hookUI = true;

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

			if( hookNetwork || hookSSL || hookUI )
				return true;
		}
	}

	return false;
}