#include "KeyboardHook.hpp"

WNDPROC KeyboardHook::realProc = 0;

LRESULT __stdcall KeyboardHook::hookedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GameConsole::Instance().isConsoleShown() && message == WM_INPUT)
	{
		UINT uiSize = 40; // sizeof(RAWINPUT)
		static unsigned char lpb[40];
		RAWINPUT* rwInput;

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &uiSize, sizeof(RAWINPUTHEADER)) != -1)
		{
			rwInput = (RAWINPUT*)lpb;

			if (rwInput->header.dwType == RIM_TYPEKEYBOARD && (rwInput->data.keyboard.Flags == RI_KEY_MAKE || rwInput->data.keyboard.Flags == RI_KEY_E0))
			{
				GameConsole::Instance().virtualKeyCallBack(rwInput->data.keyboard.VKey);
			}
			else if (rwInput->header.dwType == RIM_TYPEMOUSE)
			{
				GameConsole::Instance().mouseCallBack(rwInput->data.mouse);
			}
		}
	}

	return CallWindowProc(realProc, hWnd, message, wParam, lParam);
}

void KeyboardHook::setHook()
{
	realProc = (WNDPROC)SetWindowLongPtr(*((HWND*) 0x199C014), GWL_WNDPROC, (LONG_PTR)KeyboardHook::hookedWndProc);
}