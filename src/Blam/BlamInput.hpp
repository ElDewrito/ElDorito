#pragma once

#include <cstdint>

namespace Blam
{
	namespace Input
	{
		enum UiButtonCode : uint32_t
		{
			eUiButtonCodeA = 0,
			eUiButtonCodeB,
			eUiButtonCodeX,
			eUiButtonCodeY,
			eUiButtonCodeRB,
			eUiButtonCodeLB,
			eUiButtonCodeLT,
			eUiButtonCodeRT,
			eUiButtonCodeDpadUp,
			eUiButtonCodeDpadDown,
			eUiButtonCodeDpadLeft,
			eUiButtonCodeDpadRight,
			eUiButtonCodeStart,
			eUiButtonCodeBack,
			eUiButtonCodeLS,
			eUiButtonCodeRS,
			eUiButtonCodeEmulatedInput,
			eUiButtonCodeUnk1,
			eUiButtonCodeLeft, // analog/arrow left
			eUiButtonCodeRight, // analog/arrow right
			eUiButtonCodeUp, // analog/arrow up
			eUiButtonCodeDown, // analog/arrow down
			eUiButtonCodeLSHorizontal = 0x1A,
			eUiButtonCodeLSVertical,
			eUiButtonCodeRSHorizontal,
			eUiButtonCodeRSVertical,
		};

		enum ControllerButton : uint8_t
		{
			eControllerButtonLeftTrigger,
			eControllerButtonRightTrigger,
			eControllerButtonDpadUp,
			eControllerButtonDpadDown,
			eControllerButtonDpadLeft,
			eControllerButtonDpadRight,
			eControllerButtonStart,
			eControllerButtonSelect,
			eControllerButtonLeftStick,
			eControllerButtonRightStick,
			eControllerButtonA,
			eControllerButtonB,
			eControllerButtonX,
			eControllerButtonY,
			eControllerButtonLeftBumper,
			eControllerButtonRightBumper,

			eControllerButton_Count,
			eControllerButton_None = 0xFF, // An invalid controller button (for use in unset bindings)
		};

		enum KeyCode : uint16_t
		{
			eKeyCodeEscape,
			eKeyCodeF1,
			eKeyCodeF2,
			eKeyCodeF3,
			eKeyCodeF4,
			eKeyCodeF5,
			eKeyCodeF6,
			eKeyCodeF7,
			eKeyCodeF8,
			eKeyCodeF9,
			eKeyCodeF10,
			eKeyCodeF11,
			eKeyCodeF12,
			eKeyCodePrintScreen,
			eKeyCodeF14,
			eKeyCodeF15,
			eKeyCodeTilde, // VK_OEM_3
			eKeyCode1,
			eKeyCode2,
			eKeyCode3,
			eKeyCode4,
			eKeyCode5,
			eKeyCode6,
			eKeyCode7,
			eKeyCode8,
			eKeyCode9,
			eKeyCode0,
			eKeyCodeMinus,
			eKeyCodePlus,
			eKeyCodeBack,
			eKeyCodeTab,
			eKeyCodeQ,
			eKeyCodeW,
			eKeyCodeE,
			eKeyCodeR,
			eKeyCodeT,
			eKeyCodeY,
			eKeyCodeU,
			eKeyCodeI,
			eKeyCodeO,
			eKeyCodeP,
			eKeyCodeLBracket, // VK_OEM_4
			eKeyCodeRBracket, // VK_OEM_6
			eKeyCodePipe, // VK_OEM_5
			eKeyCodeCapital,
			eKeyCodeA,
			eKeyCodeS,
			eKeyCodeD,
			eKeyCodeF,
			eKeyCodeG,
			eKeyCodeH,
			eKeyCodeJ,
			eKeyCodeK,
			eKeyCodeL,
			eKeyCodeColon, // VK_OEM_1
			eKeyCodeQuote, // VK_OEM_7
			eKeyCodeEnter,
			eKeyCodeLShift,
			eKeyCodeZ,
			eKeyCodeX,
			eKeyCodeC,
			eKeyCodeV,
			eKeyCodeB,
			eKeyCodeN,
			eKeyCodeM,
			eKeyCodeComma,
			eKeyCodePeriod,
			eKeyCodeQuestion, // VK_OEM_2
			eKeyCodeRShift,
			eKeyCodeLControl,
			eKeyCodeUnused46, // Left Windows key, but will always fail
			eKeyCodeLAlt,
			eKeyCodeSpace,
			eKeyCodeRAlt,
			eKeyCodeUnused4A, // Right Windows key, but will always fail
			eKeyCodeApps,
			eKeyCodeRcontrol,
			eKeyCodeUp,
			eKeyCodeDown,
			eKeyCodeLeft,
			eKeyCodeRight,
			eKeyCodeInsert,
			eKeyCodeHome,
			eKeyCodePageUp,
			eKeyCodeDelete,
			eKeyCodeEnd,
			eKeyCodePageDown,
			eKeyCodeNumLock,
			eKeyCodeDivide,
			eKeyCodeMultiply,
			eKeyCodeNumpad0,
			eKeyCodeNumpad1,
			eKeyCodeNumpad2,
			eKeyCodeNumpad3,
			eKeyCodeNumpad4,
			eKeyCodeNumpad5,
			eKeyCodeNumpad6,
			eKeyCodeNumpad7,
			eKeyCodeNumpad8,
			eKeyCodeNumpad9,
			eKeyCodeSubtract,
			eKeyCodeAdd,
			eKeyCodeNumpadEnter,
			eKeyCodeDecimal,
			eKeyCodeUnused68,
			eKeyCodeShift,
			eKeyCodeCtrl,
			eKeyCodeUnused6B, // Windows key, but will always fail
			eKeyCodeAlt,

			eKeyCode_Count,
			eKeyCode_None = 0xFF, // An invalid key code (for use in unset bindings)
		};

		enum MouseButton : uint8_t
		{
			eMouseButtonLeft,
			eMouseButtonMiddle,
			eMouseButtonRight,
			eMouseButton4,
			eMouseButton5,

			// Not sure what these 3 are...they aren't buttons and can't be bound to anything
			eMouseButtonUnk5,
			eMouseButtonUnk6,
			eMouseButtonUnk7,

			eMouseButtonWheelUp,
			eMouseButtonWheelDown,

			eMouseButton_Count,
			eMouseButton_None = 0xFF, // An invalid mouse button (for use in unset bindings)
		};

		enum InputType : uint32_t
		{
			eInputTypeUi,      // ABXY, mouse clicks, etc.
			eInputTypeGame,    // All in-game actions (including camera)
			                   //   Disabled when the pause menu is open
			eInputTypeSpecial, // Escape, tab, menu navigation
		};

		enum KeyEventModifiers : uint8_t
		{
			eKeyEventModifiersShift = 1 << 0,
			eKeyEventModifiersCtrl  = 1 << 1,
			eKeyEventModifiersalt   = 1 << 2,
		};

		enum KeyEventType : uint32_t
		{
			eKeyEventTypeDown, // A key was pressed.
			eKeyEventTypeUp,   // A key was released.
			eKeyEventTypeChar  // A character was typed.
		};

		struct KeyEvent
		{
			KeyEventModifiers Modifiers; // Bitfield of modifier keys that are down
			KeyEventType Type;           // Event type
			KeyCode Code;                // The key code, or -1 if unavailable
			char16_t Char;               // For eKeyEventTypeChar events, the character that was typed, or -1 if unavailable
			bool PreviousState;          // If true, the key was down before this event happened
		};
		static_assert(sizeof(KeyEvent) == 0x10, "Invalid KeyEvent size");

		enum GameAction : uint8_t
		{
			// These actions mirror the ControllerButton enum

			eGameActionUiLeftTrigger,
			eGameActionUiRightTrigger,
			eGameActionUiUp,
			eGameActionUiDown,
			eGameActionUiLeft,
			eGameActionUiRight,
			eGameActionUiStart,
			eGameActionUiSelect,
			eGameActionUiLeftStick,
			eGameActionUiRightStick,
			eGameActionUiA,
			eGameActionUiB,
			eGameActionUiX,
			eGameActionUiY,
			eGameActionUiLeftBumper,
			eGameActionUiRightBumper,

			// In-game actions

			eGameActionJump,
			eGameActionSwitchGrenades,
			eGameActionSwitchWeapons,
			eGameActionUnk19,
			eGameActionReloadRight,
			eGameActionUse,
			eGameActionReloadLeft,
			eGameActionPickUpLeft,
			eGameActionMelee,
			eGameActionThrowGrenade,
			eGameActionFireRight,
			eGameActionFireLeft,
			eGameActionMeleeFire, // Fires a weapon with a trigger bound to the melee key
			eGameActionCrouch,
			eGameActionZoom,
			eGameActionUnk31,
			eGameActionUnk32,
			eGameActionSprint,
			eGameActionUnk34,
			eGameActionUnk35,
			eGameActionUnk36,
			eGameActionUnk37,
			eGameActionUnk38,
			eGameActionGeneralChat,
			eGameActionTeamChat,
			eGameActionUnk41,
			eGameActionUnk42,
			eGameActionUnk43,
			eGameActionUseConsumable1,
			eGameActionUseConsumable2,
			eGameActionUseConsumable3,
			eGameActionUseConsumable4,
			eGameActionVehicleBoost,
			eGameActionVehicleDive,
			eGameActionVehicleRaise,
			eGameActionVehicleAccelerate,
			eGameActionVehicleBrake,
			eGameActionVehicleFire,
			eGameActionVehicleAltFire,
			eGameActionVehicleExit,
			eGameActionUnk56,
			eGameActionUnk57,
			eGameActionUnk58,

			eGameAction_ControllerCount = 59,

			// These actions CANNOT be bound to controller buttons or else you
			// will overflow the controller bindings array! (Also, it seems
			// that mouse bindings will ignore these, even though there's room
			// for them.)

			eGameActionMoveForward = 59,
			eGameActionMoveBack,
			eGameActionMoveLeft,
			eGameActionMoveRight,

			eGameAction_KeyboardMouseCount = 63
		};

		struct BindingsTable
		{
			float Unknown0; // Default = 120
			float Unknown4; // Default = 60
			ControllerButton ControllerButtons[eGameAction_ControllerCount];
			bool ControllerHoldButtons[eGameAction_ControllerCount]; // true if a button needs to be held down? just a guess
			KeyCode PrimaryKeys[eGameAction_KeyboardMouseCount];
			KeyCode SecondaryKeys[eGameAction_KeyboardMouseCount];
			MouseButton PrimaryMouseButtons[eGameAction_KeyboardMouseCount];
			MouseButton SecondaryMouseButtons[eGameAction_KeyboardMouseCount];
			uint32_t Unknown1F8;
			uint32_t Unknown1FC;
			float Unknown200; // Default = .8
			float Unknown204; // Default = 1
		};
		static_assert(sizeof(BindingsTable) == 0x208, "Invalid BindingsTable size");

		struct BindingsPreferences
		{
			KeyCode PrimaryKeys[eGameAction_KeyboardMouseCount];
			MouseButton PrimaryMouseButtons[eGameAction_KeyboardMouseCount];
			KeyCode SecondaryKeys[eGameAction_KeyboardMouseCount];
			MouseButton SecondaryMouseButtons[eGameAction_KeyboardMouseCount];
		};
		static_assert(sizeof(BindingsPreferences) == 0x17C, "Invalid BindingsPreferences size");

		enum ActionStateFlags : uint8_t
		{
			eActionStateFlags_None = 0,
			eActionStateFlagsHandled = 1 << 0, // Action has been handled and should be ignored
			// TODO: Other flags?
		};

		struct ActionState
		{
			uint16_t Msec;     // Number of milliseconds the action has been down for
			uint8_t Ticks;     // Number of ticks the action has been down for
			uint8_t Flags;     // See the ActionStateFlags enum
			float Time;        // Number of seconds the action has been down for
			uint32_t Unknown8;
		};
		static_assert(sizeof(ActionState) == 0xC, "Invalid ActionState size");

		struct ConfigurableAction
		{
			const char *Name;        // "settings_" followed by the string_id
			int DefaultPrimaryKey;   // KeyCode, (MouseButton + 1) * 0x100, or -1
			int DefaultSecondaryKey; // KeyCode, (MouseButton + 1) * 0x100, or -1
			int Action;              // GameAction
		};
		static_assert(sizeof(ConfigurableAction) == 0x10, "Invalid ConfigurableAction size");

		// Gets the number of ticks that a key has been held down for.
		// Will always be nonzero if the key is down.
		uint8_t GetKeyTicks(KeyCode key, InputType type);

		// Gets the number of milliseconds that a key has been held down for.
		// Will always be nonzero if the key is down.
		uint16_t GetKeyMs(KeyCode key, InputType type);

		// Reads a raw keyboard input event. Returns false if nothing is
		// available. You should call this in a loop to ensure that you process
		// all available events. NOTE THAT THIS IS ONLY GUARANTEED TO WORK
		// AFTER WINDOWS MESSAGES HAVE BEEN PUMPED IN THE UPDATE CYCLE. ALSO,
		// THIS WILL NOT WORK IF UI INPUT IS DISABLED, REGARDLESS OF THE INPUT
		// TYPE YOU SPECIFY.
		bool ReadKeyEvent(KeyEvent* result, InputType type);

		// Blocks or unblocks an input type.
		void BlockInput(InputType type, bool block);

		// Gets the input bindings table for a local player.
		// Halo Online only uses index 0, but there are 4 total.
		// This will fail if the index is out-of-range.
		bool GetBindings(int localPlayerIndex, BindingsTable *result);

		// Updates the input bindings table for a local player.
		// Halo Online only uses index 0, but there are 4 total.
		// This will fail if the index is out-of range.
		bool SetBindings(int localPlayerIndex, const BindingsTable &newBindings);

		// Gets the state of the button bound to an action.
		// Will return null if the action is invalid.
		ActionState* GetActionState(GameAction action);
	}
}