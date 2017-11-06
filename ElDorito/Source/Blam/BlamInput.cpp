#define _CRT_SECURE_NO_WARNINGS

#include "BlamInput.hpp"

#include <cwchar>
#include <cstring>
#include "../Utils/Macros.hpp"

namespace Blam::Input
{
	uint8_t GetKeyTicks(KeyCode key, InputType type)
	{
		typedef uint8_t(*EngineGetKeyTicksPtr)(KeyCode, InputType);
		auto EngineGetKeyTicks = reinterpret_cast<EngineGetKeyTicksPtr>(0x511B60);
		return EngineGetKeyTicks(key, type);
	}

	uint16_t GetKeyMs(KeyCode key, InputType type)
	{
		typedef uint8_t(*EngineGetKeyMsPtr)(KeyCode, InputType);
		auto EngineGetKeyMs = reinterpret_cast<EngineGetKeyMsPtr>(0x511CE0);
		return EngineGetKeyMs(key, type);
	}

	uint8_t GetMouseButtonTicks(MouseButton button, InputType type)
	{
		typedef uint8_t(*EngineGetMouseButtonTicksPtr)(MouseButton, InputType);
		auto EngineGetMousButtonTicks = reinterpret_cast<EngineGetMouseButtonTicksPtr>(0x00511DF0);
		return EngineGetMousButtonTicks(button, type);
	}

	bool ReadKeyEvent(KeyEvent* result, InputType type)
	{
		typedef bool(*EngineReadKeyEventPtr)(KeyEvent*, InputType);
		auto EngineReadKeyEvent = reinterpret_cast<EngineReadKeyEventPtr>(0x5118C0);
		return EngineReadKeyEvent(result, type);
	}

	void BlockInput(InputType type, bool block)
	{
		typedef uint8_t(*EngineBlockInputPtr)(InputType, bool);
		auto EngineBlockInput = reinterpret_cast<EngineBlockInputPtr>(0x512530);
		EngineBlockInput(type, block);
	}

	bool GetBindings(int localPlayerIndex, BindingsTable *result)
	{
		if (localPlayerIndex < 0 || localPlayerIndex >= 4)
			return false;
		typedef void(*EngineGetBindingsPtr)(int localPlayerIndex, BindingsTable *result);
		auto EngineGetBindings = reinterpret_cast<EngineGetBindingsPtr>(0x60BE70);
		EngineGetBindings(localPlayerIndex, result);
		return true;
	}

	bool SetBindings(int localPlayerIndex, const BindingsTable &newBindings)
	{
		if (localPlayerIndex < 0 || localPlayerIndex >= 4)
			return false;
		typedef void(*EngineSetBindingsPtr)(int localPlayerIndex, const BindingsTable &newBindings);
		auto EngineSetBindings = reinterpret_cast<EngineSetBindingsPtr>(0x60D830);
		EngineSetBindings(localPlayerIndex, newBindings);
		return true;
	}

	ActionState* GetActionState(GameAction action)
	{
		if (action < 0 || action >= eGameAction_KeyboardMouseCount)
			return nullptr;
		auto actions = reinterpret_cast<ActionState*>(0x244D1F0);
		return &actions[action];
	}

	VirtualKeyboard::VirtualKeyboard(const char *file, int line, int arg2, int arg3, int arg4, int arg5, int maxLength, int arg7, int arg8)
	{
		Unknown4 = 0;
		State = eVirtualKeyboardStateClosed;
		Unknown8 = 0;
		UnknownC = 0;
		Unknown10 = 0;
		Unknown14 = 0;
		memset(Value, 0, sizeof(Value));
		memset(DefaultValue, 0, sizeof(DefaultValue));
		memset(Title, 0, sizeof(Title));
		memset(Description, 0, sizeof(Description));
		MaxLength = Utils::Clamp(maxLength, 0, 0x100);
	}

	int VirtualKeyboard::Stub0() { return 0; }
	int VirtualKeyboard::Stub4() { return 0; }
	int VirtualKeyboard::Stub8() { return 0; }
	int VirtualKeyboard::StubC() { return 0; }
	int VirtualKeyboard::Stub10() { return 0; }
	int VirtualKeyboard::Stub14() { return 0; }
	int VirtualKeyboard::Stub18() { return 0; }
	void VirtualKeyboard::Reset() { State = eVirtualKeyboardStateClosed; }
	int VirtualKeyboard::Stub20() { return 0; }

	void VirtualKeyboard::SetTitle(const wchar_t *newTitle)
	{
		const auto TitleLength = sizeof(Title) / sizeof(Title[0]);
		wcsncpy(Title, newTitle, TitleLength - 1);
		Title[TitleLength - 1] = '\0';
	}

	void VirtualKeyboard::SetDescription(const wchar_t *newDescription)
	{
		const auto DescriptionLength = sizeof(Description) / sizeof(Description[0]);
		wcsncpy(Description, newDescription, DescriptionLength - 1);
		Description[DescriptionLength - 1] = '\0';
	}

	void VirtualKeyboard::SetValue(const wchar_t *newValue)
	{
		const auto ValueLength = sizeof(Value) / sizeof(Value[0]);
		wcsncpy(Value, newValue, ValueLength - 1);
		Value[ValueLength - 1] = '\0';
	}

	void VirtualKeyboard::SetDefaultValue(const wchar_t *newDefaultValue)
	{
		const auto DefaultValueLength = sizeof(DefaultValue) / sizeof(DefaultValue[0]);
		wcsncpy(DefaultValue, newDefaultValue, DefaultValueLength - 1);
		DefaultValue[DefaultValueLength - 1] = '\0';
	}

	void VirtualKeyboard::Start()
	{
		State = eVirtualKeyboardStateBusy1;
	}

	void VirtualKeyboard::Finish()
	{
		State = eVirtualKeyboardStateFinished;
	}
}
