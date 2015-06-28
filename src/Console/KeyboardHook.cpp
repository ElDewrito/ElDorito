#include "KeyboardHook.hpp"

WNDPROC KeyboardHook::realProc = 0;
HHOOK KeyboardHook::ourHookedFunctionPtr = 0;

LRESULT __stdcall KeyboardHook::hookedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto& console = GameConsole::Instance();
	if ((console.showChat || console.showConsole) && message == WM_INPUT)
	{
		UINT uiSize = 40; // sizeof(RAWINPUT)
		static unsigned char lpb[40];
		RAWINPUT* rwInput;

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &uiSize, sizeof(RAWINPUTHEADER)) != -1)
		{
			rwInput = (RAWINPUT*)lpb;

			if (rwInput->header.dwType == RIM_TYPEKEYBOARD && (rwInput->data.keyboard.Flags == RI_KEY_MAKE || rwInput->data.keyboard.Flags == RI_KEY_E0))
			{
				console.virtualKeyCallBack(rwInput->data.keyboard.VKey);
			}
			else if (rwInput->header.dwType == RIM_TYPEMOUSE)
			{
				console.mouseCallBack(rwInput->data.mouse);
			}
		}
	}

	return CallWindowProc(realProc, hWnd, message, wParam, lParam);
}

LRESULT __stdcall KeyboardHook::hookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode > 0)
	{
		return CallNextHookEx(ourHookedFunctionPtr, nCode, wParam, lParam);
	}
	else if (nCode == HC_ACTION)
	{
		if (HK_IS_PRESSED)
		{
			GameConsole::Instance().virtualKeyCallBack(wParam);
		}
	}

	return CallNextHookEx(ourHookedFunctionPtr, nCode, wParam, lParam);
}

void KeyboardHook::setHook()
{
	realProc = (WNDPROC)SetWindowLongPtr(*((HWND*) 0x199C014), GWL_WNDPROC, (LONG_PTR)KeyboardHook::hookedWndProc);

	if (!(ourHookedFunctionPtr = SetWindowsHookEx(WH_KEYBOARD, KeyboardHook::hookCallback, GetModuleHandle(0), GetWindowThreadProcessId(*((HWND*)0x199C014), 0))))
	{
		OutputDebugString("Failed to install keyboard hook!");
	}
}