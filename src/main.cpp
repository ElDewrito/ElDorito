#include "Console/GameConsole.hpp"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <TlHelp32.h> //GetModuleBase
#include <memory>
#include <IPTypes.h> // for proxy
#include <fstream>
#include <detours.h>

#include "Utils/VersionInfo.hpp"
#include "ElDorito.hpp"
#include "ElPatches.hpp"
#include "Menu.hpp"

/*LONG WINAPI TopLevelExceptionHandler(unsigned int code, EXCEPTION_POINTERS *pExceptionInfo)
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
}*/

extern BOOL installMedalJunk();

void initMedals()
{
	// This is kind of a hack, but only install the medal system for now if
	// halo3.zip can be opened for reading
	std::ifstream halo3Zip("mods\\medals\\halo3.zip");
	if (!halo3Zip.is_open())
		return;
	halo3Zip.close();
	installMedalJunk();
}

int(__cdecl * loadFinished)(void*) = (int(__cdecl *) (void*)) 0x5312C0;

int __cdecl loadFinishedHook(void* a1) {
	Menu::Instance().toggleMenu();
	return (*loadFinished)(a1);
}

BOOL InitInstance(HINSTANCE hModule)
{
	DisableThreadLibraryCalls(hModule);
	initMedals();

	Utils::Version::SetModule(hModule);
	ElDorito::SetMainThreadID(GetCurrentThreadId());

	Patches::ApplyRequired();
	ElDorito::Instance().Initialize();

	// TODO: Clean up Patches::ApplyRequired() so that it's written like it is in C++ instead of C
	
	// Hook loadFinished
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&loadFinished, &loadFinishedHook);

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugString("[sub_5312C0] loadFinished hook failed");
	}

	return true;
}

BOOL ExitInstance()
{
	return true;
}

// TODO: remove this export, removing it needs us to use a new mtndew-enabled eldorado.exe though
DWORD GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
	return 0;
}

int GetEDVersion()
{
	return Utils::Version::GetVersionInt();
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