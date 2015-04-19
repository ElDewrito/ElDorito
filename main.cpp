#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <TlHelp32.h> //GetModuleBase
#include <memory>
#include <IPTypes.h> // for proxy

#include "Utils/VersionInfo.h"
#include "ElDorito.h"
#include "ElPatches.h"

LONG WINAPI TopLevelExceptionHandler(unsigned int code, EXCEPTION_POINTERS *pExceptionInfo)
{
	const char *error_type = "Unknown";
	switch( code )
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
#ifndef _DEBUG
	__try
	{
#endif
		std::chrono::high_resolution_clock::time_point PrevTime, CurTime;
		CurTime = std::chrono::high_resolution_clock::now();
		while( true )
		{
			TODO("Make this use a tickrate instead of sleeping");
			Sleep(16);
			PrevTime = CurTime;
			CurTime = std::chrono::high_resolution_clock::now();
			ElDorito::Instance().Tick(
				std::chrono::duration_cast<std::chrono::duration<double>>(CurTime - PrevTime)
				);
		}
#ifndef _DEBUG
	}
	__except( TopLevelExceptionHandler(GetExceptionCode(), GetExceptionInformation()) )
	{
	}
#endif
	return 0;
}

BOOL InitInstance(HINSTANCE hModule)
{
	DisableThreadLibraryCalls(hModule);

	Console::AllocateConsole("ElDewrito");

	Utils::Version::SetModule(hModule);
	ElDorito::SetMainThreadID(GetCurrentThreadId());

	// Apply patches before starting the thread
	Patches::ApplyRequired();

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&Thread, NULL, 0, NULL);
	return true;
}

BOOL ExitInstance()
{
	return true;
}

TODO("Change the GetAdaptersInfo export because we don't use an iphlpapi proxy anymore")
DWORD GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
	return 0;
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