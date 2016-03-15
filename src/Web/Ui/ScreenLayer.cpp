#include "ScreenLayer.hpp"
#include "../WebRenderer.hpp"
#include "../../Patches/Input.hpp"
#include "../../Patches/Core.hpp"
#include "../../Blam/BlamInput.hpp"
#include "../../Patches/Ui.hpp"
#include <Windows.h>

using namespace Blam::Input;
using namespace Anvil::Client::Rendering;

namespace
{
	HHOOK MessageHook;
	bool InputCaptured = false;

	void WindowCreated(HWND window);
	LRESULT CALLBACK GetMsgHook(int code, WPARAM wParam, LPARAM lParam);

	void ShutdownRenderer();

	class WebOverlayInputContext : public Patches::Input::InputContext
	{
	public:
		void Activated() override { }
		void Deactivated() override { }

		bool GameInputTick() override
		{
			if (!WebRenderer::GetInstance()->IsRendering())
				InputCaptured = false;
			return InputCaptured;
		}

		bool UiInputTick() override
		{
			if (!WebRenderer::GetInstance()->IsRendering())
				InputCaptured = false;
			return InputCaptured;
		}
	};
}

namespace Web
{
	namespace Ui
	{
		namespace ScreenLayer
		{
			void Init()
			{
				Patches::Ui::OnCreateWindow(WindowCreated);
				Patches::Core::OnShutdown(ShutdownRenderer);
			}

			void Tick()
			{
				WebRenderer::GetInstance()->Update();
			}

			void Resize()
			{
				auto hwnd = *reinterpret_cast<HWND*>(0x199C014);
				RECT clientRect;
				if (GetClientRect(hwnd, &clientRect))
					WebRenderer::GetInstance()->Resize(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
			}

			void Show(bool show)
			{
				auto webRenderer = WebRenderer::GetInstance();
				if (show == webRenderer->IsRendering())
					return;
				webRenderer->ShowRenderer(show, true);
			}

			void Show(const std::string &screenId, const std::string &data)
			{
				// ui.requestScreen(id, data)
				auto js = "ui.requestScreen('" + screenId + "', " + data + ");";
				WebRenderer::GetInstance()->ExecuteJavascript(js);
			}

			void Hide(const std::string &screenId)
			{
				// ui.hideScreen(id)
				auto js = "ui.hideScreen('" + screenId + "');";
				WebRenderer::GetInstance()->ExecuteJavascript(js);
			}

			void Notify(const std::string &event, const std::string &data, bool broadcast)
			{
				// ui.notify(event, data, broadcast, fromDew)
				auto js = "ui.notify('" + event + "'," + data + "," + (broadcast ? "true" : "false") + ",true);";
				WebRenderer::GetInstance()->ExecuteJavascript(js);
			}

			void CaptureInput(bool capture)
			{
				if (InputCaptured == capture || (capture && !WebRenderer::GetInstance()->IsRendering()))
					return;
				InputCaptured = capture;
				if (capture)
				{
					// Override game input
					auto inputContext = std::make_shared<WebOverlayInputContext>();
					Patches::Input::PushContext(inputContext);
				}
			}
		}
	}
}

namespace
{
	void WindowCreated(HWND window)
	{
		Web::Ui::ScreenLayer::Resize();

		// Install window hooks
		auto threadId = GetWindowThreadProcessId(window, nullptr);
		MessageHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgHook, nullptr, threadId);
	}

	void ShutdownRenderer()
	{
		WebRenderer::GetInstance()->Shutdown();
	}

	// Keyboard and mouse functions based off of cefclient

	bool IsKeyDown(WPARAM wparam)
	{
		return (GetKeyState(wparam) & 0x8000) != 0;
	}

	int GetCefMouseModifiers(WPARAM wParam)
	{
		auto modifiers = 0;
		if (wParam & MK_CONTROL)
			modifiers |= EVENTFLAG_CONTROL_DOWN;
		if (wParam & MK_SHIFT)
			modifiers |= EVENTFLAG_SHIFT_DOWN;
		if (IsKeyDown(VK_MENU))
			modifiers |= EVENTFLAG_ALT_DOWN;
		if (wParam & MK_LBUTTON)
			modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
		if (wParam & MK_MBUTTON)
			modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
		if (wParam & MK_RBUTTON)
			modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

		// Low bit set from GetKeyState indicates "toggled".
		if (GetKeyState(VK_NUMLOCK) & 1)
			modifiers |= EVENTFLAG_NUM_LOCK_ON;
		if (GetKeyState(VK_CAPITAL) & 1)
			modifiers |= EVENTFLAG_CAPS_LOCK_ON;
		return modifiers;
	}

	int GetLastClickCount(LPMSG msg)
	{
		static int lastClickX, lastClickY;
		static int lastClickCount;
		static UINT lastClickButton;
		static LONG lastClickTime;
		auto cancelPreviousClick = false;

		if (msg->message == WM_LBUTTONDOWN || msg->message == WM_RBUTTONDOWN ||
			msg->message == WM_MBUTTONDOWN || msg->message == WM_MOUSEMOVE ||
			msg->message == WM_MOUSELEAVE)
		{
			auto currentTime = msg->time;
			auto x = msg->pt.x;
			auto y = msg->pt.y;
			cancelPreviousClick =
				(abs(lastClickX - x) > (GetSystemMetrics(SM_CXDOUBLECLK) / 2))
				|| (abs(lastClickY - y) > (GetSystemMetrics(SM_CYDOUBLECLK) / 2))
				|| ((currentTime - lastClickTime) > GetDoubleClickTime());
			if (cancelPreviousClick && (msg->message == WM_MOUSEMOVE || msg->message == WM_MOUSELEAVE))
			{
				lastClickCount = 0;
				lastClickX = 0;
				lastClickY = 0;
				lastClickTime = 0;
			}
		}
		if (msg->message == WM_LBUTTONDOWN || msg->message == WM_MBUTTONDOWN || msg->message == WM_RBUTTONDOWN)
		{
			if (!cancelPreviousClick && msg->message == lastClickButton)
			{
				lastClickCount++;
			}
			else
			{
				lastClickCount = 1;
				lastClickX = msg->pt.x;
				lastClickY = msg->pt.y;
			}
			lastClickTime = msg->time;
			lastClickButton = msg->message;
		}
		return lastClickCount;
	}

	void HandleMouseMessage(LPMSG msg)
	{
		auto webRenderer = WebRenderer::GetInstance();

		auto mousePos = msg->pt;
		ScreenToClient(msg->hwnd, &mousePos);

		CefMouseEvent mouse;
		mouse.x = mousePos.x;
		mouse.y = mousePos.y;
		mouse.modifiers = GetCefMouseModifiers(msg->wParam);
		auto lastClickCount = GetLastClickCount(msg);

		switch (msg->message)
		{
		case WM_MOUSEMOVE:
			webRenderer->SendMouseMoveEvent(mouse, false);
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			webRenderer->SendMouseClickEvent(mouse, MBT_LEFT, msg->message == WM_LBUTTONUP, lastClickCount);
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			webRenderer->SendMouseClickEvent(mouse, MBT_MIDDLE, msg->message == WM_MBUTTONUP, lastClickCount);
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			webRenderer->SendMouseClickEvent(mouse, MBT_RIGHT, msg->message == WM_RBUTTONUP, lastClickCount);
			break;
		case WM_MOUSEWHEEL:
			webRenderer->SendMouseWheelEvent(mouse, 0, GET_WHEEL_DELTA_WPARAM(msg->wParam));
			break;
		case WM_MOUSEHWHEEL:
			webRenderer->SendMouseWheelEvent(mouse, GET_WHEEL_DELTA_WPARAM(msg->wParam), 0);
			break;
		}
	}

	int GetCefKeyboardModifiers(WPARAM wParam, LPARAM lParam)
	{
		auto modifiers = 0;
		if (IsKeyDown(VK_SHIFT))
			modifiers |= EVENTFLAG_SHIFT_DOWN;
		if (IsKeyDown(VK_CONTROL))
			modifiers |= EVENTFLAG_CONTROL_DOWN;
		if (IsKeyDown(VK_MENU))
			modifiers |= EVENTFLAG_ALT_DOWN;

		// Low bit set from GetKeyState indicates "toggled".
		if (GetKeyState(VK_NUMLOCK) & 1)
			modifiers |= EVENTFLAG_NUM_LOCK_ON;
		if (GetKeyState(VK_CAPITAL) & 1)
			modifiers |= EVENTFLAG_CAPS_LOCK_ON;

		switch (wParam) {
		case VK_RETURN:
			if ((lParam >> 16) & KF_EXTENDED)
				modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			if (!((lParam >> 16) & KF_EXTENDED))
				modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_NUMLOCK:
		case VK_NUMPAD0:
		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
		case VK_DIVIDE:
		case VK_MULTIPLY:
		case VK_SUBTRACT:
		case VK_ADD:
		case VK_DECIMAL:
		case VK_CLEAR:
			modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_SHIFT:
			if (IsKeyDown(VK_LSHIFT))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (IsKeyDown(VK_RSHIFT))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_CONTROL:
			if (IsKeyDown(VK_LCONTROL))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (IsKeyDown(VK_RCONTROL))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_MENU:
			if (IsKeyDown(VK_LMENU))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (IsKeyDown(VK_RMENU))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_LWIN:
			modifiers |= EVENTFLAG_IS_LEFT;
			break;
		case VK_RWIN:
			modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		}
		return modifiers;
	}

	void HandleKeyMessage(LPMSG msg)
	{
		CefKeyEvent key;
		key.windows_key_code = msg->wParam;
		key.native_key_code = msg->lParam;
		key.is_system_key = (msg->message == WM_SYSCHAR || msg->message == WM_SYSKEYDOWN || msg->message == WM_SYSKEYUP);
		key.modifiers = GetCefKeyboardModifiers(msg->wParam, msg->lParam);

		switch (msg->message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			key.type = KEYEVENT_RAWKEYDOWN;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			key.type = KEYEVENT_KEYUP;
			break;
		case WM_CHAR:
		case WM_SYSCHAR:
			key.type = KEYEVENT_CHAR;
			break;
		}

		WebRenderer::GetInstance()->SendKeyEvent(key);
	}

	void GetMsgHookImpl(int code, WPARAM wParam, LPARAM lParam)
	{
		// Ignore messages not sent to a window
		auto msg = reinterpret_cast<LPMSG>(lParam);
		if (!msg->hwnd)
			return;

		// Ignore messages if the web renderer isn't active or if input isn't captured
		auto webRenderer = WebRenderer::GetInstance();
		if (!InputCaptured || !webRenderer->IsRendering())
			return;

		switch (msg->message)
		{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			HandleMouseMessage(msg);
			break;

		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			HandleKeyMessage(msg);
			break;
		}
	}

	LRESULT CALLBACK GetMsgHook(int code, WPARAM wParam, LPARAM lParam)
	{
		if (code >= HC_ACTION)
			GetMsgHookImpl(code, wParam, lParam);

		return CallNextHookEx(MessageHook, code, wParam, lParam);
	}
}