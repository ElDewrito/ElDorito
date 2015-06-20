#include "KeyboardHook.h"

KeyboardHook* KeyboardHook::thisInstance;

LRESULT __stdcall KeyboardHook::hookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode > 0)
	{
		return CallNextHookEx(thisInstance->ourHookedFunctionPtr, nCode, wParam, lParam);
	}
	else if (nCode == HC_ACTION)
	{
		if (HK_IS_PRESSED)
		{
			thisInstance->gameConsoleInstance->virtualKeyCallBack(wParam);
		}
	}

	return CallNextHookEx(thisInstance->ourHookedFunctionPtr, nCode, wParam, lParam);
}

void KeyboardHook::setHook()
{
	if (!(ourHookedFunctionPtr = SetWindowsHookEx(WH_KEYBOARD, KeyboardHook::hookCallback, GetModuleHandle(0), GetWindowThreadProcessId(*((HWND*)0x199C014), 0))))
	{
		OutputDebugString("Failed to install keyboard hook!");
	}
}

KeyboardHook::KeyboardHook(GameConsole* gameConsoleInstance) : gameConsoleInstance(gameConsoleInstance)
{
	thisInstance = this;
	setHook();
}