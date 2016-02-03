#pragma once

#include <cstdint>

namespace Blam
{
	namespace Input
	{
		enum ButtonCodes : uint32_t
		{
			eButtonCodesA = 0,
			eButtonCodesB,
			eButtonCodesX,
			eButtonCodesY,
			eButtonCodesRB,
			eButtonCodesLB,
			eButtonCodesLT,
			eButtonCodesRT,
			eButtonCodesDpadUp,
			eButtonCodesDpadDown,
			eButtonCodesDpadLeft,
			eButtonCodesDpadRight,
			eButtonCodesStart,
			eButtonCodesBack,
			eButtonCodesLS,
			eButtonCodesRS,
			eButtonCodesEmulatedInput,
			eButtonCodesUnk1,
			eButtonCodesLeft, // analog/arrow left
			eButtonCodesRight, // analog/arrow right
			eButtonCodesUp, // analog/arrow up
			eButtonCodesDown, // analog/arrow down
			eButtonCodesLSHorizontal = 0x1A,
			eButtonCodesLSVertical,
			eButtonCodesRSHorizontal,
			eButtonCodesRSVertical,
		};

		enum KeyCodes : int16_t
		{
			eKeyCodesEscape,
			eKeyCodesF1,
			eKeyCodesF2,
			eKeyCodesF3,
			eKeyCodesF4,
			eKeyCodesF5,
			eKeyCodesF6,
			eKeyCodesF7,
			eKeyCodesF8,
			eKeyCodesF9,
			eKeyCodesF10,
			eKeyCodesF11,
			eKeyCodesF12,
			eKeyCodesPrintScreen,
			eKeyCodesF14,
			eKeyCodesF15,
			eKeyCodesTilde, // VK_OEM_3
			eKeyCodes1,
			eKeyCodes2,
			eKeyCodes3,
			eKeyCodes4,
			eKeyCodes5,
			eKeyCodes6,
			eKeyCodes7,
			eKeyCodes8,
			eKeyCodes9,
			eKeyCodes0,
			eKeyCodesMinus,
			eKeyCodesPlus,
			eKeyCodesBack,
			eKeyCodesTab,
			eKeyCodesQ,
			eKeyCodesW,
			eKeyCodesE,
			eKeyCodesR,
			eKeyCodesT,
			eKeyCodesY,
			eKeyCodesU,
			eKeyCodesI,
			eKeyCodesO,
			eKeyCodesP,
			eKeyCodesLBracket, // VK_OEM_4
			eKeyCodesRBracket, // VK_OEM_6
			eKeyCodesPipe, // VK_OEM_5
			eKeyCodesCapital,
			eKeyCodesA,
			eKeyCodesS,
			eKeyCodesD,
			eKeyCodesF,
			eKeyCodesG,
			eKeyCodesH,
			eKeyCodesJ,
			eKeyCodesK,
			eKeyCodesL,
			eKeyCodesColon, // VK_OEM_1
			eKeyCodesQuote, // VK_OEM_7
			eKeyCodesEnter,
			eKeyCodesLShift,
			eKeyCodesZ,
			eKeyCodesX,
			eKeyCodesC,
			eKeyCodesV,
			eKeyCodesB,
			eKeyCodesN,
			eKeyCodesM,
			eKeyCodesComma,
			eKeyCodesPeriod,
			eKeyCodesQuestion, // VK_OEM_2
			eKeyCodesRShift,
			eKeyCodesLControl,
			eKeyCodesUnused46, // Left Windows key, but will always fail
			eKeyCodesLAlt,
			eKeyCodesSpace,
			eKeyCodesRAlt,
			eKeyCodesUnused4A, // Right Windows key, but will always fail
			eKeyCodesApps,
			eKeyCodesRcontrol,
			eKeyCodesUp,
			eKeyCodesDown,
			eKeyCodesLeft,
			eKeyCodesRight,
			eKeyCodesInsert,
			eKeyCodesHome,
			eKeyCodesPageUp,
			eKeyCodesDelete,
			eKeyCodesEnd,
			eKeyCodesPageDown,
			eKeyCodesNumLock,
			eKeyCodesDivide,
			eKeyCodesMultiply,
			eKeyCodesNumpad0,
			eKeyCodesNumpad1,
			eKeyCodesNumpad2,
			eKeyCodesNumpad3,
			eKeyCodesNumpad4,
			eKeyCodesNumpad5,
			eKeyCodesNumpad6,
			eKeyCodesNumpad7,
			eKeyCodesNumpad8,
			eKeyCodesNumpad9,
			eKeyCodesSubtract,
			eKeyCodesAdd,
			eKeyCodesNumpadEnter,
			eKeyCodesDecimal,
			eKeyCodesUnused68,
			eKeyCodesShift,
			eKeyCodesCtrl,
			eKeyCodesUnused6B, // Windows key, but will always fail
			eKeyCodesAlt,

			eKeyCodes_Count, // Not actually a key, just represents the number
							 // of keys that the game scans

			eKeyCodes_None = 0xFF, // An invalid key code (for use in unset bindings)
		};

		enum MouseButtons : uint8_t
		{
			eMouseButtonsLeft,
			eMouseButtonsMiddle,
			eMouseButtonsRight,
			eMouseButtons4,
			eMouseButtons5,

			// Not sure what these 3 are...they aren't buttons and can't be bound to anything
			eMouseButtonsUnk5,
			eMouseButtonsUnk6,
			eMouseButtonsUnk7,

			eMouseButtonsWheelUp,
			eMouseButtonsWheelDown,

			eMouseButtons_Count,
			eMouseButtons_None = 0xFF, // An invalid mouse button (for use in unset bindings)
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
			KeyCodes Code;               // The key code, or -1 if unavailable
			char16_t Char;               // For eKeyEventTypeChar events, the character that was typed, or -1 if unavailable
			bool PreviousState;          // If true, the key was down before this event happened
		};
		static_assert(sizeof(KeyEvent) == 0x10, "Invalid KeyEvent size");

		// TODO: Map more of these unknowns

		enum InputAction : uint8_t
		{
			eInputActionUnk0,
			eInputActionUnk1,
			eInputActionUnk2,
			eInputActionUnk3,
			eInputActionUnk4,
			eInputActionUnk5,
			eInputActionPause,
			eInputActionScoreboard,
			eInputActionUnk8,
			eInputActionUnk9,
			eInputActionUnk10,
			eInputActionUnk11,
			eInputActionUnk12,
			eInputActionUnk13,
			eInputActionUnk14,
			eInputActionUnk15,
			eInputActionJump,
			eInputActionSwitchGrenades,
			eInputActionUnk18,
			eInputActionUnk19,
			eInputActionReloadRight,
			eInputActionPickUpRight,
			eInputActionReloadLeft,
			eInputActionPickUpLeft,
			eInputActionMelee,
			eInputActionThrowGrenade,
			eInputActionFireRight,
			eInputActionFireLeft,
			eInputActionMelee2, // ???
			eInputActionCrouch,
			eInputActionZoom,
			eInputActionUnk31,
			eInputActionUnk32,
			eInputActionSprint,
			eInputActionUnk34,
			eInputActionUnk35,
			eInputActionUnk36,
			eInputActionUnk37,
			eInputActionUnk38,
			eInputActionGeneralChat,
			eInputActionTeamChat,
			eInputActionUnk41,
			eInputActionUnk42,
			eInputActionUnk43,
			eInputActionUseConsumable1,
			eInputActionUseConsumable2,
			eInputActionUseConsumable3,
			eInputActionUseConsumable4,
			eInputActionBoostVehicle,
			eInputActionDiveVehicle,
			eInputActionRaiseVehicle,
			eInputActionAccelerate,
			eInputActionBrake,
			eInputActionUnk53,
			eInputActionUnk54,
			eInputActionExitVehicle,
			eInputActionUnk56,
			eInputActionUnk57,
			eInputActionUnk58,

			eInputAction_ControllerCount = 59,

			// These actions CANNOT be bound to controller buttons or else you
			// will overflow the controller bindings array! (Also, it seems
			// that mouse bindings will ignore these, even though there's room
			// for them.)

			eInputActionMoveForward = 59,
			eInputActionMoveBack,
			eInputActionMoveLeft,
			eInputActionMoveRight,

			eInputAction_KeyboardMouseCount = 63
		};

		struct BindingsTable
		{
			float Unknown0;
			float Unknown4;
			uint8_t ControllerBindings[eInputAction_ControllerCount];
			uint8_t Unknown43[eInputAction_ControllerCount];
			KeyCodes PrimaryKeys[eInputAction_KeyboardMouseCount];
			KeyCodes SecondaryKeys[eInputAction_KeyboardMouseCount];
			MouseButtons PrimaryMouseButtons[eInputAction_KeyboardMouseCount];
			MouseButtons SecondaryMouseButtons[eInputAction_KeyboardMouseCount];
			float Unknown1F8;
			float Unknown1FC;
			float Unknown200;
			float Unknown204;
		};
		static_assert(sizeof(BindingsTable) == 0x208, "Invalid BindingsTable size");

		// Gets the number of ticks that a key has been held down for.
		// Will always be nonzero if the key is down.
		uint8_t GetKeyTicks(KeyCodes key, InputType type);

		// Gets the number of milliseconds that a key has been held down for.
		// Will always be nonzero if the key is down.
		uint16_t GetKeyMs(KeyCodes key, InputType type);

		// Reads a raw keyboard input event. Returns false if nothing is
		// available. You should call this in a loop to ensure that you process
		// all available events. NOTE THAT THIS IS ONLY GUARANTEED TO WORK
		// AFTER WINDOWS MESSAGES HAVE BEEN PUMPED IN THE UPDATE CYCLE. ALSO,
		// THIS WILL NOT WORK IF UI INPUT IS DISABLED, REGARDLESS OF THE INPUT
		// TYPE YOU SPECIFY.
		bool ReadKeyEvent(KeyEvent* result, InputType type);

		// Blocks or unblocks an input type.
		void BlockInput(InputType type, bool block);

		// Gets a pointer to the input bindings table for a local player.
		// Halo Online only uses index 0, but there are 4 total.
		// This will return null if the index is out-of-range.
		BindingsTable* GetBindings(int localPlayerIndex);
	}
}