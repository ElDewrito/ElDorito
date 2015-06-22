#pragma once
#include "GameConsole.h"
#include <windows.h>
#include <detours.h>

#define HK_IS_PRESSED ((DWORD)lParam & 0x40000000)
#define HK_IS_HELD ((DWORD)lParam & 0x40000000)
#define HK_IS_RELEASED (!((DWORD)lParam & 0x40000000))

class GameConsole;

class KeyboardHook
{
private:
	static HHOOK ourHookedFunctionPtr;
	static LRESULT __stdcall hookCallback(int nCode, WPARAM wParam, LPARAM lParam);

public:
	static void setHook();
};