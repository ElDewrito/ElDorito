#define _USE_MATH_DEFINES
#include "Input.hpp"
#include <stack>
#include <cmath>

#include "../Patch.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Modules/ModuleInput.hpp"
#include "../Console.hpp"
#include "../ElDorito.hpp"

using namespace Patches::Input;
using namespace Blam::Input;

namespace
{
	void UpdateInputHook();
	void ProcessUiInputHook();
	void QuickUpdateUiInputHook();
	void BlockInputHook(InputType type, bool blocked);
	void InitBindingsHook(BindingsTable *bindings);
	BindingsPreferences* PreferencesGetKeyBindingsHook(BindingsPreferences *result);
	void PreferencesSetKeyBindingsHook(BindingsPreferences newBindings);
	void GetDefaultBindingsHook(int type, BindingsTable *result);
	void GetKeyboardActionTypeHook();
	void ProcessKeyBindingsHook(const BindingsTable &bindings, ActionState *actions);
	void ProcessMouseBindingsHook(const BindingsTable &bindings, ActionState *actions);
	void UpdateUiControllerInputHook(int a0);
	char GetControllerStateHook(int dwUserIndex, int a2, void *a3);
	DWORD SetControllerStateHook(int dwUserIndex, int motorSpeed, char disable);
	void SetControllerVibrationHook(int controllerIndex, uint16_t leftMotorSpeed, uint16_t rightMotorSpeed);
	void LocalPlayerInputHook(int localPlayerIndex, uint32_t playerIndex, int a3, int a4, int a5, uint8_t* state);
	void null_stub() {};

	// Block/unblock input without acquiring or de-acquiring the mouse
	void QuickBlockInput();
	void QuickUnblockInput();

	void UpdateForgeMonitorBindings();

	std::stack<std::shared_ptr<InputContext>> contextStack;
	std::vector<DefaultInputHandler> defaultHandlers;
	std::vector<MenuUIInputHandler> uiHandlers;
	bool contextDone = false;
	bool blockStates[eInputType_Count];

	std::vector<ConfigurableAction> settings;

	extern InputType actionInputTypes[eGameAction_KeyboardMouseCount];
	bool queuedGameAction[eGameAction_KeyboardMouseCount] = {};

	BindingsTable s_ForgeMonitorModeBindings;
	uint32_t s_ControllerVibrationTestTicks = 0;
	uint32_t bindingsNextUpdateTicks = -1;
}

namespace Patches::Input
{
	void ApplyAll()
	{
		Hook(0x1129A0, UpdateInputHook).Apply();
		Hook(0x105CBA, ProcessUiInputHook, HookFlags::IsCall).Apply();
		Hook(0x106417, QuickUpdateUiInputHook, HookFlags::IsCall).Apply();
		Hook(0x234238, BlockInputHook, HookFlags::IsCall).Apply();
		Hook(0x20BF00, InitBindingsHook).Apply();
		Hook(0x10AB40, PreferencesGetKeyBindingsHook).Apply();
		Hook(0x10D040, PreferencesSetKeyBindingsHook).Apply();
		Hook(0x20C040, GetDefaultBindingsHook).Apply();
		Hook(0x20C4F6, GetKeyboardActionTypeHook).Apply();
		Hook(0x1128FB, GetControllerStateHook, HookFlags::IsCall).Apply();
		Hook(0x11298B, SetControllerStateHook, HookFlags::IsCall).Apply();
		Hook(0x1124F0, SetControllerVibrationHook).Apply();
		Hook(0x105C58, null_stub, HookFlags::IsCall).Apply(); // remove the hardcoded binding of screenshot to printscreen
		Patch::NopFill(Pointer::Base(0x6A225B), 2); // Prevent the game from forcing certain binds on load
		Patch(0x6940E7, { 0x90, 0xE9 }).Apply(); // Disable custom UI input code
		Hook(0x695012, UpdateUiControllerInputHook, HookFlags::IsCall).Apply();
		Patch::NopFill(Pointer::Base(0x112613), 2); // Never clip the cursor to the window boundaries

		// Fix a bug in the keyboard input routine that screws up UI keys.
		// If an action has the "handled" flag set and has a secondary key
		// bound to it, then the tick count gets reset to 0 because the
		// secondary key won't be down and the action gets spammed.
		//
		// To fix it, we have to nop out every place where the flag is
		// checked, disable the code which updates the flag, and then
		// update it ourselves only after every key has been checked.
		uint32_t keyboardFixPointers[] = { 0x20C529, 0x20C558, 0x20C591, 0x20C5C2, 0x20C5FA, 0x20C63F };
		for (auto pointer : keyboardFixPointers)
			Patch::NopFill(Pointer::Base(pointer), 2);
		Patch(0x20C69F, { 0xEB }).Apply();
		Hook(0x20D980, ProcessKeyBindingsHook, HookFlags::IsCall).Apply();
		Hook(0x20D99B, ProcessMouseBindingsHook, HookFlags::IsCall).Apply();
		Hook(0x1D4C66, LocalPlayerInputHook, HookFlags::IsCall).Apply();
	}

	bool QueueGameAction(int index)
	{
		if (index > -1 && index < eGameAction_KeyboardMouseCount)
		{
			queuedGameAction[index] = true;
			return true;
		}
		return false;
	}

	void PushContext(std::shared_ptr<InputContext> context)
	{
		if (contextStack.empty())
		{
			// Block all input, unacquiring the mouse in the process
			for (auto i = 0; i < eInputType_Count; i++)
				BlockInput(static_cast<InputType>(i), true);
		}
		else
		{
			// Deactivate the current context
			contextStack.top()->Deactivated();
		}

		// Push and activate the new context
		contextStack.push(context);
		context->Activated();
	}

	void RegisterDefaultInputHandler(DefaultInputHandler func)
	{
		defaultHandlers.push_back(func);
	}

	void RegisterMenuUIInputHandler(MenuUIInputHandler func)
	{
		uiHandlers.push_back(func);
	}

	void SetKeyboardSettingsMenu(
		const std::vector<ConfigurableAction> &infantrySettings,
		const std::vector<ConfigurableAction> &vehicleSettings)
	{
		// The settings array needs to have infantry settings followed by
		// vehicle settings due to assumptions that the EXE makes
		settings.clear();
		settings.insert(settings.end(), infantrySettings.begin(), infantrySettings.end());
		settings.insert(settings.end(), vehicleSettings.begin(), vehicleSettings.end());

		// Patch the exe to point to the new menus
		auto infantryCount = infantrySettings.size();
		auto vehicleCount = vehicleSettings.size();
		uint32_t infantryPointers[] = { 0x398573, 0x3993CF, 0x39A42F, 0x39B4B0 };
		uint32_t vehiclePointers[] = { 0x39856D, 0x3993CA, 0x39A63A, 0x39A645, 0x39B4AB };
		uint32_t endPointers[] = { 0x39A84A };
		uint32_t infantryCountPointers[] = { 0x39857B, 0x3993BC, 0x39B495 };
		uint32_t vehicleCountPointers[] = { 0x398580, 0x3993C1, 0x39B49A };
		for (auto pointer : infantryPointers)
			Pointer::Base(pointer).Write<ConfigurableAction*>(&settings[0]);
		for (auto pointer : vehiclePointers)
			Pointer::Base(pointer).Write<ConfigurableAction*>(&settings[0] + infantryCount);
		for (auto pointer : endPointers)
			Pointer::Base(pointer).Write<ConfigurableAction*>(&settings[0] + settings.size());
		for (auto pointer : infantryCountPointers)
			Pointer::Base(pointer).Write<int>(static_cast<int>(infantryCount));
		for (auto pointer : vehicleCountPointers)
			Pointer::Base(pointer).Write<int>(static_cast<int>(vehicleCount));
	}

	void TestControllerVibration(float durationSeconds)
	{
		s_ControllerVibrationTestTicks = Blam::Time::SecondsToTicks(durationSeconds);
	}

	void InvalidateBindings()
	{
		// bindings are applied individually one per-tick, so we need to debounce updates
		bindingsNextUpdateTicks = 100;
	}
}

namespace
{
	void PopContext()
	{
		contextStack.top()->Deactivated();
		contextStack.pop();
		if (!contextStack.empty())
		{
			// Activate the previous context
			contextStack.top()->Activated();
		}
		else
		{
			// Restore the game's input block states
			for (auto i = 0; i < eInputType_Count; i++)
				BlockInput(static_cast<InputType>(i), blockStates[i]);
		}
	}

	void UpdateInputHook()
	{
		// If the current context is done, pop it off
		if (contextDone)
		{
			PopContext();
			contextDone = false;
		}

		bool allowHandlers = false;

		if (!contextStack.empty())
		{
			// Tick the active context
			contextStack.top()->InputUnblock();
			if (!contextStack.top()->GameInputTick())
				contextDone = true;
			contextStack.top()->InputBlock();
			allowHandlers = contextStack.top()->allowHandlers;
		}
		else
		{
			// Run default handlers
			for (auto &&handler : defaultHandlers)
				handler();
		}

		if (allowHandlers)
		{
			// Run default handlers
			for (auto &&handler : defaultHandlers)
				handler();
		}

		if (s_ControllerVibrationTestTicks > 0)
			s_ControllerVibrationTestTicks--;

		if (bindingsNextUpdateTicks != -1 && --bindingsNextUpdateTicks == 0)
		{
			bindingsNextUpdateTicks = -1;
			UpdateForgeMonitorBindings();
		}
	}

	void UiInputTick()
	{
		if (contextStack.empty())
			return;

		// Tick the active context
		contextStack.top()->InputUnblock();
		if (!contextStack.top()->UiInputTick())
			contextDone = true;
		contextStack.top()->InputBlock();
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

	void BlockInputHook(InputType type, bool blocked)
	{
		// If a context isn't active, then block input normally,
		// otherwise save the value for when the contexts are done
		if (contextStack.empty())
			BlockInput(type, blocked);
		blockStates[type] = blocked;
	}

	void UpdateForgeMonitorBindings()
	{
		// TODO: move these into ModuleInput and possibly add them to the config
		s_ForgeMonitorModeBindings = *Modules::ModuleInput::GetBindings();

		// prevent moving while using rotation snap
		s_ForgeMonitorModeBindings.SecondaryKeys[eGameActionMoveLeft] = eKeyCode_None;
		s_ForgeMonitorModeBindings.SecondaryKeys[eGameActionMoveRight] = eKeyCode_None;
		s_ForgeMonitorModeBindings.SecondaryKeys[eGameActionMoveForward] = eKeyCode_None;
		s_ForgeMonitorModeBindings.SecondaryKeys[eGameActionMoveBack] = eKeyCode_None;
		s_ForgeMonitorModeBindings.PrimaryKeys[eGameActionUiB] = eKeyCodeC; // Clone
	}

	// Hook to initialize bindings with ModuleInput's values
	void InitBindingsHook(BindingsTable *bindings)
	{
		*bindings = *Modules::ModuleInput::GetBindings();
		bindingsNextUpdateTicks = 100;
	}

	// Hook to redirect keybind preference reads to ModuleInput
	BindingsPreferences* PreferencesGetKeyBindingsHook(BindingsPreferences *result)
	{
		auto bindings = Modules::ModuleInput::GetBindings();
		memcpy(result->PrimaryKeys, bindings->PrimaryKeys, sizeof(result->PrimaryKeys));
		memcpy(result->PrimaryMouseButtons, bindings->PrimaryMouseButtons, sizeof(result->PrimaryMouseButtons));
		memcpy(result->SecondaryKeys, bindings->SecondaryKeys, sizeof(result->SecondaryKeys));
		memcpy(result->SecondaryMouseButtons, bindings->SecondaryMouseButtons, sizeof(result->SecondaryMouseButtons));
		return result;
	}

	// Hook to redirect keybind preference writes to ModuleInput
	void PreferencesSetKeyBindingsHook(BindingsPreferences newBindings)
	{
		auto bindings = Modules::ModuleInput::GetBindings();
		memcpy(bindings->PrimaryKeys, newBindings.PrimaryKeys, sizeof(bindings->PrimaryKeys));
		memcpy(bindings->PrimaryMouseButtons, newBindings.PrimaryMouseButtons, sizeof(bindings->PrimaryMouseButtons));
		memcpy(bindings->SecondaryKeys, newBindings.SecondaryKeys, sizeof(bindings->SecondaryKeys));
		memcpy(bindings->SecondaryMouseButtons, newBindings.SecondaryMouseButtons, sizeof(bindings->SecondaryMouseButtons));
		Modules::ModuleInput::UpdateBindings();
		Modules::CommandMap::Instance().ExecuteCommand("WriteConfig");
	}

	// Hook to prevent the game from resetting keybindings when we don't want it to
	void GetDefaultBindingsHook(int type, BindingsTable *result)
	{
		*result = *Modules::ModuleInput::GetBindings();
	}

	// Hook to get keyboard action types from the actionInputTypes array
	// instead of using hardcoded values
	__declspec(naked) void GetKeyboardActionTypeHook()
	{
		__asm
		{
			// ecx has the action index
			// eax needs to contain the type on return
			// ecx needs to be 0 on return
			mov eax, actionInputTypes[ecx * 4]
			xor ecx, ecx
			push 0x60C51E
			ret
		}
	}

	bool InMonitorMode()
	{
		static auto Forge_GetEditorModeState = (bool(__cdecl *)(uint32_t playerIndex, uint32_t* heldObjectIndex, uint32_t* objectIndexUnderCrosshair))(0x0059A6F0);
		return Forge_GetEditorModeState(Blam::Players::GetLocalPlayer(0), nullptr, nullptr);
	}

	void ProcessKeyBindingsHook(const BindingsTable &bindings, ActionState *actions)
	{
		typedef void(*EngineProcessKeyBindingsPtr)(const BindingsTable &bindings, ActionState *actions);
		auto EngineProcessKeyBindings = reinterpret_cast<EngineProcessKeyBindingsPtr>(0x60C4A0);

		// if we're in monitor mode and no UI is open
		if(InMonitorMode() && !Pointer(0x05260F34)[0](0x3c).Read<int16_t>())
			EngineProcessKeyBindings(s_ForgeMonitorModeBindings, actions);
		else
			EngineProcessKeyBindings(bindings, actions);

		// Unset the "handled" flag for inactive actions
		for (auto i = 0; i < eGameAction_KeyboardMouseCount; i++)
		{
			if (actions[i].Ticks == 0)
				actions[i].Flags &= ~eActionStateFlagsHandled;
			if (queuedGameAction[i])
			{
				actions[i].Ticks = 1;
				queuedGameAction[i] = false;
			}
		}
	}

	void ProcessMouseBindingsHook(const BindingsTable &bindings, ActionState *actions)
	{
		static auto ProcessMouseBindings = (void(*)(const BindingsTable &bindings, ActionState *actions))(0x60CE70);
		if (InMonitorMode())
			ProcessMouseBindings(s_ForgeMonitorModeBindings, actions);
		else
			ProcessMouseBindings(bindings, actions);
	}

	void UpdateUiControllerInputHook(int a0)
	{
		typedef void(*UiUpdateControllerInputPtr)(int a0);
		auto UiUpdateControllerInput = reinterpret_cast<UiUpdateControllerInputPtr>(0xA93A50);
		typedef bool(*IsMainMenuPtr)();
		auto IsMainMenu = reinterpret_cast<IsMainMenuPtr>(0x531E90);
		typedef float(*UiGetTimeDeltaPtr)();
		auto UiGetTimeDelta = reinterpret_cast<UiGetTimeDeltaPtr>(0xA844E0);
		typedef void(*UpdateCharPlatformPtr)();
		auto UpdateCharPlatform = reinterpret_cast<UpdateCharPlatformPtr>(0xBB5F00);
		typedef void(*RotateCharPlatformPtr)(float timeDelta, float amount);
		auto RotateCharPlatform = reinterpret_cast<RotateCharPlatformPtr>(0xBB5DA0);

		UiUpdateControllerInput(a0);

		// Handle char_platform controls
		//
		// This spices things up a bit compared to the default implementation
		// by making the platform automatically rotate slowly until the player
		// manually rotates it.

		static auto firstRotate = true;
		if (!IsMainMenu())
		{
			// char_platform is only on the main menu
			firstRotate = true;
			return;
		}
		if (firstRotate)
		{
			// Rotate it to the left a bit initially so that you can see the
			// front of your Spartan at the start
			RotateCharPlatform(1.0f, -0.25f);
			firstRotate = false;
			return;
		}
		auto leftAction = GetActionState(eGameActionUiLeft);
		auto rightAction = GetActionState(eGameActionUiRight);
		auto rotateLeft = (!(leftAction->Flags & eActionStateFlagsHandled) && leftAction->Ticks != 0);
		auto rotateRight = (!(rightAction->Flags & eActionStateFlagsHandled) && rightAction->Ticks != 0);
		auto rotateAmount = static_cast<int>(rotateRight) - static_cast<int>(rotateLeft);
		if (rotateAmount)
		{
			RotateCharPlatform(UiGetTimeDelta(), rotateAmount * 1.0f);
		}
		else
		{
			UpdateCharPlatform();
		}

		//main menu ui handlers
		for (auto &&handler : uiHandlers)
			handler();
	}

	char GetControllerStateHook(int dwUserIndex, int a2, void *controllerState)
	{
		typedef char(*GetControllerStatePtr)(int dwUserIndex, int a2, void *controllerState);
		auto GetControllerState = reinterpret_cast<GetControllerStatePtr>(0x65EF60);
		auto val = GetControllerState(Modules::ModuleInput::Instance().VarInputControllerPort->ValueInt, a2, controllerState);

		//Invert right joystick
		if (Modules::ModuleInput::Instance().VarControllerInvertY->ValueInt)
		{
			auto rY = Pointer(controllerState)(0x3A).Read<short>();
			//Prevent an overflow
			if (rY == -32768)
				rY++;
			Pointer(controllerState)(0x3A).WriteFast<short>(rY * -1);
		}

		return val;
	}


	DWORD SetControllerStateHook(int dwUserIndex, int motorSpeed, char disable)
	{
		static auto SetControllerState = (DWORD(*)(int dwUserIndex, int motorSpeed, char disable))(0x65F220);
		return SetControllerState((dwUserIndex == 0) ? Modules::ModuleInput::Instance().VarInputControllerPort->ValueInt : dwUserIndex, motorSpeed, disable);
	}

	void SetControllerVibrationHook(int controllerIndex, uint16_t leftMotorSpeed, uint16_t rightMotorSpeed)
	{
		struct CONTROLLER_VIBRATION { uint16_t LeftMotorSpeed, RightMotorSpeed; };

		const auto& moduleInput = Modules::ModuleInput::Instance();
		auto intensity = moduleInput.VarControllerVibrationIntensity->ValueFloat;

		if (s_ControllerVibrationTestTicks > 0)
		{
			leftMotorSpeed = 65535;
			rightMotorSpeed = 65535;
		}

		leftMotorSpeed = static_cast<uint16_t>(leftMotorSpeed * intensity);
		rightMotorSpeed = static_cast<uint16_t>(rightMotorSpeed * intensity);

		auto& vibration = ((CONTROLLER_VIBRATION*)0x0238E840)[controllerIndex];
		vibration.LeftMotorSpeed = leftMotorSpeed;
		vibration.RightMotorSpeed = rightMotorSpeed;
	}

	bool IsDualWielding()
	{
		auto& objects = Blam::Objects::GetObjects();

		auto unitObjectIndex = ElDorito::GetMainTls(0xC4)[0](0x300).Read<uint32_t>();
		if (unitObjectIndex == -1)
			return false;

		auto unitObjectPtr = Pointer(objects.Get(unitObjectIndex))[0xC];
		if (!unitObjectPtr)
			return false;

		return unitObjectPtr(0x2CB).Read<uint8_t>() != 0xFF;
	}

	//Checks if two game actions have the same binding, and that same binding is down.
	//Eg, true if for parameters eGameActionPickUpLeft and eGameActionUseConsumable 1, if both are bound to Z and Z is down.
	//False if bindings are the same, but different inputs are down, or if bindings are different.
	bool HasEqualBindingsDown(GameAction action1, GameAction action2, InputType inputType)
	{
		auto isUsingController = *(bool*)0x0244DE98;
		auto& moduleInput = Modules::ModuleInput::Instance();
		auto bindings = moduleInput.GetBindings();

		//Controller's easy, so check that first.
		if (isUsingController)
		{
			auto action1State = GetActionState(action1);
			auto action2State = GetActionState(action2);

			if (action1State->Ticks != 0 && action2State->Ticks != 0)
				return true;
		}
		else
		{
			//Check keyboard first, with the most bindings it's most likely.

			//Find all keyboard/mouse bindings for the two actions
			KeyCode action1PrimaryKey = bindings->PrimaryKeys[action1];
			KeyCode action1SecondaryKey = bindings->SecondaryKeys[action1];

			KeyCode action2PrimaryKey = bindings->PrimaryKeys[action2];
			KeyCode action2SecondaryKey = bindings->SecondaryKeys[action2];

			//Find which bindings are active
			bool action1PrimaryKeyDown = GetKeyTicks(action1PrimaryKey, inputType) != 0;
			bool action1SecondaryKeyDown = GetKeyTicks(action1SecondaryKey, inputType) != 0;

			bool action2PrimaryKeyDown = GetKeyTicks(action2PrimaryKey, inputType) != 0;
			bool action2SecondaryKeyDown = GetKeyTicks(action2SecondaryKey, inputType) != 0;

			if (action1PrimaryKeyDown)
			{
				if (action1PrimaryKey == action2PrimaryKey && action2PrimaryKeyDown)
					return true;
				if (action1PrimaryKey == action1SecondaryKey && action1SecondaryKeyDown)
					return true;
			}
			if(action1SecondaryKeyDown)
			{
				if (action1SecondaryKey == action2PrimaryKey && action2PrimaryKeyDown)
					return true;
				if (action1SecondaryKey == action1SecondaryKey && action1SecondaryKeyDown)
					return true;
			}

			//Now check mouse bindings.

			MouseButton action1PrimaryMouseButton = bindings->PrimaryMouseButtons[action1];
			MouseButton action1SecondaryMouseButton = bindings->SecondaryMouseButtons[action1];

			MouseButton action2PrimaryMouseButton = bindings->PrimaryMouseButtons[action2];
			MouseButton action2SecondaryMouseButton = bindings->SecondaryMouseButtons[action2];

			bool action1PrimaryMouseButtonDown = GetMouseButtonTicks(action1PrimaryMouseButton, inputType) != 0;
			bool action1SecondaryMouseButtonDown = GetMouseButtonTicks(action1SecondaryMouseButton, inputType) != 0;

			bool action2PrimaryMouseButtonDown = GetMouseButtonTicks(action2PrimaryMouseButton, inputType) != 0;
			bool action2SecondaryMouseButtonDown = GetMouseButtonTicks(action2SecondaryMouseButton, inputType) != 0;

			if (action1PrimaryMouseButtonDown)
			{
				if (action1PrimaryMouseButton == action2PrimaryMouseButton && action2PrimaryMouseButton)
					return true;
				if (action1PrimaryMouseButton == action2SecondaryMouseButton && action2SecondaryMouseButton)
					return true;
			}
			if (action1SecondaryMouseButtonDown)
			{
				if (action1SecondaryMouseButton == action2PrimaryMouseButton && action2PrimaryMouseButton)
					return true;
				if (action1SecondaryMouseButton == action2SecondaryMouseButton && action2SecondaryMouseButton)
					return true;
			}
		}
		return false;
	}

	void LocalPlayerInputHook(int localPlayerIndex, uint32_t playerIndex, int a3, int a4, int a5, uint8_t* state)
	{
		struct ControlGlobalsAction
		{
			uint16_t Type;
			uint16_t Flags;
			uint32_t ObjectIndex;
			// ...
		};

		static auto LocalPlayerInputHook = (void(__cdecl*)(int localPlayerIndex, uint32_t playerIndex, int a3, int a4, int a5, uint8_t* state))(0x5D0C90);
		static auto GetPlayerControlsAction = (ControlGlobalsAction*(__cdecl*)(int playerMappingIndex))(0x5D0BD0);

		static bool s_SprintToggled = false;
		static bool s_ConsumablesLocked = false;

		auto& moduleInput = Modules::ModuleInput::Instance();
		auto bindings = moduleInput.GetBindings();

		auto isDualWielding = IsDualWielding();
		auto isUsingController = *(bool*)0x0244DE98;

		if (!isUsingController && isDualWielding)
		{
			auto fireLeftAction = GetActionState(eGameActionFireLeft);
			auto fireRightAction = GetActionState(eGameActionFireRight);

			if (fireLeftAction->Ticks != 0 || fireRightAction->Ticks != 0)
			{
				ActionState tmp = *fireLeftAction;
				*fireLeftAction = *fireRightAction;
				*fireRightAction = tmp;
			}
		}

		LocalPlayerInputHook(localPlayerIndex, playerIndex, a3, a4, a5, state);

		if (moduleInput.VarToggleSprint->ValueInt)
		{
			if (s_SprintToggled)
			{
				auto playerIndex = Blam::Players::GetLocalPlayer(localPlayerIndex);
				auto& players = Blam::Players::GetPlayers();
				auto player = players.Get(playerIndex);
				if (player)
				{
					auto ticksSprinting = Pointer(player)(0x2CE4).Read<uint16_t>();
					if (ticksSprinting == 0)
						s_SprintToggled = false;
				}
			}

			if (GetActionState(eGameActionSprint)->Ticks == 1)
				s_SprintToggled = !s_SprintToggled;

			if (s_SprintToggled)
				*(uint32_t *)(state + 0x18) |= 0x100u;
		}

		if (*(uint32_t*)(state + 0x18) & 0x10)
		{
			// prevent equipment from being used while picking up/swapping weapons when those actions are bound to the same button
			auto controlGlobalsAction = GetPlayerControlsAction(localPlayerIndex);
			if (controlGlobalsAction->Type == 1 && (controlGlobalsAction->Flags & 4 || controlGlobalsAction->Flags & 8)) // only if it's dual-wieldable
			{
				if (isUsingController)
					s_ConsumablesLocked = bindings->ControllerButtons[eGameActionUseConsumable1] == bindings->ControllerButtons[eGameActionPickUpLeft];
				else
					s_ConsumablesLocked = HasEqualBindingsDown(eGameActionUseConsumable1, eGameActionPickUpLeft, eInputTypeGame);
			}
		}
		else
		{
			s_ConsumablesLocked = false;
		}

		if (isDualWielding)
		{
			if(!s_ConsumablesLocked)
				s_ConsumablesLocked = HasEqualBindingsDown(eGameActionUseConsumable1, eGameActionReloadLeft, eInputTypeGame);

			if (bindings->ControllerButtons[eGameActionMelee] == bindings->ControllerButtons[eGameActionFireLeft])
				*(uint32_t *)(state + 0x18) &= ~0x20u;

			if (bindings->ControllerButtons[eGameActionThrowGrenade] == bindings->ControllerButtons[eGameActionFireLeft])
				*(uint32_t *)(state + 0x18) &= ~0x10000000u;
		}

		if (s_ConsumablesLocked)
			*(uint32_t *)(state + 0x18) &= ~0x10;
	}
}

namespace
{
	// These override the input type used to check each action, because the
	// defaults aren't very good and treat the UI actions as game input
	//
	// TODO: Use this with the controller input routine too!
	InputType actionInputTypes[eGameAction_KeyboardMouseCount] =
	{
		eInputTypeUi,      // eGameActionUiLeftTrigger
		eInputTypeUi,      // eGameActionUiRightTrigger
		eInputTypeUi,      // eGameActionUiUp
		eInputTypeUi,      // eGameActionUiDown
		eInputTypeUi,      // eGameActionUiLeft
		eInputTypeUi,      // eGameActionUiRight
		eInputTypeSpecial, // eGameActionUiStart
		eInputTypeSpecial, // eGameActionUiSelect
		eInputTypeUi,      // eGameActionUiLeftStick
		eInputTypeUi,      // eGameActionUiRightStick
		eInputTypeUi,      // eGameActionUiA
		eInputTypeUi,      // eGameActionUiB
		eInputTypeUi,      // eGameActionUiX
		eInputTypeUi,      // eGameActionUiY
		eInputTypeUi,      // eGameActionUiLeftBumper
		eInputTypeUi,      // eGameActionUiRightBumper
		eInputTypeGame,    // eGameActionJump
		eInputTypeGame,    // eGameActionSwitchGrenades
		eInputTypeGame,    // eGameActionSwitchWeapons
		eInputTypeGame,    // eGameActionUnk19
		eInputTypeGame,    // eGameActionReloadRight
		eInputTypeGame,    // eGameActionUse
		eInputTypeGame,    // eGameActionReloadLeft
		eInputTypeGame,    // eGameActionPickUpLeft
		eInputTypeGame,    // eGameActionMelee
		eInputTypeGame,    // eGameActionThrowGrenade
		eInputTypeGame,    // eGameActionFireRight
		eInputTypeGame,    // eGameActionFireLeft
		eInputTypeGame,    // eGameActionMeleeFire
		eInputTypeGame,    // eGameActionCrouch
		eInputTypeGame,    // eGameActionZoom
		eInputTypeGame,    // eGameActionUnk31
		eInputTypeGame,    // eGameActionUnk32
		eInputTypeGame,    // eGameActionSprint
		eInputTypeGame,    // eGameActionUnk34
		eInputTypeGame,    // eGameActionUnk35
		eInputTypeGame,    // eGameActionUnk36
		eInputTypeGame,    // eGameActionUnk37
		eInputTypeGame,    // eGameActionUnk38
		eInputTypeUi,      // eGameActionGeneralChat
		eInputTypeUi,      // eGameActionTeamChat
		eInputTypeSpecial, // eGameActionVoiceChat
		eInputTypeGame,    // eGameActionUnk42
		eInputTypeGame,    // eGameActionUnk43
		eInputTypeGame,    // eGameActionUseConsumable1
		eInputTypeGame,    // eGameActionUseConsumable2
		eInputTypeGame,    // eGameActionUseConsumable3
		eInputTypeGame,    // eGameActionUseConsumable4
		eInputTypeGame,    // eGameActionVehicleBoost
		eInputTypeGame,    // eGameActionVehicleDive
		eInputTypeGame,    // eGameActionVehicleRaise
		eInputTypeGame,    // eGameActionVehicleAccelerate
		eInputTypeGame,    // eGameActionVehicleBrake
		eInputTypeGame,    // eGameActionVehicleFire
		eInputTypeGame,    // eGameActionVehicleAltFire
		eInputTypeGame,    // eGameActionVehicleExit
		eInputTypeUi,      // eGameActionUnk56
		eInputTypeUi,      // eGameActionUnk57
		eInputTypeUi,      // eGameActionUnk58
		eInputTypeGame,    // eGameActionMoveForward
		eInputTypeGame,    // eGameActionMoveBack
		eInputTypeGame,    // eGameActionMoveLeft
		eInputTypeGame,    // eGameActionMoveRight
	};
}
