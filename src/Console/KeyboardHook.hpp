#pragma once
#include "GameConsole.hpp"
#include <windows.h>
#include <detours.h>

class KeyboardHook
{
private:
	static WNDPROC realProc;
	static LRESULT __stdcall hookedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static void setHook();
};