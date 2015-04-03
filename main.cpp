#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h> //_O_TEXT
#include <io.h> //_open_osfhandle
#include <TlHelp32.h> //GetModuleBase
#include <memory>
#include <IPTypes.h> // for proxy

#include "Utils/VersionInfo.h"
#include "ElDorito.h"

HINSTANCE g_hThisInstance = 0;
HMODULE g_hOriginalDll = 0;

LONG WINAPI TopLevelExceptionHandler(unsigned int code, EXCEPTION_POINTERS *pExceptionInfo)
{
	const char *error_type = "Unknown";
	switch(code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		error_type = "EXCEPTION_ACCESS_VIOLATION";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		error_type = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		break;
	case EXCEPTION_BREAKPOINT:
		error_type = "EXCEPTION_BREAKPOINT";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		error_type = "EXCEPTION_DATATYPE_MISALIGNMENT";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		error_type = "EXCEPTION_FLT_DENORMAL_OPERAND";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		error_type = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		error_type = "EXCEPTION_FLT_INEXACT_RESULT";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		error_type = "EXCEPTION_FLT_INVALID_OPERATION";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		error_type = "EXCEPTION_FLT_OVERFLOW";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		error_type = "EXCEPTION_FLT_STACK_CHECK";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		error_type = "EXCEPTION_FLT_UNDERFLOW";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		error_type = "EXCEPTION_ILLEGAL_INSTRUCTION";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		error_type = "EXCEPTION_IN_PAGE_ERROR";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		error_type = "EXCEPTION_INT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_INT_OVERFLOW:
		error_type = "EXCEPTION_INT_OVERFLOW";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		error_type = "EXCEPTION_INVALID_DISPOSITION";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		error_type = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		error_type = "EXCEPTION_PRIV_INSTRUCTION";
		break;
	case EXCEPTION_SINGLE_STEP:
		error_type = "EXCEPTION_SINGLE_STEP";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		error_type = "EXCEPTION_STACK_OVERFLOW";
		break;
	}

	// Format exception string
	std::cout << "Exception: 0x" << std::uppercase << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode << std::nouppercase << " - " << error_type << std::endl;

	return EXCEPTION_CONTINUE_SEARCH;
}

int Thread()
{
	__try
	{
		std::chrono::high_resolution_clock::time_point PrevTime, CurTime;
		CurTime = std::chrono::high_resolution_clock::now();
		while(true)
		{
			PrevTime = CurTime;
			CurTime = std::chrono::high_resolution_clock::now();
			ElDorito::Instance().Tick(
				std::chrono::duration_cast<std::chrono::duration<double>>(CurTime - PrevTime)
				);
		}
	}
	__except(TopLevelExceptionHandler(GetExceptionCode(), GetExceptionInformation())){
		
	}
	return 0;
}

BOOL InitInstance(HINSTANCE hModule)
{
	DisableThreadLibraryCalls(hModule);
	//Init proxy stuff
	g_hThisInstance = hModule;

	// load original DLL
	char buffer[MAX_PATH];
	GetSystemDirectoryA(buffer, MAX_PATH);
	strcat_s(buffer, MAX_PATH, "\\iphlpapi.dll");

	if( !g_hOriginalDll )
		g_hOriginalDll = ::LoadLibraryA(buffer);

	if( !g_hOriginalDll )
		ExitProcess(0); // exit the hard way

	//Set up new console window
	//Input
	AllocConsole();
	SetConsoleTitleA("ElDorito");
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE | SC_MINIMIZE, MF_GRAYED);
	DrawMenuBar(GetConsoleWindow());

	HANDLE hStd = GetStdHandle(STD_INPUT_HANDLE); // STD in Handle
	unsigned int hConsole = _open_osfhandle((intptr_t)hStd, _O_TEXT); // Console Handle
	FILE* fIn = _fdopen(hConsole, "r");
	*stdin = *fIn;
	setvbuf(stdin, NULL, _IONBF, 0);

	//Output
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
	consoleInfo.dwSize.Y = 25;
	consoleInfo.dwSize.X = 30;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleInfo.dwSize);
	hStd = GetStdHandle(STD_OUTPUT_HANDLE); // STD out Handle
	hConsole = _open_osfhandle((intptr_t)hStd, _O_TEXT); // Console Handle
	FILE* fOut = _fdopen(hConsole, "w");
	*stdout = *fOut;
	setvbuf(stdout, NULL, _IONBF, 0);
	std::ios::sync_with_stdio();

	Utils::Version::SetModule(hModule);
	ElDorito::SetMainThreadID(GetCurrentThreadId());

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&Thread, NULL, 0, NULL);

	return true;
}

BOOL ExitInstance()
{
	if( g_hOriginalDll )
	{
		FreeLibrary(g_hOriginalDll);
		g_hOriginalDll = NULL;
	}

	return true;
}

void* GetOrigProcAddr(char* funcName)
{
#pragma warning(suppress: 6387)
	void* origFunc = GetProcAddress(g_hOriginalDll, funcName);
	if( !origFunc )
		::ExitProcess(0); // exit to be safe

	return origFunc;
}

DWORD GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
	typedef DWORD(WINAPI* funcType)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
	funcType origFunc = (funcType)GetOrigProcAddr("GetAdaptersInfo");

	return origFunc(pAdapterInfo, pOutBufLen);
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD Reason, LPVOID Misc)
{
	switch( Reason )
	{
	case DLL_PROCESS_ATTACH: return InitInstance(hModule);
	case DLL_PROCESS_DETACH: return ExitInstance();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		return true;
	}

	return false;
}