#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace DebugHelper {
	void Translate(LPTSTR lpszFunction);
	void TranslateConsole(LPTSTR lpszFunction);
	void DisplayAddress(DWORD address, short radix);
	void __cdecl UnloadHAC(void*);
	void ReloadHAC();
	DWORD WINAPI FreeAndLoad(void* buffer);
};