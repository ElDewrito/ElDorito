#include "Console/GameConsole.h"
#include "ElDorito.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <Windows.h>
#include <conio.h> // _getch()
#include <cctype> //isprint

#include <codecvt>
#include <cvt/wstring> // wstring_convert

#include "Utils/Utils.h"
#include "ElPatches.h"
#include "Patches/PlayerUid.h"

size_t ElDorito::MainThreadID = 0;

ElDorito::ElDorito()
{
}

void ElDorito::Initialize()
{
	::CreateDirectoryA(GetDirectory().c_str(), NULL);

	// init our command modules
	Modules::ElModules::Instance();

	// load variables/commands from cfg file
	Modules::CommandMap::Instance().ExecuteCommand("Execute dewrito_prefs.cfg");
	Modules::CommandMap::Instance().ExecuteCommand("Execute autoexec.cfg"); // also execute autoexec, which is a user-made cfg guaranteed not to be overwritten by ElDew

	// Parse command-line commands
	int numArgs = 0;
	LPWSTR* szArgList = CommandLineToArgvW(GetCommandLineW(), &numArgs);
	bool usingLauncher = Modules::ModuleGame::Instance().VarSkipLauncher->ValueInt == 1;

	if( szArgList && numArgs > 1 )
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		for( int i = 1; i < numArgs; i++ )
		{
			std::wstring arg = std::wstring(szArgList[i]);
			if( arg.compare(0, 1, L"-") != 0 ) // if it doesn't start with -
				continue;

#ifndef _DEBUG
			if (arg.compare(L"-launcher") == 0)
				usingLauncher = true;
#endif

			size_t pos = arg.find(L"=");
			if( pos == std::wstring::npos || arg.length() <= pos + 1 ) // if it doesn't contain an =, or there's nothing after the =
				continue;

			std::string argname = converter.to_bytes(arg.substr(1, pos - 1));
			std::string argvalue = converter.to_bytes(arg.substr(pos + 1));

			Modules::CommandMap::Instance().ExecuteCommand(argname + " \"" + argvalue + "\"");
		}
	}


	// Language patch
	Patch(0x2333FD, { (uint8_t)Modules::ModuleGame::Instance().VarLanguageID->ValueInt }).Apply();

	setWatermarkText("ElDewrito | Version: " + Utils::Version::GetVersionString() + " | Build Date: " __DATE__);

#ifndef _DEBUG
	if (!usingLauncher) // force release builds to use launcher, simple check so its easy to get around if needed
	{
		MessageBox(GetConsoleWindow(), "Please run Halo Online using the ElDewrito launcher.\nIt should be named DewritoUpdater.exe.", "ElDewrito", MB_OK | MB_ICONINFORMATION);
		TerminateProcess(GetCurrentProcess(), 0);
	}
#endif
}

void ElDorito::Tick(const std::chrono::duration<double>& DeltaTime)
{
	Patches::Tick();

	if (!d3d9Loaded && GetModuleHandle("d3d9.dll"))
	{
		d3d9InitTime = GetTickCount();
		d3d9Loaded = true;
	}

	if (!consoleLoaded && d3d9Loaded)
	{
		if (GetTickCount() - d3d9InitTime > 10000)
		{
			consoleLoaded = true;
			GameConsole::Instance(); // initialize console
			Patches::PlayerUid::Get();
		}
	}

	if (consoleLoaded)
	{
		GameConsole::Instance().checkForReturnKey();
	}

	if (!windowTitleSet && *((HWND*)0x199C014))
	{
		windowTitleSet = true;
		setWindowTitle("ElDewrito | Version: " + Utils::Version::GetVersionString() + " | Build Date: " __DATE__);
	}
}

namespace
{
	static void HandleFinder()
	{
	};
}

std::string ElDorito::GetDirectory()
{
	char Path[MAX_PATH];
	HMODULE hMod;
	if( !GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&::HandleFinder, &hMod) )
	{
		int Error = GetLastError();
		OutputDebugString(std::string("Unable to resolve current directory, error code: ").append(std::to_string(Error)).c_str());
	}
	GetModuleFileNameA(hMod, Path, sizeof(Path));
	std::string Dir(Path);
	Dir = Dir.substr(0, std::string(Dir).find_last_of('\\') + 1);
	return Dir;
}

void ElDorito::setWindowTitle(const std::string& Message)
{
	SetWindowText(*((HWND*) 0x199C014), Message.c_str());
}

// This is for the watermark in the bottom right corner (hidden by default)
void ElDorito::setWatermarkText(const std::string& Message)
{
	static wchar_t msgBuf[256];
	wmemset(msgBuf, 0, 256);

	std::wstring msgUnicode = Utils::String::WidenString(Message);
	wcscpy_s(msgBuf, 256, msgUnicode.c_str());

	Pointer::Base(0x2E5338).Write<uint8_t>(0x68);
	Pointer::Base(0x2E5339).Write(&msgBuf);
	Pointer::Base(0x2E533D).Write<uint8_t>(0x90);
	Pointer::Base(0x2E533E).Write<uint8_t>(0x90);
}

Pointer ElDorito::GetMainTls(size_t Offset)
{
	static Pointer ThreadLocalStorage;
	if( !ThreadLocalStorage && GetMainThreadID() )
	{
		size_t MainThreadID = GetMainThreadID();

		HANDLE MainThreadHandle = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, false, MainThreadID);

		// Get thread context
		CONTEXT MainThreadContext;
		MainThreadContext.ContextFlags = CONTEXT_FULL;

		if (MainThreadID != GetCurrentThreadId())
			SuspendThread(MainThreadHandle);

		BOOL success = GetThreadContext(MainThreadHandle, &MainThreadContext);
		if( !success )
		{
			OutputDebugString(std::string("Error getting thread context: ").append(std::to_string(GetLastError())).c_str());
			std::exit(1);
		}
		ResumeThread(MainThreadHandle);

		// Get thread selector

		LDT_ENTRY MainThreadLdt;

		success = GetThreadSelectorEntry(MainThreadHandle, MainThreadContext.SegFs, &MainThreadLdt);
		if( !success )
		{
			OutputDebugString(std::string("Error getting thread context: ").append(std::to_string(GetLastError())).c_str());
		}
		size_t TlsPtrArrayAddress = (size_t)((size_t)(MainThreadLdt.HighWord.Bits.BaseHi << 24) | (MainThreadLdt.HighWord.Bits.BaseMid << 16) | MainThreadLdt.BaseLow) + 0x2C;
		size_t TlsPtrAddress = Pointer(TlsPtrArrayAddress).Read<uint32_t>();

		// Index has been consistantly 0. Keep a look out.
		ThreadLocalStorage = Pointer(TlsPtrAddress)[0];
	}

	return ThreadLocalStorage(Offset);
}