#include "DebugHelper.h"
//#include "console.h"
#include <tchar.h>
#include <strsafe.h>
#include <cstdlib>

void DebugHelper::TranslateConsole(LPTSTR lpszFunction) {
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

	if(lpDisplayBuf == NULL) {
		OutputDebugString("Memory allocation failed!");
		return;
	}

	StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
	//hkDrawText((char*)lpDisplayBuf, C_TEXT_RED);
	
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);	
}

void DebugHelper::Translate(LPTSTR lpszFunction) {
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

	if(lpDisplayBuf == NULL) {
		OutputDebugString("Memory allocation failed!");
		return;
	}

	StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
	
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);	
}

void DebugHelper::DisplayAddress(DWORD address, short radix) {
	char buff[10];
	_itoa_s(address, buff, 10, radix);
	MessageBox(NULL, buff, buff, 0);
	OutputDebugString(buff);
}

void __cdecl DebugHelper::UnloadHAC(void*) {
	HMODULE handle = GetModuleHandle("hac.dll");
	if(handle != NULL)
		FreeLibraryAndExitThread(handle, 0);
}

void DebugHelper::ReloadHAC() {
	//Copy DLL name into halo(ce).exe's address space - safe from unloading
	WCHAR buffer[] = L"hac.dll";
	WCHAR *toLoad = (WCHAR*)VirtualAlloc(NULL, 30, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	//Copy reload function into halo(ce).exe's address space - safe from unloading
	BYTE* copiedFunc = (BYTE*)VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if(copiedFunc == NULL || toLoad == NULL) {
		//hkDrawText("Error allocating memory! Unable to reload.", C_TEXT_RED);
		return;
	}

	wcscpy_s(toLoad, 15, buffer);

	DWORD address;
	
	#ifndef _DEBUG	
		address = (DWORD)FreeAndLoad;	
	#else
		//Calculate final jump address
		DWORD distance = *(DWORD*)((DWORD)FreeAndLoad + 1);
		address = (DWORD)FreeAndLoad + distance + 5;
	#endif

	memcpy(copiedFunc, (void*)address, 4096);
	CreateRemoteThread(GetCurrentProcess(), NULL, 0, (LPTHREAD_START_ROUTINE)copiedFunc, (void*)toLoad, 0, NULL);
}

DWORD WINAPI DebugHelper::FreeAndLoad(void* buffer) {
	WCHAR *dllName = (WCHAR*)buffer;
	DWORD LoadLib = (DWORD)LoadLibraryW;
	DWORD SleepFunc = (DWORD)Sleep;
	HMODULE handle = GetModuleHandleW(dllName);

	if(handle == NULL) {
		//hkDrawText("Reload failed.", C_TEXT_RED);
		return -1;
	}

	FreeLibrary(handle);

	__asm {
		push 5000
		call SleepFunc
		push dllName
		call LoadLib
	}

	return 0;
}