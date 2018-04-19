#include "ModuleInput.hpp"
#include <sstream>
#include <algorithm>
#include "../ElDorito.hpp"
#include "../Patches/Input.hpp"
#include "../Console.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Utils/NameValueTable.hpp"

#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
using namespace Blam::Input;

namespace
{
	extern Utils::NameValueTable<ControllerButton> controllerButtons;
	extern Utils::NameValueTable<KeyCode> keyCodes;
	extern Utils::NameValueTable<MouseButton> mouseButtons;
	extern Utils::NameValueTable<GameAction> gameActions;
	extern std::vector<ConfigurableAction> infantryControls;
	extern std::vector<ConfigurableAction> vehicleControls;

	// The bindings to redirect preferences.dat reads to
	BindingsTable bindings;

	// Holds information about a command bound to a key
	struct CommandBinding
	{
		std::vector<std::string> command; // If this is empty, no command is bound
		bool isHold; // True if the command binds to a boolean variable
		bool active; // True if this is a hold command and the key is down
	};

	// Bindings for each key
	CommandBinding commandBindings[eKeyCode_Count];

	void CopyBinding(GameAction sourceAction, GameAction destAction)
	{
		bindings.ControllerButtons[destAction] = bindings.ControllerButtons[sourceAction];
		bindings.ControllerHoldButtons[destAction] = bindings.ControllerHoldButtons[sourceAction];
		bindings.PrimaryKeys[destAction] = bindings.PrimaryKeys[sourceAction];
		bindings.SecondaryKeys[destAction] = bindings.SecondaryKeys[sourceAction];
		bindings.PrimaryMouseButtons[destAction] = bindings.PrimaryMouseButtons[sourceAction];
		bindings.SecondaryMouseButtons[destAction] = bindings.SecondaryMouseButtons[sourceAction];
	}

	void LoadDefaultBindings()
	{
		memset(&bindings, 0, sizeof(bindings));

		bindings.ControllerSensitivityX = 120;
		bindings.ControllerSensitivityY = 60;
		bindings.FlyingCameraSpeed = .8f;
		bindings.FlyingCameraThrust = 1.f;

		// Set all bindings to "none"
		for (auto i = 0; i < eGameAction_KeyboardMouseCount; i++)
		{
			if (i < eGameAction_ControllerCount)
				bindings.ControllerButtons[i] = eControllerButton_None;
			bindings.PrimaryKeys[i] = eKeyCode_None;
			bindings.SecondaryKeys[i] = eKeyCode_None;
			bindings.PrimaryMouseButtons[i] = eMouseButton_None;
			bindings.SecondaryMouseButtons[i] = eMouseButton_None;
		}

		// Bind UI actions directly to their controller buttons
		// The ControllerButton enum lines up with the beginning of the
		// GameAction enum, and the game itself even uses a for loop like this
		for (auto i = 0; i < eControllerButton_Count; i++)
			bindings.ControllerButtons[i] = static_cast<ControllerButton>(i);

		// Default controller bindings

		bindings.ControllerButtons[eGameActionJump]           = eControllerButtonA;
		bindings.ControllerButtons[eGameActionSwitchGrenades] = eControllerButtonDpadRight;
		bindings.ControllerButtons[eGameActionSwitchWeapons]  = eControllerButtonY;
		bindings.ControllerButtons[eGameActionUnk19]          = eControllerButtonY;
		bindings.ControllerButtons[eGameActionReloadRight]    = eControllerButtonRightBumper;
		bindings.ControllerButtons[eGameActionUse]            = eControllerButtonRightBumper;
		bindings.ControllerButtons[eGameActionReloadLeft]     = eControllerButtonLeftBumper;
		bindings.ControllerButtons[eGameActionPickUpLeft]     = eControllerButtonLeftBumper;
		bindings.ControllerButtons[eGameActionMelee]          = eControllerButtonB;
		bindings.ControllerButtons[eGameActionThrowGrenade]   = eControllerButtonLeftTrigger;
		bindings.ControllerButtons[eGameActionFireRight]      = eControllerButtonRightTrigger;
		bindings.ControllerButtons[eGameActionFireLeft]       = eControllerButtonLeftTrigger;
		bindings.ControllerButtons[eGameActionCrouch]         = eControllerButtonX;
		bindings.ControllerButtons[eGameActionZoom]           = eControllerButtonRightStick;
		bindings.ControllerButtons[eGameActionUnk31]          = eControllerButtonDpadLeft;
		bindings.ControllerButtons[eGameActionUnk32]          = eControllerButtonDpadRight;
		bindings.ControllerButtons[eGameActionSprint]         = eControllerButtonLeftStick;
		bindings.ControllerButtons[eGameActionFlashlight]     = eControllerButtonDpadUp;
		bindings.ControllerButtons[eGameActionUnk35]          = eControllerButtonDpadDown;
		bindings.ControllerButtons[eGameActionUnk36]          = eControllerButtonDpadUp;
		bindings.ControllerButtons[eGameActionUnk37]          = eControllerButtonDpadLeft;
		bindings.ControllerButtons[eGameActionUnk38]          = eControllerButtonDpadRight;
		bindings.ControllerButtons[eGameActionVoiceChat]      = eControllerButtonDpadDown;
		bindings.ControllerButtons[eGameActionUnk42]          = eControllerButtonA;
		bindings.ControllerButtons[eGameActionVehicleBoost]   = eControllerButtonLeftTrigger;
		bindings.ControllerButtons[eGameActionVehicleDive]    = eControllerButtonX;
		bindings.ControllerButtons[eGameActionVehicleRaise]   = eControllerButtonA;
		bindings.ControllerButtons[eGameActionVehicleFire]    = eControllerButtonRightTrigger;
		bindings.ControllerButtons[eGameActionVehicleAltFire] = eControllerButtonLeftTrigger;
		bindings.ControllerButtons[eGameActionMeleeFire]      = eControllerButtonB;
		bindings.ControllerButtons[eGameActionNextPlayer]     = eControllerButtonDpadRight;
		bindings.ControllerButtons[eGameActionPrevPlayer]     = eControllerButtonDpadLeft;
		bindings.ControllerButtons[eGameActionUnk58]          = eControllerButtonA;
		bindings.ControllerButtons[eGameActionUseConsumable1] = eControllerButtonLeftBumper;
		bindings.ControllerButtons[eGameActionUiStart]        = eControllerButtonStart;
		bindings.ControllerButtons[eGameActionUiSelect]       = eControllerButtonSelect;

		bindings.ControllerHoldButtons[eGameActionUnk19]      = true;
		bindings.ControllerHoldButtons[eGameActionUse]        = true;
		bindings.ControllerHoldButtons[eGameActionPickUpLeft] = true;

		// Default keyboard/mouse bindings

		bindings.PrimaryKeys[eGameActionUiUp]                    = eKeyCodeUp;
		bindings.PrimaryKeys[eGameActionUiDown]                  = eKeyCodeDown;
		bindings.PrimaryKeys[eGameActionUiLeft]                  = eKeyCodeLeft;
		bindings.PrimaryKeys[eGameActionUiRight]                 = eKeyCodeRight;
		bindings.PrimaryKeys[eGameActionUiStart]                 = eKeyCodeEscape;
		bindings.PrimaryKeys[eGameActionUiSelect]                = eKeyCodeTab;
		bindings.PrimaryKeys[eGameActionUiA]                     = eKeyCodeEnter;
		bindings.SecondaryKeys[eGameActionUiA]                   = eKeyCodeSpace;
		bindings.PrimaryKeys[eGameActionUiB]                     = eKeyCodeEscape;
		bindings.SecondaryKeys[eGameActionUiB]                   = eKeyCodeBack;
		bindings.PrimaryKeys[eGameActionUiX]                     = eKeyCodeX;
		bindings.PrimaryKeys[eGameActionUiY]                     = eKeyCodeDelete;
		bindings.PrimaryKeys[eGameActionUiLeftBumper]            = eKeyCodeQ;
		bindings.PrimaryKeys[eGameActionUiRightBumper]           = eKeyCodeE;
		bindings.PrimaryKeys[eGameActionJump]                    = eKeyCodeSpace;
		bindings.PrimaryKeys[eGameActionSwitchGrenades]          = eKeyCodeG;
		bindings.PrimaryMouseButtons[eGameActionSwitchWeapons]   = eMouseButtonWheelUp;
		bindings.SecondaryMouseButtons[eGameActionSwitchWeapons] = eMouseButtonWheelDown;
		bindings.PrimaryKeys[eGameActionReloadRight]             = eKeyCodeR;
		bindings.PrimaryKeys[eGameActionUse]                     = eKeyCodeE;
		bindings.PrimaryKeys[eGameActionReloadLeft]              = eKeyCodeZ; // TODO: Find a better key?
		bindings.PrimaryKeys[eGameActionPickUpLeft]              = eKeyCodeZ; // TODO: Find a better key?
		bindings.PrimaryKeys[eGameActionMelee]                   = eKeyCodeQ;
		bindings.SecondaryKeys[eGameActionMelee]                 = eKeyCodeV;
		bindings.PrimaryKeys[eGameActionThrowGrenade]            = eKeyCodeF;
		bindings.SecondaryMouseButtons[eGameActionThrowGrenade]  = eMouseButtonMiddle;
		bindings.PrimaryMouseButtons[eGameActionFireRight]       = eMouseButtonLeft;
		bindings.PrimaryMouseButtons[eGameActionFireLeft]        = eMouseButtonRight;
		bindings.PrimaryKeys[eGameActionCrouch]                  = eKeyCodeLControl;
		bindings.PrimaryMouseButtons[eGameActionZoom]            = eMouseButtonRight;
		bindings.PrimaryKeys[eGameActionSprint]                  = eKeyCodeLShift;
		bindings.PrimaryKeys[eGameActionGeneralChat]             = eKeyCodeT;
		bindings.PrimaryKeys[eGameActionTeamChat]                = eKeyCodeY;
		bindings.PrimaryKeys[eGameActionVoiceChat]               = eKeyCodeCapital;
		bindings.PrimaryKeys[eGameActionUnk42]                   = eKeyCodeSpace;
		bindings.PrimaryKeys[eGameActionVehicleBoost]            = eKeyCodeLShift;
		bindings.PrimaryKeys[eGameActionVehicleDive]             = eKeyCodeLControl;
		bindings.SecondaryMouseButtons[eGameActionVehicleDive]   = eMouseButtonWheelDown;
		bindings.PrimaryKeys[eGameActionVehicleRaise]            = eKeyCodeSpace;
		bindings.SecondaryMouseButtons[eGameActionVehicleRaise]  = eMouseButtonWheelUp;
		bindings.PrimaryKeys[eGameActionVehicleAccelerate]       = eKeyCodeW;
		bindings.SecondaryKeys[eGameActionVehicleAccelerate]     = eKeyCodeUp;
		bindings.PrimaryKeys[eGameActionVehicleBrake]            = eKeyCodeS;
		bindings.SecondaryKeys[eGameActionVehicleBrake]          = eKeyCodeDown;
		bindings.PrimaryMouseButtons[eGameActionVehicleFire]     = eMouseButtonLeft;
		bindings.PrimaryMouseButtons[eGameActionVehicleAltFire]  = eMouseButtonRight;
		bindings.PrimaryKeys[eGameActionMeleeFire]               = eKeyCodeQ;
		bindings.SecondaryKeys[eGameActionMeleeFire]             = eKeyCodeV;
		bindings.PrimaryKeys[eGameActionNextPlayer]              = eKeyCodeD;
		bindings.PrimaryKeys[eGameActionPrevPlayer]              = eKeyCodeA;
		bindings.PrimaryKeys[eGameActionUnk58]                   = eKeyCodeSpace;
		bindings.PrimaryKeys[eGameActionMoveForward]             = eKeyCodeW;
		bindings.SecondaryKeys[eGameActionMoveForward]           = eKeyCodeUp;
		bindings.PrimaryKeys[eGameActionMoveBack]                = eKeyCodeS;
		bindings.SecondaryKeys[eGameActionMoveBack]              = eKeyCodeDown;
		bindings.PrimaryKeys[eGameActionMoveLeft]                = eKeyCodeA;
		bindings.SecondaryKeys[eGameActionMoveLeft]              = eKeyCodeLeft;
		bindings.PrimaryKeys[eGameActionMoveRight]               = eKeyCodeD;
		bindings.SecondaryKeys[eGameActionMoveRight]             = eKeyCodeRight;
		bindings.SecondaryKeys[eGameActionUseConsumable1]		 = eKeyCode1;
		bindings.SecondaryKeys[eGameActionFlashlight]            = eKeyCode2;

		Modules::ModuleInput::UpdateBindings();
	}

	int GetDefaultSettingsKey(int action, KeyCode *keys, MouseButton *mouseButtons)
	{
		auto primaryKey = keys[action];
		if (primaryKey != eKeyCode_None)
			return primaryKey;
		auto mouseButton = mouseButtons[action];
		if (mouseButton != eMouseButton_None)
			return (mouseButton + 1) * 0x100;
		return -1;
	}

	void PopulateDefaultKeys(std::vector<ConfigurableAction> *actions)
	{
		for (auto &configAction : *actions)
		{
			configAction.DefaultPrimaryKey = GetDefaultSettingsKey(configAction.Action, bindings.PrimaryKeys, bindings.PrimaryMouseButtons);
			configAction.DefaultSecondaryKey = GetDefaultSettingsKey(configAction.Action, bindings.SecondaryKeys, bindings.SecondaryMouseButtons);
		}
	}

	void BuildSettingsMenu()
	{
		// Set default keys based on the default bindings that have been set
		PopulateDefaultKeys(&infantryControls);
		PopulateDefaultKeys(&vehicleControls);
		Patches::Input::SetKeyboardSettingsMenu(infantryControls, vehicleControls);
	}

	bool VariableInputRawInputUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		unsigned long value = Modules::ModuleInput::Instance().VarInputRawInput->ValueInt;

		std::string status = "disabled.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "enabled.";

		returnInfo = "Raw input " + status;
		return true;
	}

	bool CommandBind(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1)
		{
			returnInfo =  "Usage: Bind <key> [[+]command] [arguments]\n";
			returnInfo += "If the command starts with a +, then it will be ";
			returnInfo += "passed an argument of 1 on key down and 0 on key ";
			returnInfo += "up. Omit the command to unbind the key.";
			return false;
		}

		const auto& rawArguments = Arguments[0];
		auto partStart = rawArguments.begin();

		std::string keyName, command;
		KeyCode keyCode;
		bool isHold = false;

		enum { KEYNAME_PART, COMMAND_HOLD_PART, COMMAND_PART, } state = KEYNAME_PART;

		for (auto it = rawArguments.begin(); it != rawArguments.end(); ++it)
		{
			auto isEnd = it == rawArguments.end() - 1;
			auto ch = *it;

			switch (state)
			{
			case KEYNAME_PART:
				if (ch == ' ' || isEnd)
				{
					keyName = std::string(partStart, isEnd ? it + 1 : it);
					if (keyName.length() == 0 || !keyCodes.FindValueIgnoreCase(keyName, &keyCode))
					{
						returnInfo = "Unrecognized key name: " + keyName;
						return false;
					}
					state = COMMAND_HOLD_PART;
					partStart = it + 1;
					break;
				}
				break;
			case COMMAND_HOLD_PART:
				partStart = it--;
				isHold = ch == '+';
				if (isHold)
					partStart++;
				state = COMMAND_PART;
				break;
			case COMMAND_PART:
				if (ch == ' ' || isEnd)
				{
					auto partEnd = isEnd ? it + 1 : it;
					command = std::string(partStart, partEnd);
					partStart = it + 1;
					it = rawArguments.end() - 1;
				}
				break;
			}
		}

		auto binding = &commandBindings[keyCode];

		// If no command was specified, unset the binding
		if (command.length() == 0)
		{
			binding->command.clear();
			returnInfo = "Binding cleared.";
			return true;
		}

		// Set the binding
		binding->isHold = isHold;
		binding->command.clear();
		binding->command.push_back(command);
		binding->command.push_back(std::string(partStart, rawArguments.end()));

		returnInfo = "Binding set.";
		return true;
	}

	void KeyboardUpdated()
	{
		for (auto i = 0; i < eKeyCode_Count; i++)
		{
			const auto binding = &commandBindings[i];
			if (binding->command.size() == 0)
				continue; // Key is not bound

			// Read the key
			auto keyCode = static_cast<KeyCode>(i);
			auto keyTicks = GetKeyTicks(keyCode, eInputTypeSpecial);

			// We're only interested in the key if it was just pressed or if
			// this is a hold binding and it was just released
			if (keyTicks > 1 || (keyTicks == 0 && !(binding->isHold && binding->active)))
				continue;

			auto command = binding->command;
			if (binding->isHold)
			{
				// The command is a hold binding - append an argument depending
				// on whether it was pressed or released
				if (keyTicks == 1)
					command.push_back("1");
				else if (keyTicks == 0)
					command.push_back("0");
				binding->active = (keyTicks > 0);
			}

			// Execute the command and print its result
			auto result = Modules::CommandMap::Instance().ExecuteCommand(command, true);
			Console::WriteLine(result);
		}
	}

	bool CommandUIButtonPress(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1)
		{
			returnInfo = "Usage: Input.UIButtonPress <btnCode>";
			return false;
		}

		// taken from sub_A935C0

		// alt?A936B0

		uint32_t keyPressData[4];
		keyPressData[0] = 0xD;
		keyPressData[1] = 0; // controller idx
		keyPressData[2] = std::stoul(Arguments[0], 0, 0); // button idx, corresponds with Blam::UiButtonCode
		keyPressData[3] = 0xFF;

		typedef void*(__cdecl *sub_AAD930Ptr)();
		auto sub_AAD930 = reinterpret_cast<sub_AAD930Ptr>(0xAAD930);
		void* classPtr = sub_AAD930();

		typedef int(__thiscall *sub_AAB7D0Ptr)(void* thisPtr, void* a2);
		auto sub_AAB7D0 = reinterpret_cast<sub_AAB7D0Ptr>(0xAAB7D0);
		int retVal = sub_AAB7D0(classPtr, keyPressData);

		return retVal != 0;
	}

	bool ParseKeyOrMouseButton(const std::string &name, KeyCode *resultKey, MouseButton *resultMouse)
	{
		*resultKey = eKeyCode_None;
		*resultMouse = eMouseButton_None;
		if (Utils::String::ToLower(name) == "none")
			return true;
		if (keyCodes.FindValueIgnoreCase(name, resultKey))
			return true;
		return mouseButtons.FindValueIgnoreCase(name, resultMouse);
	}

	bool ParseControllerButton(const std::string &name, ControllerButton *result)
	{
		*result = eControllerButton_None;
		if (Utils::String::ToLower(name) == "none")
			return true;
		return controllerButtons.FindValueIgnoreCase(name, result);
	}

	bool CommandBindControllerAction(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1 || Arguments.size() > 2)
			return false;

		// Parse the action name
		auto actionName = Arguments[0];
		GameAction action;
		if (!gameActions.FindValueIgnoreCase(actionName, &action))
		{
			returnInfo = "Unrecognized action name: " + actionName;
			return false;
		}
		if (action >= eGameAction_ControllerCount)
		{
			returnInfo = "Movement actions cannot be bound to controller buttons.";
			return false;
		}

		// If a button name was passed, parse it
		auto button = eControllerButton_None;
		if (Arguments.size() > 1)
		{
			auto buttonName = Arguments[1];
			if (!ParseControllerButton(buttonName, &button))
			{
				returnInfo = "Unrecognized button name: " + buttonName;
				return false;
			}
		}

		// Update the bindings table!
		bindings.ControllerButtons[action] = button;

		Modules::ModuleInput::UpdateBindings();
		if (button != eKeyCode_None)
			returnInfo = "Action binding set.";
		else
			returnInfo = "Action binding cleared.";
		return true;
	}

	bool CommandBindKeyboardAction(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1 || Arguments.size() > 3)
			return false;

		// Parse the action name
		auto actionName = Arguments[0];
		GameAction action;
		if (!gameActions.FindValueIgnoreCase(actionName, &action))
		{
			returnInfo = "Unrecognized action name: " + actionName;
			return false;
		}

		// If a primary key name was passed, parse it
		auto primaryKey = eKeyCode_None;
		auto primaryMouse = eMouseButton_None;
		if (Arguments.size() > 1)
		{
			auto primaryKeyName = Arguments[1];
			if (!ParseKeyOrMouseButton(primaryKeyName, &primaryKey, &primaryMouse))
			{
				returnInfo = "Unrecognized primary key name: " + primaryKeyName;
				return false;
			}
		}

		// If a secondary key name was passed, parse it
		auto secondaryKey = eKeyCode_None;
		auto secondaryMouse = eMouseButton_None;
		if (Arguments.size() > 2)
		{
			auto secondaryKeyName = Arguments[2];
			if (!ParseKeyOrMouseButton(secondaryKeyName, &secondaryKey, &secondaryMouse))
			{
				returnInfo = "Unrecognized secondary key name: " + secondaryKeyName;
				return false;
			}
		}

		// If the primary key is none, move the secondary key into the primary key
		if (primaryKey == eKeyCode_None && primaryMouse == eMouseButton_None)
		{
			primaryKey = secondaryKey;
			primaryMouse = secondaryMouse;
			secondaryKey = eKeyCode_None;
			secondaryMouse = eMouseButton_None;
		}

		// If the action is a movement action, make sure it's getting set to a key
		// (Everything after the controller-only actions can only be bound to keys)
		if (action >= eGameAction_ControllerCount && (primaryMouse != eMouseButton_None || secondaryMouse != eMouseButton_None))
		{
			returnInfo = "Movement actions cannot be bound to mouse buttons.";
			return false;
		}

		// Update the bindings table!
		bindings.PrimaryKeys[action] = primaryKey;
		bindings.PrimaryMouseButtons[action] = primaryMouse;
		bindings.SecondaryKeys[action] = secondaryKey;
		bindings.SecondaryMouseButtons[action] = secondaryMouse;

		Modules::ModuleInput::UpdateBindings();
		if (primaryKey != eKeyCode_None || primaryMouse != eMouseButton_None)
			returnInfo = "Action binding set.";
		else
			returnInfo = "Action binding cleared.";
		return true;
	}

	bool VariableControllerSensitivityXUpdated(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto newSensitivity = Modules::ModuleInput::Instance().VarControllerSensitivityX->ValueFloat;
		bindings.ControllerSensitivityX = std::max(1.f, newSensitivity);
		Modules::ModuleInput::UpdateBindings();
		return true;
	}

	bool VariableControllerSensitivityYUpdated(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto newSensitivity = Modules::ModuleInput::Instance().VarControllerSensitivityY->ValueFloat;
		bindings.ControllerSensitivityY = std::max(1.f, newSensitivity);
		Modules::ModuleInput::UpdateBindings();
		return true;
	}

	bool VariableControllerStickLayoutUpdated(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto layout = Modules::ModuleInput::Instance().VarControllerStickLayout->ValueInt;
		bindings.JoystickLayout = layout;
		Modules::ModuleInput::UpdateBindings();
		return true;
	}

	bool CommandDumpBindingsJson(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		returnInfo = "";
		BindingsTable currentBindings;
		GetBindings(0, &currentBindings);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		writer.StartArray();
		for (auto i = 0; i < eGameAction_KeyboardMouseCount; i++)
		{
			std::string names[6] = { "none", "n/a", "none", "none", "none", "none" };
			if (i < eGameAction_ControllerCount)
			{
				controllerButtons.FindName(currentBindings.ControllerButtons[i], &names[0]);
				names[1] = currentBindings.ControllerHoldButtons[i] ? "true" : "false";
			}
			keyCodes.FindName(currentBindings.PrimaryKeys[i], &names[2]);
			keyCodes.FindName(currentBindings.SecondaryKeys[i], &names[3]);
			mouseButtons.FindName(currentBindings.PrimaryMouseButtons[i], &names[4]);
			mouseButtons.FindName(currentBindings.SecondaryMouseButtons[i], &names[5]);
			writer.StartObject();

			std::string actionName;
			if (gameActions.FindName(static_cast<GameAction>(i), &actionName))
			{
				writer.Key("actionName");
				writer.String(actionName.c_str());
			}
			else{
				writer.Key("actionName");
				writer.String(std::to_string(i).c_str());
			}
			writer.Key("controllerButton");
			writer.String(names[0].c_str());

			writer.Key("controllerHoldButton");
			writer.String(names[1].c_str());

			writer.Key("primaryKey");
			writer.String(names[2].c_str());

			writer.Key("secondaryKey");
			writer.String(names[3].c_str());

			writer.Key("primaryMouseButton");
			writer.String(names[4].c_str());

			writer.Key("secondaryMouseButton");
			writer.String(names[5].c_str());
			writer.EndObject();

		}
		writer.EndArray();
		returnInfo += buffer.GetString();

		return true;

	}

	bool CommandFindKeybdBinding(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1)
		{
			returnInfo = "Not enough arguments";
			return false;
		}
		std::string search_for = Arguments[0];
		std::transform(search_for.begin(), search_for.end(), search_for.begin(), ::tolower);
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartArray();
		for (int i = 0; i < eKeyCode_Count; i++)
		{
			for (int k = 0; k < commandBindings[i].command.size(); k++)
			{
				std::string command_part = commandBindings[i].command[k];
				std::transform(command_part.begin(), command_part.end(), command_part.begin(), ::tolower);
				if (command_part.compare(search_for) == 0)
				{
					std::string key;
					keyCodes.FindName((Blam::Input::KeyCode)i, &key);
					writer.String(key.c_str());
				}
			}
		}
		writer.EndArray();
		returnInfo = buffer.GetString();
		return true;
	}

#ifdef _DEBUG
	bool CommandDumpBindings(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		returnInfo = "";
		BindingsTable currentBindings;
		GetBindings(0, &currentBindings);
		for (auto i = 0; i < eGameAction_KeyboardMouseCount; i++)
		{
			std::string names[6] = { "none", "n/a", "none", "none", "none", "none" };
			if (i < eGameAction_ControllerCount)
			{
				controllerButtons.FindName(currentBindings.ControllerButtons[i], &names[0]);
				names[1] = currentBindings.ControllerHoldButtons[i] ? "true" : "false";
			}
			keyCodes.FindName(currentBindings.PrimaryKeys[i], &names[2]);
			keyCodes.FindName(currentBindings.SecondaryKeys[i], &names[3]);
			mouseButtons.FindName(currentBindings.PrimaryMouseButtons[i], &names[4]);
			mouseButtons.FindName(currentBindings.SecondaryMouseButtons[i], &names[5]);
			returnInfo += std::to_string(i);
			std::string actionName;
			if (gameActions.FindName(static_cast<GameAction>(i), &actionName))
				returnInfo += " (" + actionName + ")";
			returnInfo += ": " + names[0] + " " + names[1] + " " + names[2] + " " + names[3] + " " + names[4] + " " + names[5] + "\n";
		}
		return true;
	}
#endif

	bool CommandResetBindings(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		LoadDefaultBindings();
		returnInfo = "Default bindings restored";
		return true;
	}

	bool TryParseFloat(const char* str, float* value)
	{
		char* endp;
		*value = std::strtod(str, &endp);
		return endp != str;
	}

	bool CommandControllerVibrationTest(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		float duration = 0.5f;
		Arguments.size() > 0 && TryParseFloat(Arguments[0].c_str(), &duration);
		Patches::Input::TestControllerVibration(duration);
		return true;
	}
}

namespace Modules
{
	ModuleInput::ModuleInput() : ModuleBase("Input")
	{
		VarInputRawInput = AddVariableInt("RawInput", "rawinput", "Enables raw mouse input with no acceleration applied", eCommandFlagsArchived, 1, VariableInputRawInputUpdate);
		VarInputRawInput->ValueIntMin = 0;
		VarInputRawInput->ValueIntMax = 1;

		VarInputControllerPort = AddVariableInt("ControllerPort", "controllerport", "The port number of the player's controller.", eCommandFlagsArchived, 0);
		VarInputControllerPort->ValueIntMin = 0;
		VarInputControllerPort->ValueIntMax = 7;

		AddCommand("Bind", "bind", "Binds a command to a key", eCommandFlagsArgsNoParse, CommandBind, { "key", "[+]command", "arguments" });
		Patches::Input::RegisterDefaultInputHandler(KeyboardUpdated);

		AddCommand("UIButtonPress", "ui_btn_press", "Emulates a gamepad button press on UI menus", eCommandFlagsNone, CommandUIButtonPress, { "btnCode The code of the button to press" });

		CommandBindControllerAction = AddCommand("ControllerAction", "controller", "Binds an in-game action to a controller button", eCommandFlagsNone, ::CommandBindControllerAction, { "action The name of the action to bind", "button The button name" });
		CommandBindKeyboardAction = AddCommand("KeyboardAction", "keyboard", "Binds an in-game action to keyboard keys or mouse buttons", eCommandFlagsNone, ::CommandBindKeyboardAction, { "action The name of the action to bind", "[primary] The primary key name", "[secondary] The secondary key name" });

#ifdef _DEBUG
		AddCommand("DebugBindings", "debugbindings", "Dumps the input bindings table", eCommandFlagsNone, CommandDumpBindings);
#endif

		AddCommand("DumpBindingsJson", "dumpbindingsjson", "Dumps the input bindings table in json", eCommandFlagsNone, CommandDumpBindingsJson);
		AddCommand("FindBind", "findbind", "Finds the key bound to a command passed", eCommandFlagsNone, CommandFindKeybdBinding);
		AddCommand("ResetBindings", "reset_bindings", "Restores default bindings", eCommandFlagsNone, CommandResetBindings);

		VarControllerSensitivityX = AddVariableFloat("ControllerSensitivityX", "xsens", "Horizontal controller look sensitivity", eCommandFlagsArchived, 120, VariableControllerSensitivityXUpdated);
		VarControllerSensitivityY = AddVariableFloat("ControllerSensitivityY", "ysens", "Vertical controller look sensitivity", eCommandFlagsArchived, 60, VariableControllerSensitivityYUpdated);

		VarControllerVibrationIntensity = AddVariableFloat("ControllerVibrationIntensity", "vibration", "Controls the controller vibration intensity", eCommandFlagsArchived, 1.0f);
		VarControllerVibrationIntensity->ValueFloatMin = 0.0f;
		VarControllerVibrationIntensity->ValueFloatMax = 1.0f;
		AddCommand("ControllerVibrationTest", "vibration_test", "Test the controller vibration", eCommandFlagsNone, CommandControllerVibrationTest);

		VarControllerStickLayout = AddVariableInt("ControllerStickLayout", "stick_layout", "Controller stick layout", eCommandFlagsArchived, 0, VariableControllerStickLayoutUpdated);
		VarControllerStickLayout->ValueIntMin = 0;
		VarControllerStickLayout->ValueIntMax = 3;

		VarSpectateSensitivity = AddVariableFloat("SpectateSensitivity", "specsens", "Spectator camera sensitivity", eCommandFlagsArchived, 1.0f);

		VarControllerInvertY = AddVariableInt("ControllerInvertY", "yinvert", "Invert the controller Y look axis", eCommandFlagsArchived, 0);
		VarControllerInvertY->ValueIntMin = 0;
		VarControllerInvertY->ValueIntMax = 1;

		VarToggleSprint = AddVariableInt("ToggleSprint", "toggle_sprint", "Sprint toggle enabled (1) or disabled (0)", eCommandFlagsArchived, 0);

		VarTapScoreboard = AddVariableInt("TapScoreboard", "tap_scoreboard", "Enable/disable double tap to lock scoreboard", eCommandFlagsArchived, 1);

		LoadDefaultBindings();
		BuildSettingsMenu();
	}

	BindingsTable* ModuleInput::GetBindings()
	{
		return &bindings;
	}

	void ModuleInput::UpdateBindings()
	{
		//CopyBinding(eGameActionMelee, eGameActionMeleeFire); // Ensure that the banshee bomb is bound to melee
		CopyBinding(eGameActionUse, eGameActionVehicleExit); // Keep enter and exit vehicle to the same button
		SetBindings(0, bindings);

		Patches::Input::InvalidateBindings();
	}

	std::string ModuleInput::ExportBindings() const
	{
		std::string result;

		// Save controller bindings
		for (auto &&action : gameActions)
		{
			// Don't save keyboard-only actions
			if (action.second >= eGameAction_ControllerCount)
				continue;

			// Look up the button name
			auto button = bindings.ControllerButtons[action.second];
			std::string buttonName = "";
			if (button < eControllerButton_Count)
				controllerButtons.FindName(button, &buttonName);

			result += CommandBindControllerAction->Name + " \"" + action.first + "\"";
			if (buttonName.length() > 0)
				result += " \"" + buttonName + "\"";
			result += "\n";
		}
		result += "\n";

		// Save keyboard and mouse bindings
		for (auto &&action : gameActions)
		{
			// Try to get primary and secondary names
			std::string primaryName, secondaryName;
			auto primaryKey = bindings.PrimaryKeys[action.second];
			auto secondaryKey = bindings.SecondaryKeys[action.second];
			auto primaryMouse = bindings.PrimaryMouseButtons[action.second];
			auto secondaryMouse = bindings.SecondaryMouseButtons[action.second];
			if (primaryKey < eKeyCode_Count)
				keyCodes.FindName(primaryKey, &primaryName);
			if (secondaryKey < eKeyCode_Count)
				keyCodes.FindName(secondaryKey, &secondaryName);
			if (action.second < eGameAction_ControllerCount)
			{
				// Mouse buttons can only be bound to actions that are on controllers too
				if (primaryMouse < eMouseButton_Count)
					mouseButtons.FindName(primaryMouse, &primaryName);
				if (secondaryMouse < eMouseButton_Count)
					mouseButtons.FindName(secondaryMouse, &secondaryName);
			}

			result += CommandBindKeyboardAction->Name + " \"" + action.first + "\"";
			if (primaryName.length() > 0)
				result += " \"" + primaryName + "\"";
			if (secondaryName.length() > 0)
				result += " \"" + secondaryName + "\"";
			result += "\n";
		}

		for (auto i = 0; i < eKeyCode_Count; i++)
		{
			const auto binding = &commandBindings[i];
			if (binding->command.size() == 0)
				continue; // Key is not bound
			std::string key_name;
			std::string line;
			keyCodes.FindName(static_cast<KeyCode>(i), &key_name);
			std::string prefix = "";
			if (binding->isHold)
				prefix = "+";
			line += "bind " + key_name + " " + prefix;
			for (std::string command : binding->command)
			{
				line += command + " ";
			}
			line.erase(line.find_last_not_of(" \n\r\t") + 1);
			result += line + "\n";
		}
		return result;
	}
	bool ModuleInput::IsCommandBound(std::string command)
	{
		for (int i = 0; i < eKeyCode_Count; i++)
		{
			const auto binding = &commandBindings[i];
			if (binding->command.size() == 0)
				continue; // Key is not bound
			std::string bound_command = "";
			if (binding->command[1] == "")
				bound_command = binding->command[0];
			else
			{
				bound_command = binding->command[0] + " " + binding->command[1];
			}
			std::transform(bound_command.begin(), bound_command.end(), bound_command.begin(), ::tolower);
			if (command.compare(bound_command) == 0)
				return true;
		}
		return false;
	}
}

namespace
{
	Utils::NameValueTable<ControllerButton> controllerButtons =
	{
		{ "LT", eControllerButtonLeftTrigger },
		{ "RT", eControllerButtonRightTrigger },
		{ "Up", eControllerButtonDpadUp },
		{ "Down", eControllerButtonDpadDown },
		{ "Left", eControllerButtonDpadLeft },
		{ "Right", eControllerButtonDpadRight },
		{ "Start", eControllerButtonStart },
		{ "Select", eControllerButtonSelect },
		{ "LS", eControllerButtonLeftStick },
		{ "RS", eControllerButtonRightStick },
		{ "A", eControllerButtonA },
		{ "B", eControllerButtonB },
		{ "X", eControllerButtonX },
		{ "Y", eControllerButtonY },
		{ "LB", eControllerButtonLeftBumper },
		{ "RB", eControllerButtonRightBumper },
	};

	Utils::NameValueTable<KeyCode> keyCodes =
	{
		{ "Escape", eKeyCodeEscape },
		{ "F1", eKeyCodeF1 },
		{ "F2", eKeyCodeF2 },
		{ "F3", eKeyCodeF3 },
		{ "F4", eKeyCodeF4 },
		{ "F5", eKeyCodeF5 },
		{ "F6", eKeyCodeF6 },
		{ "F7", eKeyCodeF7 },
		{ "F8", eKeyCodeF8 },
		{ "F9", eKeyCodeF9 },
		{ "F10", eKeyCodeF10 },
		{ "F11", eKeyCodeF11 },
		{ "F12", eKeyCodeF12 },
		{ "PrintScreen", eKeyCodePrintScreen },
		{ "F14", eKeyCodeF14 },
		{ "F15", eKeyCodeF15 },
		{ "Tilde", eKeyCodeTilde },
		{ "1", eKeyCode1 },
		{ "2", eKeyCode2 },
		{ "3", eKeyCode3 },
		{ "4", eKeyCode4 },
		{ "5", eKeyCode5 },
		{ "6", eKeyCode6 },
		{ "7", eKeyCode7 },
		{ "8", eKeyCode8 },
		{ "9", eKeyCode9 },
		{ "0", eKeyCode0 },
		{ "Minus", eKeyCodeMinus },
		{ "Plus", eKeyCodePlus },
		{ "Back", eKeyCodeBack },
		{ "Tab", eKeyCodeTab },
		{ "Q", eKeyCodeQ },
		{ "W", eKeyCodeW },
		{ "E", eKeyCodeE },
		{ "R", eKeyCodeR },
		{ "T", eKeyCodeT },
		{ "Y", eKeyCodeY },
		{ "U", eKeyCodeU },
		{ "I", eKeyCodeI },
		{ "O", eKeyCodeO },
		{ "P", eKeyCodeP },
		{ "LBracket", eKeyCodeLBracket },
		{ "RBracket", eKeyCodeRBracket },
		{ "Pipe", eKeyCodePipe },
		{ "CapsLock", eKeyCodeCapital },
		{ "A", eKeyCodeA },
		{ "S", eKeyCodeS },
		{ "D", eKeyCodeD },
		{ "F", eKeyCodeF },
		{ "G", eKeyCodeG },
		{ "H", eKeyCodeH },
		{ "J", eKeyCodeJ },
		{ "K", eKeyCodeK },
		{ "L", eKeyCodeL },
		{ "Colon", eKeyCodeColon },
		{ "Quote", eKeyCodeQuote },
		{ "Enter", eKeyCodeEnter },
		{ "LShift", eKeyCodeLShift },
		{ "Z", eKeyCodeZ },
		{ "X", eKeyCodeX },
		{ "C", eKeyCodeC },
		{ "V", eKeyCodeV },
		{ "B", eKeyCodeB },
		{ "N", eKeyCodeN },
		{ "M", eKeyCodeM },
		{ "Comma", eKeyCodeComma },
		{ "Period", eKeyCodePeriod },
		{ "Question", eKeyCodeQuestion },
		{ "RShift", eKeyCodeRShift },
		{ "LControl", eKeyCodeLControl },
		{ "LAlt", eKeyCodeLAlt },
		{ "Space", eKeyCodeSpace },
		{ "RAlt", eKeyCodeRAlt },
		{ "Apps", eKeyCodeApps },
		{ "RControl", eKeyCodeRcontrol },
		{ "Up", eKeyCodeUp },
		{ "Down", eKeyCodeDown },
		{ "Left", eKeyCodeLeft },
		{ "Right", eKeyCodeRight },
		{ "Insert", eKeyCodeInsert },
		{ "Home", eKeyCodeHome },
		{ "PageUp", eKeyCodePageUp },
		{ "Delete", eKeyCodeDelete },
		{ "End", eKeyCodeEnd },
		{ "PageDown", eKeyCodePageDown },
		{ "NumLock", eKeyCodeNumLock },
		{ "Divide", eKeyCodeDivide },
		{ "Multiply", eKeyCodeMultiply },
		{ "Numpad0", eKeyCodeNumpad0 },
		{ "Numpad1", eKeyCodeNumpad1 },
		{ "Numpad2", eKeyCodeNumpad2 },
		{ "Numpad3", eKeyCodeNumpad3 },
		{ "Numpad4", eKeyCodeNumpad4 },
		{ "Numpad5", eKeyCodeNumpad5 },
		{ "Numpad6", eKeyCodeNumpad6 },
		{ "Numpad7", eKeyCodeNumpad7 },
		{ "Numpad8", eKeyCodeNumpad8 },
		{ "Numpad9", eKeyCodeNumpad9 },
		{ "Subtract", eKeyCodeSubtract },
		{ "Add", eKeyCodeAdd },
		{ "NumpadEnter", eKeyCodeNumpadEnter },
		{ "Decimal", eKeyCodeDecimal },
		{ "Shift", eKeyCodeShift },
		{ "Ctrl", eKeyCodeCtrl },
		{ "Alt", eKeyCodeAlt },
	};

	Utils::NameValueTable<MouseButton> mouseButtons =
	{
		{ "MouseLeft", eMouseButtonLeft },
		{ "MouseMiddle", eMouseButtonMiddle },
		{ "MouseRight", eMouseButtonRight },
		{ "Mouse4", eMouseButton4 },
		{ "Mouse5", eMouseButton5 },
		{ "MouseWheelUp", eMouseButtonWheelUp },
		{ "MouseWheelDown", eMouseButtonWheelDown },
	};

	Utils::NameValueTable<GameAction> gameActions =
	{
		// Movement
		{ "Forward", eGameActionMoveForward },
		{ "Back", eGameActionMoveBack },
		{ "Left", eGameActionMoveLeft },
		{ "Right", eGameActionMoveRight },
		{ "Sprint", eGameActionSprint },
		{ "Jump", eGameActionJump },
		{ "Crouch", eGameActionCrouch },
		{ "Use", eGameActionUse },
		{ "DualWield", eGameActionPickUpLeft },
		{ "UseEquipment", eGameActionUseConsumable1 },

		// Combat
		{ "Fire", eGameActionFireRight },
		{ "FireLeft", eGameActionFireLeft },
		{ "Reload", eGameActionReloadRight },
		{ "ReloadLeft", eGameActionReloadLeft },
		{ "Zoom", eGameActionZoom },
		{ "SwitchWeapons", eGameActionSwitchWeapons },
		{ "Melee", eGameActionMelee },
		{ "Grenade", eGameActionThrowGrenade },
		{ "SwitchGrenades", eGameActionSwitchGrenades },
		{ "Flashlight", eGameActionFlashlight },

		// Vehicles
		{ "VehicleAccelerate", eGameActionVehicleAccelerate },
		{ "VehicleBrake", eGameActionVehicleBrake },
		{ "VehicleBoost", eGameActionVehicleBoost },
		{ "VehicleRaise", eGameActionVehicleRaise },
		{ "VehicleDive", eGameActionVehicleDive },
		{ "VehicleFire", eGameActionVehicleFire },
		{ "VehicleAltFire", eGameActionVehicleAltFire },
		{ "BansheeBomb", eGameActionMeleeFire },

		// UI
		{ "Menu", eGameActionUiStart },
		{ "Scoreboard", eGameActionUiSelect },

		{ "Chat", eGameActionGeneralChat },
		{ "TeamChat", eGameActionTeamChat },
		{ "VoiceChat", eGameActionVoiceChat },
		{ "UiLeftTrigger", eGameActionUiLeftTrigger },
		{ "UiRightTrigger", eGameActionUiRightTrigger },
		{ "UiUp", eGameActionUiUp },
		{ "UiDown", eGameActionUiDown },
		{ "UiLeft", eGameActionUiLeft },
		{ "UiRight", eGameActionUiRight },
		{ "UiLeftStick", eGameActionUiLeftStick },
		{ "UiRightStick", eGameActionUiRightStick },
		{ "UiA", eGameActionUiA },
		{ "UiB", eGameActionUiB },
		{ "UiX", eGameActionUiX },
		{ "UiY", eGameActionUiY },
		{ "UiLeftBumper", eGameActionUiLeftBumper },
		{ "UiRightBumper", eGameActionUiRightBumper },

		// Observer 
		{ "NextPlayer", eGameActionNextPlayer },
		{ "PrevPlayer", eGameActionPrevPlayer },
	};

	// Macro to prefix a string with "settings_" because the actual string_ids don't include that prefix
	#define S(name) ("settings_" name)

	// Default keys for these menus are populated in BuildSettingsMenu

	// Infantry controls settings menu
	std::vector<ConfigurableAction> infantryControls =
	{
		{ S("actions_move_forward"),        -1, -1, eGameActionMoveForward },
		{ S("actions_move_backward"),       -1, -1, eGameActionMoveBack },
		{ S("actions_move_left"),           -1, -1, eGameActionMoveLeft },
		{ S("actions_move_right"),          -1, -1, eGameActionMoveRight },
		{ S("actions_sprint"),              -1, -1, eGameActionSprint },
		{ S("actions_jump"),                -1, -1, eGameActionJump },
		{ S("actions_crouch"),              -1, -1, eGameActionCrouch },
		{ S("actions_use"),                 -1, -1, eGameActionUse },
		{ S("actions_dualwield"),           -1, -1, eGameActionPickUpLeft },
		{ S("actions_shoot"),               -1, -1, eGameActionFireRight },
		{ S("actions_shoot_left"),          -1, -1, eGameActionFireLeft },
		{ S("actions_reload"),              -1, -1, eGameActionReloadRight },
		{ S("actions_reload_left"),         -1, -1, eGameActionReloadLeft },
		{ S("actions_zoom"),                -1, -1, eGameActionZoom },
		{ S("actions_switch_weapons"),      -1, -1, eGameActionSwitchWeapons },
		{ S("actions_melee"),               -1, -1, eGameActionMelee },
		{ S("actions_throw_grenade"),       -1, -1, eGameActionThrowGrenade },
		{ S("actions_toggle_grenade_type"), -1, -1, eGameActionSwitchGrenades },
		{ S("actions_show_scores"),         -1, -1, eGameActionUiSelect },
		{ S("actions_forge_delete"),        -1, -1, eGameActionUiY },
		{ S("actions_chat_all"),            -1, -1, eGameActionGeneralChat },
		{ S("actions_chat_team"),           -1, -1, eGameActionTeamChat },
		{ S("actions_chat_voice"),          -1, -1, eGameActionVoiceChat },
	};

	// Vehicle controls settings menu
	std::vector<ConfigurableAction> vehicleControls =
	{
		{ S("actions_vehicle_accelerate"), -1, -1, eGameActionVehicleAccelerate },
		{ S("actions_vehicle_reverse"),    -1, -1, eGameActionVehicleBrake },
		{ S("actions_vehicle_boost"),      -1, -1, eGameActionVehicleBoost },
		{ S("actions_vehicle_raise"),      -1, -1, eGameActionVehicleRaise },
		{ S("actions_vehicle_dive"),       -1, -1, eGameActionVehicleDive },
		{ S("actions_vehicle_fire"),       -1, -1, eGameActionVehicleFire },
		{ S("actions_vehicle_alt_fire"),   -1, -1, eGameActionVehicleAltFire },
		{ S("actions_vehicle_bansheebomb"),-1, -1, eGameActionMeleeFire },
	};
}