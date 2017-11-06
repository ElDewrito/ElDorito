#include "Pointer.hpp"
#include <Windows.h>
#include <TlHelp32.h> //GetModuleBase
#include <memory>

size_t GetMainThreadId()
{
	const std::shared_ptr<void> hThreadSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0), CloseHandle);
	if( hThreadSnapshot.get() == INVALID_HANDLE_VALUE )
		return 0;

	THREADENTRY32 tEntry;
	tEntry.dwSize = sizeof(THREADENTRY32);
	DWORD result = 0;
	DWORD currentPID = GetCurrentProcessId();
	for( BOOL success = Thread32First(hThreadSnapshot.get(), &tEntry);
		!result && success && GetLastError() != ERROR_NO_MORE_FILES;
		success = Thread32Next(hThreadSnapshot.get(), &tEntry) )
	{
		if( tEntry.th32OwnerProcessID == currentPID )
			result = tEntry.th32ThreadID;
	}
	return result;
}

size_t GetMainThreadId(size_t ProcessId)
{
	LPVOID lpTid;

	_asm
	{
		mov eax, fs:[18h]
			add eax, 36
			mov[lpTid], eax
	}

	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, ProcessId);
	if( hProcess == NULL )
		return NULL;

	DWORD dwTid;
	if( ReadProcessMemory(hProcess, lpTid, &dwTid, sizeof(dwTid), NULL) == FALSE )
	{
		CloseHandle(hProcess);
		return NULL;
	}

	CloseHandle(hProcess);

	return dwTid;
}

void* GetModuleBase()
{
	static void* Base = nullptr;

	if( Base == nullptr )
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
		if( hSnapShot == INVALID_HANDLE_VALUE )
			return nullptr;
		MODULEENTRY32 lpModuleEntry;
		lpModuleEntry.dwSize = sizeof(MODULEENTRY32);
		int bRet = Module32First(hSnapShot, &lpModuleEntry);
		CloseHandle(hSnapShot);
		Base = (bRet != 0) ? (void*)lpModuleEntry.modBaseAddr : nullptr;
	}

	return Base;
}

void* GetModuleBase(const std::string& ModuleName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if( hSnapShot == INVALID_HANDLE_VALUE )
		return 0;

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

void SetPageAccess(size_t Offset, size_t Size, size_t AccessFlags)
{
	DWORD temp;
	VirtualProtect((void*)Offset, Size, AccessFlags, &temp);
}