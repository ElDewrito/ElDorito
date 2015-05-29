#include "Logging.h"
#include "../Patch.h"
#include "../ElDorito.h"
#include "../ElMacros.h"

#include <cstdarg>
#include <fstream>
#include "../Modules/ModuleGame.h"

namespace
{
	void dbglog(const char* module, char* format, ...);
	void debuglog_string(char* name, char* value);
	void debuglog_int(char* name, int value);
	void debuglog_float(char* name, float value);
	int networkLogHook(char* format, ...);
	void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5);
	void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5);

	Hook NetworkLogHook(0x9858D0, networkLogHook);
	Hook SSLHook(0xA7FE10, sslLogHook);
	Hook UIHook(0xAED600, uiLogHook);
	Hook Game1Hook(0x106FB0, dbglog);
	Hook DebugLogFloatHook(0x2189F0, debuglog_float);
	Hook DebugLogIntHook(0x218A10, debuglog_int);
	Hook DebugLogStringHook(0x218A30, debuglog_string);
}

namespace Patches
{
	namespace Logging
	{
		void EnableNetworkLog(bool enable)
		{
			if (enable)
				NetworkLogHook.Apply();
			else
				NetworkLogHook.Reset();
		}

		void EnableSslLog(bool enable)
		{
			if (enable)
				SSLHook.Apply();
			else
				SSLHook.Reset();
		}

		void EnableUiLog(bool enable)
		{
			if (enable)
				UIHook.Apply();
			else
				UIHook.Reset();
		}

		void EnableGame1Log(bool enable)
		{
			if (enable)
				Game1Hook.Apply();
			else
				Game1Hook.Reset();
		}

		void EnableGame2Log(bool enable)
		{
			if (enable)
			{
				DebugLogFloatHook.Apply();
				DebugLogIntHook.Apply();
				DebugLogStringHook.Apply();
			}
			else
			{
				DebugLogFloatHook.Reset();
				DebugLogIntHook.Reset();
				DebugLogStringHook.Reset();
			}
		}
	}
}

namespace
{
	TODO("Move dbglog into a header that the whole project can use")
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

		for (auto filter : Modules::ModuleGame::Instance().FiltersExclude)
		{
			if (strstr(buff, filter.c_str()) != NULL)
				return; // string contains an excluded string
		}

		for (auto filter : Modules::ModuleGame::Instance().FiltersInclude)
		{
			if (strstr(buff, filter.c_str()) == NULL)
				return; // string doesn't contain an included string
		}

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
		// fix strings using broken printf statements
		std::string formatStr(format);
		Utils::String::ReplaceString(formatStr, "%LX", "%llX");

		char dstBuf[4096];
		memset(dstBuf, 0, 4096);

		va_list args;
		va_start(args, format);
		vsnprintf_s(dstBuf, 4096, 4096, formatStr.c_str(), args);
		va_end(args);

		dbglog("Network", "%s", dstBuf);

		return 1;
	}

	void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5)
	{
		char* logData1 = (*(char**)(a3));
		char* logData2 = (*(char**)((DWORD_PTR)a3 + 0x8));
		if (logData1 == 0)
			logData1 = "";
		else
			logData1 += 0xC;

		if (logData2 == 0)
			logData2 = "";
		else
			logData2 += 0xC;

		dbglog((const char*)logData1, (char*)logData2);
		return;
	}

	void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5)
	{
		char* logData1 = (*(char**)(a3));
		if (logData1 == 0)
			logData1 = "";
		else
			logData1 += 0xC;

		dbglog("UiLog", (char*)logData1);
		return;
	}
}