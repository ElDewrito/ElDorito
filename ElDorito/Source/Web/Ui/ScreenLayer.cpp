#include "ScreenLayer.hpp"
#include "../WebRenderer.hpp"
#include "../WebRendererSchemeHandler.hpp"
#include "../../Modules/ModuleServer.hpp"
#include "../../Patches/Input.hpp"
#include "../../Patches/Core.hpp"
#include "../../Blam/BlamInput.hpp"
#include "../../Blam/BlamTime.hpp"
#include "../../Patches/Ui.hpp"
#include "../../ElDorito.hpp"
#include <Windows.h>
#include <shellapi.h>
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../CommandMap.hpp"
#include "../../Modules/ModuleSettings.hpp"
#include "../../Modules/ModuleInput.hpp"

using namespace Blam::Input;
using namespace Anvil::Client::Rendering;

namespace
{
	const auto kVariableUpdateNotifyDebounceSeconds = 0.25f;

	HHOOK MessageHook;
	bool InputCaptured = false;
	bool PointerCaptured = false;
	uint32_t lastVariableUpdateTicks = 0;
	std::vector<const Modules::Command*> lastVariableUpdates;

	void WindowCreated(HWND window);
	LRESULT CALLBACK GetMsgHook(int code, WPARAM wParam, LPARAM lParam);

	void ShutdownRenderer();
	void OnGameInputUpdated();
	void OnUIInputUpdated();
	void NotifyVariablesUpdated();
	void OnVariableUpdate(const Modules::Command *comamnd);

	void QuickBlockInput();
	void QuickUnblockInput();

	class WebOverlayInputContext : public Patches::Input::InputContext
	{
	public:
		void Activated() override { }
		void Deactivated() override { }

		bool GameInputTick() override
		{
			OnGameInputUpdated();
			if (!WebRenderer::GetInstance()->IsRendering())
				InputCaptured = false;
			return InputCaptured || PointerCaptured;
		}

		bool UiInputTick() override
		{
			if (!WebRenderer::GetInstance()->IsRendering())
				InputCaptured = false;
			if(InputCaptured)
				OnUIInputUpdated();

			if (!InputCaptured && PointerCaptured)
			{
				GetActionState(static_cast<GameAction>(eGameActionFireLeft))->Flags |= eActionStateFlagsHandled;
				GetActionState(static_cast<GameAction>(eGameActionFireRight))->Flags |= eActionStateFlagsHandled;
			}
			
			return InputCaptured || PointerCaptured;
		}

		void InputBlock() override
		{
			if (!InputCaptured && PointerCaptured)
			{
				QuickUnblockInput();
				return;
			}
			QuickBlockInput();
		}

		void InputUnblock() override
		{
			QuickUnblockInput();
		}
	};

	void QuickBlockInput()
	{
		memset(reinterpret_cast<bool*>(0x238DBEB), 1, eInputType_Count);
	}

	void QuickUnblockInput()
	{
		memset(reinterpret_cast<bool*>(0x238DBEB), 0, eInputType_Count);
	}
}

namespace Web::Ui::ScreenLayer
{
	void Init()
	{
		Patches::Ui::OnCreateWindow(WindowCreated);
		Patches::Core::OnShutdown(ShutdownRenderer);
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
		Modules::CommandMap::Instance().OnVariableUpdate(OnVariableUpdate);
	}

	void Tick()
	{
		WebRenderer::GetInstance()->Update();
		NotifyVariablesUpdated();
	}

	void Resize()
	{
		auto *windowResolution = reinterpret_cast<int *>(0x19106E4);
		WebRenderer::GetInstance()->Resize(windowResolution[0], windowResolution[1]);
	}

	void Show(bool show)
	{
		if (ElDorito::Instance().IsDedicated())
			return;
		auto webRenderer = WebRenderer::GetInstance();
		if (show == webRenderer->IsRendering())
			return;
		webRenderer->ShowRenderer(show, true);
	}

	void Show(const std::string &screenId, const std::string &data)
	{
		if (ElDorito::Instance().IsDedicated())
			return;
		// ui.requestScreen(id, data)
		auto js = "if (window.ui) ui.requestScreen('" + screenId + "', " + data + ");";
		WebRenderer::GetInstance()->ExecuteJavascript(js);
	}

	void Hide(const std::string &screenId)
	{
		if (ElDorito::Instance().IsDedicated())
			return;
		// ui.hideScreen(id)
		auto js = "if (window.ui) ui.hideScreen('" + screenId + "');";
		WebRenderer::GetInstance()->ExecuteJavascript(js);
	}

	void Notify(const std::string &event, const std::string &data, bool broadcast)
	{
		if (ElDorito::Instance().IsDedicated())
			return;
		// ui.notify(event, data, broadcast, fromDew)
		auto js = "if (window.ui) ui.notify('" + event + "'," + data + "," + (broadcast ? "true" : "false") + ",true);";
		WebRenderer::GetInstance()->ExecuteJavascript(js);
	}

	void NotifyScreen(const std::string &screenId, const std::string &event, const std::string &data)
	{
		if (ElDorito::Instance().IsDedicated())
			return;
		// ui.notifyScreen(screen, event, data)
		auto js = "if (window.ui) ui.notifyScreen('"+screenId+"', '" + event + "'," + data + ");";
		WebRenderer::GetInstance()->ExecuteJavascript(js);
	}

	void CaptureInput(bool capture, bool pointerCapture)
	{
		if ((InputCaptured == capture && PointerCaptured == pointerCapture) || (capture && !WebRenderer::GetInstance()->IsRendering()))
			return;

		InputCaptured = capture;
		PointerCaptured = pointerCapture;
		if (capture || pointerCapture)
		{
			// Override game input
			auto inputContext = std::make_shared<WebOverlayInputContext>();
			inputContext->allowHandlers = PointerCaptured && !InputCaptured;
			Patches::Input::PushContext(inputContext);
		}
	}

	void ShowAlert(const std::string &title, const std::string &body, AlertIcon icon)
	{
		const char* iconStrs[] = { "none", "download", "pause", "upload", "checkbox" };

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();
		writer.Key("icon");
		writer.String(iconStrs[int(icon)]);
		writer.Key("title");
		writer.String(title.c_str());
		writer.Key("body");
		writer.String(body.c_str());
		writer.EndObject();

		Ui::ScreenLayer::Show("alert", buffer.GetString());
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
			if (msg->lParam & (1 << 29) && msg->wParam == VK_F4) // alt-f4
			{
				Modules::CommandMap::Instance().ExecuteCommand("Game.Exit");
				return;
			}
			if (msg->wParam == VK_SNAPSHOT) // print-scr
			{
				Modules::CommandMap::Instance().ExecuteCommand("Game.TakeScreenshot");
				return;
			}
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
		if ((!InputCaptured && !PointerCaptured) || !webRenderer->IsRendering())
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
			if(InputCaptured)
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

	// Debug keys
	void OnGameInputUpdated()
	{
		if (!ElDorito::Instance().IsWebDebuggingEnabled())
			return;

		auto webRenderer = WebRenderer::GetInstance();

		// If F5 is pressed, reload all screens
		if (GetKeyTicks(eKeyCodeF5, eInputTypeUi) == 1)
		{
			WebRendererSchemeHandler::ClearCache(); // hax
			webRenderer->ExecuteJavascript("ui.reload();");
		}

		// If F6 is pressed, reload everything and ignore the cache
		if (GetKeyTicks(eKeyCodeF6, eInputTypeUi) == 1)
		{
			webRenderer->Reload(true);
			Web::Ui::ScreenLayer::CaptureInput(false, false);
		}

		// If F7 is pressed, open the remote debugger in Chrome
		if (GetKeyTicks(eKeyCodeF7, eInputTypeUi) == 1)
			ShellExecute(nullptr, nullptr, "chrome.exe", "http://localhost:8884/", nullptr, SW_SHOWNORMAL);
	}


	void OnUIInputUpdated()
	{
		auto &moduleSettings = Modules::ModuleSettings::Instance();
		auto &moduleInput = Modules::ModuleInput::Instance();

		struct ControllerAxes { int16_t LeftX, LeftY, RightX, RightY; };
		auto& controllerAxes = *(ControllerAxes*)(0x0244D1F0 + 0x2F4);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();
		writer.Key("gameTicks");
		writer.Double(Blam::Time::GetGameTicks());
		writer.Key("secondsPerTick");
		writer.Double(Blam::Time::GetSecondsPerTick());
		writer.Key("AxisLeftX");
		writer.Double(controllerAxes.LeftX / 32768.0f);
		writer.Key("AxisLeftY");
		writer.Double(controllerAxes.LeftY / 32768.0f);
		writer.Key("AxisRightX");
		writer.Double(controllerAxes.RightX / 32768.0f);
		writer.Key("AxisRightY");
		writer.Double((controllerAxes.RightY / 32768.0f) * (moduleInput.VarControllerInvertY->ValueInt ? -1.0f : 1.0f));
		writer.Key("LeftTrigger");
		writer.Int(GetActionState(eGameActionUiLeftTrigger)->Ticks);
		writer.Key("RightTrigger");
		writer.Int(GetActionState(eGameActionUiRightTrigger)->Ticks);
		writer.Key("Up");
		writer.Int(GetActionState(eGameActionUiUp)->Ticks);
		writer.Key("Down");
		writer.Int(GetActionState(eGameActionUiDown)->Ticks);
		writer.Key("Left");
		writer.Int(GetActionState(eGameActionUiLeft)->Ticks);
		writer.Key("Right");
		writer.Int(GetActionState(eGameActionUiRight)->Ticks);
		writer.Key("Start");
		writer.Int(GetActionState(eGameActionUiStart)->Ticks);
		writer.Key("Select");
		writer.Int(GetActionState(eGameActionUiSelect)->Ticks);
		writer.Key("LeftStick");
		writer.Int(GetActionState(eGameActionUiLeftStick)->Ticks);
		writer.Key("RightStick");
		writer.Int(GetActionState(eGameActionUiRightStick)->Ticks);
		writer.Key("A");
		writer.Int(GetActionState(eGameActionUiA)->Ticks);
		writer.Key("B");
		writer.Int(GetActionState(eGameActionUiB)->Ticks);
		writer.Key("X");
		writer.Int(GetActionState(eGameActionUiX)->Ticks);
		writer.Key("Y");
		writer.Int(GetActionState(eGameActionUiY)->Ticks);
		writer.Key("LeftBumper");
		writer.Int(GetActionState(eGameActionUiLeftBumper)->Ticks);
		writer.Key("RightBumper");
		writer.Int(GetActionState(eGameActionUiRightBumper)->Ticks);
		writer.EndObject();

		auto js = "if (window.ui) ui.sendControllerInput(" + std::string(buffer.GetString()) + ");";
		WebRenderer::GetInstance()->ExecuteJavascript(js);

		controllerAxes.LeftX = 0;
		controllerAxes.LeftY = 0;
		controllerAxes.RightX = 0;
		controllerAxes.RightY = 0;
		for (auto i = 0; i < eGameActionJump; i++)
		{
			auto action = GetActionState(static_cast<GameAction>(i));
			action->Flags |= eActionStateFlagsHandled;
		}
	}

	void OnVariableUpdate(const Modules::Command *command)
	{
		lastVariableUpdates.push_back(command);
	}

	void NotifyVariablesUpdated()
	{
		if (lastVariableUpdates.size() < 1)
			return;
		if (Blam::Time::TicksToSeconds(lastVariableUpdateTicks++) < kVariableUpdateNotifyDebounceSeconds)
			return;

		lastVariableUpdateTicks = 0;

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartArray();
		while (lastVariableUpdates.size() > 0)
		{
			auto &command = *lastVariableUpdates.back();
			lastVariableUpdates.pop_back();

			writer.StartObject();
			writer.Key("type");
			writer.Int(command.Type);
			writer.Key("module");
			writer.String(command.ModuleName.c_str());
			writer.Key("name");
			writer.String(command.Name.c_str());
			writer.Key("shortName");
			writer.String(command.ShortName.c_str());
			writer.Key("value");
			switch (command.Type)
			{
			case Modules::eCommandTypeVariableInt:
				writer.Int(command.ValueInt);
				break;
			case Modules::eCommandTypeVariableInt64:
				writer.Int64(command.ValueInt64);
				break;
			case Modules::eCommandTypeVariableFloat:
				writer.Double(command.ValueFloat);
				break;
			case Modules::eCommandTypeVariableString:
				writer.String(command.ValueString.c_str());
				break;
			default:
				writer.Null();
				break;
			}
			writer.EndObject();
		}
		writer.EndArray();

		Web::Ui::ScreenLayer::Notify("variable_update", buffer.GetString(), true);
	}
}
