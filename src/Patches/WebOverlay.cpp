#include "WebOverlay.hpp"
#include "../Web/WebRenderer.hpp"
#include "../Patch.hpp"
#include "Input.hpp"
#include "Core.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Patches/Network.hpp"
#include <Windows.h>

using namespace Blam::Input;
using namespace Anvil::Client::Rendering;

namespace
{
	HHOOK MessageHook;
	uint16_t PingId;
	bool InputCaptured = false;

	HWND CreateGameWindowHook();
	LRESULT CALLBACK GetMsgHook(int code, WPARAM wParam, LPARAM lParam);
	void PongReceived(const Blam::Network::NetworkAddress &from, uint32_t timestamp, uint16_t id, uint32_t latency);

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

namespace Patches
{
	namespace WebOverlay
	{
		void ApplyAll()
		{
			Hook(0x622057, CreateGameWindowHook, HookFlags::IsCall).Apply();
			Core::RegisterShutdownCallback(ShutdownRenderer);
			Network::OnPong(PongReceived);
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

		void ShowScreen(const std::string &id, const std::string &data)
		{
			WebRenderer::GetInstance()->ExecuteJavascript("ui.requestScreen('" + id + "', " + data + ");");
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

		uint16_t GetPingId()
		{
			return PingId;
		}
	}
}

namespace
{
	void ShutdownRenderer()
	{
		WebRenderer::GetInstance()->Shutdown();
	}

	HWND CreateGameWindowHook()
	{
		typedef HWND(*CreateGameWindowPtr)();
		auto CreateGameWindow = reinterpret_cast<CreateGameWindowPtr>(0xA223F0);
		auto hwnd = CreateGameWindow();
		if (!hwnd)
			return nullptr;

		Patches::WebOverlay::Resize();

		// Install window hooks
		auto threadId = GetWindowThreadProcessId(hwnd, nullptr);
		MessageHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgHook, nullptr, threadId);

		return hwnd;
	}

	void HandleMouseMessage(LPMSG msg)
	{
		auto webRenderer = WebRenderer::GetInstance();

		auto mousePos = msg->pt;
		ScreenToClient(msg->hwnd, &mousePos);

		switch (msg->message)
		{
		case WM_MOUSEMOVE:
			webRenderer->UpdateMouse(mousePos.x, mousePos.y);
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			webRenderer->SendMouseEvent(mousePos.x, mousePos.y, MBT_LEFT, msg->message == WM_LBUTTONDOWN);
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			webRenderer->SendMouseEvent(mousePos.x, mousePos.y, MBT_MIDDLE, msg->message == WM_MBUTTONDOWN);
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			webRenderer->SendMouseEvent(mousePos.x, mousePos.y, MBT_RIGHT, msg->message == WM_RBUTTONDOWN);
			break;
		case WM_MOUSEWHEEL:
			webRenderer->SendMouseWheelEvent(mousePos.x, mousePos.y, 0, GET_WHEEL_DELTA_WPARAM(msg->wParam));
			break;
		case WM_MOUSEHWHEEL:
			webRenderer->SendMouseWheelEvent(mousePos.x, mousePos.y, GET_WHEEL_DELTA_WPARAM(msg->wParam), 0);
			break;
		}
	}

	// Keyboard functions taken from cefclient

	bool IsKeyDown(WPARAM wparam)
	{
		return (GetKeyState(wparam) & 0x8000) != 0;
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

	void PongReceived(const Blam::Network::NetworkAddress &from, uint32_t timestamp, uint16_t id, uint32_t latency)
	{
		if (id != PingId)
			return;
		std::string data = "{";
		data += "address: '" + from.ToString() + "'";
		data += ",latency: " + std::to_string(latency);
		data += ",timestamp: " + std::to_string(timestamp);
		data += "}";
		WebRenderer::GetInstance()->ExecuteJavascript("ui.notify('pong'," + data + ",false,true);");
	}
}