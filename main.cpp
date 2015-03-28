#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h> //_O_TEXT
#include <io.h> //_open_osfhandle
#include <TlHelp32.h> //GetModuleBase
#include <memory>

#include "ElDorito.h"

int Thread()
{
	/*
	const std::tr1::shared_ptr<void> hThreadSnapshot(
	CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);
	if( hThreadSnapshot.get() == INVALID_HANDLE_VALUE )
	{
	return 0;
	}
	THREADENTRY32 tEntry;
	tEntry.dwSize = sizeof(THREADENTRY32);
	DWORD MainThread = 0;
	DWORD currentPID = GetCurrentProcessId();
	for( BOOL success = Thread32First(hThreadSnapshot.get(), &tEntry);
	!MainThread && success && GetLastError() != ERROR_NO_MORE_FILES;
	success = Thread32Next(hThreadSnapshot.get(), &tEntry) )
	{
	if( tEntry.th32OwnerProcessID == currentPID )
	{
	MainThread = tEntry.th32ThreadID;
	}
	}

	std::chrono::high_resolution_clock::time_point PrevTime, CurTime;
	HANDLE NewThread = OpenThread(THREAD_ALL_ACCESS, 0, MainThread);
	DWORD MainThreadStatus = WAIT_TIMEOUT;
	while( MainThreadStatus == WAIT_TIMEOUT )
	{
	MainThreadStatus = WaitForSingleObject(Thread, 0);
	PrevTime = CurTime;
	CurTime = std::chrono::high_resolution_clock::now();
	ElDorito::Instance().Tick(CurTime - PrevTime);
	}
	*/

	std::chrono::high_resolution_clock::time_point PrevTime, CurTime;
	CurTime = std::chrono::high_resolution_clock::now();
	while( true )
	{
		PrevTime = CurTime;
		CurTime = std::chrono::high_resolution_clock::now();
		ElDorito::Instance().Tick(CurTime - PrevTime);
	}
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD Reason, LPVOID Misc)
{
	if( Reason == DLL_PROCESS_ATTACH )
	{
		::DisableThreadLibraryCalls(hDLL);
		//Set up new console window
		//Input
		::AllocConsole();
		::SetConsoleTitleA("ElDorito");
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		::EnableMenuItem(::GetSystemMenu(::GetConsoleWindow(), FALSE),
						 SC_CLOSE | SC_MINIMIZE,
						 MF_GRAYED);
		::DrawMenuBar(::GetConsoleWindow());
		HANDLE hStd = ::GetStdHandle(STD_INPUT_HANDLE); // STD in Handle
		unsigned int hConsole = _open_osfhandle((intptr_t)hStd, _O_TEXT); // Console Handle
		FILE* fIn = ::_fdopen(hConsole, "r");
		*stdin = *fIn;
		setvbuf(stdin, NULL, _IONBF, 0);
		//Output
		::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
		consoleInfo.dwSize.Y = 25;
		consoleInfo.dwSize.X = 30;
		::SetConsoleScreenBufferSize(::GetStdHandle(STD_OUTPUT_HANDLE), consoleInfo.dwSize);
		hStd = ::GetStdHandle(STD_OUTPUT_HANDLE); // STD out Handle
		hConsole = ::_open_osfhandle((intptr_t)hStd, _O_TEXT); // Console Handle
		FILE* fOut = ::_fdopen(hConsole, "w");
		*stdout = *fOut;
		::setvbuf(stdout, NULL, _IONBF, 0);
		std::ios::sync_with_stdio();
		::CreateThread(
			NULL,
			NULL,
			(LPTHREAD_START_ROUTINE)&Thread,
			NULL,
			0,
			NULL);
		return true;
	}
	else if( Reason == DLL_PROCESS_DETACH )
	{
		return true;
	}
	else if( Reason == DLL_THREAD_ATTACH )
	{
		return true;
	}
	else if( Reason == DLL_THREAD_DETACH )
	{
		return true;
	}
	else
	{
		return true;
	}
	return false;
}