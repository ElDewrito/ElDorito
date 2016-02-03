#include "Input.hpp"
#include <stack>

#include "../Patch.hpp"
#include "../Blam/BlamInput.hpp"

namespace
{
	using namespace Patches::Input;

	void UpdateInputHook();
	void ProcessUiInputHook();
	void QuickUpdateUiInputHook();
	void KeyTestHook();
	void SetBindingsHook(int localPlayerIndex, Blam::Input::BindingsTable *bindings);
	void DualWieldBindingsHandler(int localPlayerIndex);

	std::stack<std::shared_ptr<InputContext>> contextStack;
	std::vector<DefaultInputHandler> defaultHandlers;
	bool contextDone = false;

	std::vector<BindingsUpdatedHandler> bindingsUpdatedHandlers;
}

namespace Patches
{
	namespace Input
	{
		void ApplyAll()
		{
			Hook(0x1129A0, UpdateInputHook).Apply();
			Hook(0x105CBA, ProcessUiInputHook, HookFlags::IsCall).Apply();
			Hook(0x106417, QuickUpdateUiInputHook, HookFlags::IsCall).Apply();
			Hook(0x111B66, KeyTestHook, HookFlags::IsCall).Apply();
			Hook(0x111CE6, KeyTestHook, HookFlags::IsCall).Apply();
			Hook(0x6A237F, SetBindingsHook, HookFlags::IsCall).Apply();
			Hook(0x10D139, SetBindingsHook, HookFlags::IsCall).Apply();
			RegisterBindingsUpdatedHandler(DualWieldBindingsHandler);
		}

		void PushContext(std::shared_ptr<InputContext> context)
		{
			if (!contextStack.empty())
				contextStack.top()->InputDeactivated();
			contextStack.push(context);
			context->InputActivated();
		}

		void RegisterDefaultInputHandler(DefaultInputHandler func)
		{
			defaultHandlers.push_back(func);
		}

		void RegisterBindingsUpdatedHandler(BindingsUpdatedHandler func)
		{
			bindingsUpdatedHandlers.push_back(func);
		}
	}
}

namespace
{
	void PopContext()
	{
		contextStack.top()->InputDeactivated();
		contextStack.pop();
		if (!contextStack.empty())
			contextStack.top()->InputActivated();
	}

	void UpdateInputHook()
	{
		// If the current context is done, pop it off
		if (contextDone)
		{
			PopContext();
			contextDone = false;
		}

		if (!contextStack.empty())
		{
			// Tick the active context
			if (!contextStack.top()->GameInputTick())
				contextDone = true;
		}
		else
		{
			// Run default handlers
			for (auto &&handler : defaultHandlers)
				handler();
		}
	}

	void UiInputTick()
	{
		// Tick the active context
		if (!contextStack.empty() && !contextStack.top()->UiInputTick())
			contextDone = true;
	}

	void ProcessUiInputHook()
	{
		// Pump Windows messages (replaced function)
		typedef void(*PumpMessagesPtr)();
		auto PumpMessages = reinterpret_cast<PumpMessagesPtr>(0x508170);
		PumpMessages();

		UiInputTick();
	}

	void QuickUpdateUiInputHook()
	{
		// Quick pump Windows messages (replaced function)
		typedef void(*QuickPumpMessagesPtr)();
		auto QuickPumpMessages = reinterpret_cast<QuickPumpMessagesPtr>(0x42E940);
		QuickPumpMessages();

		UiInputTick();
	}

	// Returns true if a key should be blocked.
	bool KeyTestHookImpl(Blam::Input::InputType type)
	{
		return (type == Blam::Input::eInputTypeGame || type == Blam::Input::eInputTypeSpecial) && !contextStack.empty();
	}

	// Hook for the engine's "get key ticks" and "get key ms" functions. If the
	// zero flag is not set when this returns, the key will be blocked.
	__declspec(naked) void KeyTestHook()
	{
		__asm
		{
			// Replaced code (checks if the input type is blocked)
			cmp byte ptr ds:0x238DBEB[eax], 0
			jnz done

			push eax
			call KeyTestHookImpl
			add esp, 4
			test eax, eax

		done:
			// The replaced instruction is 7 bytes long, so get the return
			// address and add 2 without affecting the flags
			pop eax
			lea eax, [eax + 2]
			jmp eax
		}
	}

	void SetBindingsHook(int localPlayerIndex, Blam::Input::BindingsTable *bindings)
	{
		typedef void(*SetBindingsPtr)(int localPlayerIndex, Blam::Input::BindingsTable *bindings);
		auto SetBindings = reinterpret_cast<SetBindingsPtr>(0x60D830);
		SetBindings(localPlayerIndex, bindings);

		for (auto &&handler : bindingsUpdatedHandlers)
			handler(localPlayerIndex);
	}

	void DualWieldBindingsHandler(int localPlayerIndex)
	{
		auto bindings = Blam::Input::GetBindings(localPlayerIndex);

		// Unbind all actions bound to Q (haxhaxhax)
		for (auto i = 0; i < Blam::Input::eInputAction_KeyboardMouseCount; i++)
		{
			if (bindings->SecondaryKeys[i] == Blam::Input::eKeyCodesQ)
				bindings->SecondaryKeys[i] = Blam::Input::eKeyCodes_None;
			if (bindings->PrimaryKeys[i] == Blam::Input::eKeyCodesQ)
			{
				bindings->PrimaryKeys[i] = bindings->SecondaryKeys[i];
				bindings->SecondaryKeys[i] = Blam::Input::eKeyCodes_None;
			}
		}

		// TODO: Pull these values from variables, or set up our own complete binding system

		// Pick up left = Q
		bindings->PrimaryKeys[Blam::Input::eInputActionPickUpLeft] = Blam::Input::eKeyCodesQ;
		bindings->SecondaryKeys[Blam::Input::eInputActionPickUpLeft] = Blam::Input::eKeyCodes_None;
		bindings->PrimaryMouseButtons[Blam::Input::eInputActionPickUpLeft] = Blam::Input::eMouseButtons_None;
		bindings->SecondaryMouseButtons[Blam::Input::eInputActionPickUpLeft] = Blam::Input::eMouseButtons_None;

		// Reload left = Q
		bindings->PrimaryKeys[Blam::Input::eInputActionReloadLeft] = Blam::Input::eKeyCodesQ;
		bindings->SecondaryKeys[Blam::Input::eInputActionReloadLeft] = Blam::Input::eKeyCodes_None;
		bindings->PrimaryMouseButtons[Blam::Input::eInputActionReloadLeft] = Blam::Input::eMouseButtons_None;
		bindings->SecondaryMouseButtons[Blam::Input::eInputActionReloadLeft] = Blam::Input::eMouseButtons_None;
		
		// Fire left = RMB
		bindings->PrimaryKeys[Blam::Input::eInputActionFireLeft] = Blam::Input::eKeyCodes_None;
		bindings->SecondaryKeys[Blam::Input::eInputActionFireLeft] = Blam::Input::eKeyCodes_None;
		bindings->PrimaryMouseButtons[Blam::Input::eInputActionFireLeft] = Blam::Input::eMouseButtonsRight;
		bindings->SecondaryMouseButtons[Blam::Input::eInputActionFireLeft] = Blam::Input::eMouseButtons_None;
	}
}