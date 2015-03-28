#include "Pointer.h"
#include <Windows.h>
#include <TlHelp32.h> //GetModuleBase
#include <memory>

unsigned int GetMainThreadId()
{
	const std::tr1::shared_ptr<void> hThreadSnapshot(
		CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);
	if( hThreadSnapshot.get() == INVALID_HANDLE_VALUE )
	{
		return 0;
	}
	THREADENTRY32 tEntry;
	tEntry.dwSize = sizeof(THREADENTRY32);
	DWORD result = 0;
	DWORD currentPID = GetCurrentProcessId();
	for( BOOL success = Thread32First(hThreadSnapshot.get(), &tEntry);
		!result && success && GetLastError() != ERROR_NO_MORE_FILES;
		success = Thread32Next(hThreadSnapshot.get(), &tEntry) )
	{
		if( tEntry.th32OwnerProcessID == currentPID )
		{
			result = tEntry.th32ThreadID;
		}
	}
	return result;
}

void* GetModuleBase()
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE,
		GetCurrentProcessId());
	if( hSnapShot == INVALID_HANDLE_VALUE )
	{
		return nullptr;
	}
	MODULEENTRY32 lpModuleEntry;
	lpModuleEntry.dwSize = sizeof(MODULEENTRY32);
	int bRet = Module32First(hSnapShot, &lpModuleEntry);
	CloseHandle(hSnapShot);
	return (bRet != 0) ? (void*)lpModuleEntry.modBaseAddr : nullptr;
}

void* GetModuleBase(const std::string& ModuleName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE,
		GetCurrentProcessId());
	if( hSnapShot == INVALID_HANDLE_VALUE )
	{
		return 0;
	}
	MODULEENTRY32 lpModuleEntry;
	lpModuleEntry.dwSize = sizeof(MODULEENTRY32);
	BOOL bModule = Module32First(hSnapShot, &lpModuleEntry);
	while( bModule )
	{
		//If module name matches: return it
		if( !ModuleName.compare(lpModuleEntry.szModule) )
		{
			CloseHandle(hSnapShot);
			return (void*)lpModuleEntry.modBaseAddr;
		}
		bModule = Module32Next(hSnapShot, &lpModuleEntry);
	}
	CloseHandle(hSnapShot);
	return nullptr;
}