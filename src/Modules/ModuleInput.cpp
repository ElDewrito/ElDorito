#include "ModuleInput.hpp"
#include <sstream>
#include "../ElDorito.hpp"
#include "../Patches/Input.hpp"
#include "../Console/GameConsole.hpp"
#include "../Blam/BlamInput.hpp"

namespace
{
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

	// Maps key names to key code values
	extern std::map<std::string, Blam::Input::KeyCodes> keyCodes;

	// Holds information about a command bound to a key
	struct KeyBinding
	{
		std::vector<std::string> command; // If this is empty, no command is bound
		bool isHold; // True if the command binds to a boolean variable
		bool active; // True if this is a hold command and the key is down
	};

	// Bindings for each key
	KeyBinding bindings[Blam::Input::eKeyCodes_Count];

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

		// Get the key, convert it to lowercase, and translate it to a key code
		auto key = Arguments[0];
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		auto it = keyCodes.find(key);
		if (it == keyCodes.end())
		{
			returnInfo = "Unrecognized key name: " + key;
			return false;
		}
		auto keyCode = it->second;
		auto binding = &bindings[keyCode];

		// If no command was specified, unset the binding
		if (Arguments.size() == 1)
		{
			binding->command.clear();
			returnInfo = "Binding cleared.";
			return true;
		}

		// Get command information
		auto command = Arguments[1];
		if (command.length() == 0)
		{
			returnInfo = "Invalid command";
			return false;
		}
		auto isHold = false;
		if (command[0] == '+')
		{
			command.erase(0, 1);
			isHold = true;
		}

		// Set the binding
		binding->isHold = isHold;
		binding->command.clear();
		binding->command.push_back(command);
		binding->command.insert(binding->command.end(), Arguments.begin() + 2, Arguments.end());
		returnInfo = "Binding set.";
		return true;
	}

	void KeyboardUpdated()
	{
		for (auto i = 0; i < Blam::Input::eKeyCodes_Count; i++)
		{
			const auto binding = &bindings[i];
			if (binding->command.size() == 0)
				continue; // Key is not bound

			// Read the key
			auto keyCode = static_cast<Blam::Input::KeyCodes>(i);
			auto keyTicks = GetKeyTicks(keyCode, Blam::Input::eInputTypeSpecial);

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
			GameConsole::Instance().consoleQueue.pushLineFromGameToUIMultipleLines(result);
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
		keyPressData[2] = std::stoul(Arguments[0], 0, 0); // button idx, corresponds with Blam::ButtonCodes
		keyPressData[3] = 0xFF;

		typedef void*(__cdecl *sub_AAD930Ptr)();
		auto sub_AAD930 = reinterpret_cast<sub_AAD930Ptr>(0xAAD930);
		void* classPtr = sub_AAD930();

		typedef int(__thiscall *sub_AAB7D0Ptr)(void* thisPtr, void* a2);
		auto sub_AAB7D0 = reinterpret_cast<sub_AAB7D0Ptr>(0xAAB7D0);
		int retVal = sub_AAB7D0(classPtr, keyPressData);

		return retVal != 0;
	}

	// TODO: Remove these once the action enum is mapped out better

#ifdef _DEBUG
	bool CommandBindAction(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() != 2)
			return false;

		// Get the key, convert it to lowercase, and translate it to a key code
		auto action = std::stoi(Arguments[0]);
		auto key = Arguments[1];
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		auto it = keyCodes.find(key);
		if (it == keyCodes.end())
		{
			returnInfo = "Unrecognized key name: " + key;
			return false;
		}
		auto keyCode = it->second;
		Blam::Input::GetBindings(0)->PrimaryKeys[action] = keyCode;
		Blam::Input::GetBindings(0)->SecondaryKeys[action] = keyCode;
		returnInfo = "Binding set.";
		return true;
	}

	bool CommandDumpBindings(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		returnInfo = "";
		auto bindings = Blam::Input::GetBindings(0);
		for (auto i = 0; i < Blam::Input::eInputAction_KeyboardMouseCount; i++)
		{
			auto primary = bindings->PrimaryKeys[i];
			auto secondary = bindings->SecondaryKeys[i];
			auto mousePrimary = bindings->PrimaryMouseButtons[i];
			auto mouseSecondary = bindings->SecondaryMouseButtons[i];
			returnInfo += std::to_string(i);
			if (primary < Blam::Input::eKeyCodes_Count)
			{
				std::string name = "Unknown";
				for (auto it = keyCodes.begin(); it != keyCodes.end(); ++it)
				{
					if (it->second == primary)
						name = it->first;
				}
				returnInfo += " " + name;
			}
			if (secondary < Blam::Input::eKeyCodes_Count)
			{
				std::string name = "Unknown";
				for (auto it = keyCodes.begin(); it != keyCodes.end(); ++it)
				{
					if (it->second == secondary)
						name = it->first;
				}
				returnInfo += " " + name;
			}
			if (mousePrimary < Blam::Input::eMouseButtons_Count)
				returnInfo += " m" + std::to_string(mousePrimary);
			if (mouseSecondary < Blam::Input::eMouseButtons_Count)
				returnInfo += " m" + std::to_string(mouseSecondary);
			returnInfo += "\n";
		}
		return true;
	}
#endif
}

namespace Modules
{
	ModuleInput::ModuleInput() : ModuleBase("Input")
	{
		VarInputRawInput = AddVariableInt("RawInput", "rawinput", "Enables raw mouse input with no acceleration applied", eCommandFlagsArchived, 1, VariableInputRawInputUpdate);
		VarInputRawInput->ValueIntMin = 0;
		VarInputRawInput->ValueIntMax = 1;

		AddCommand("Bind", "bind", "Binds a command to a key", eCommandFlagsNone, CommandBind, { "key", "[+]command", "arguments" });
		Patches::Input::RegisterDefaultInputHandler(KeyboardUpdated);

		AddCommand("UIButtonPress", "ui_btn_press", "Emulates a gamepad button press on UI menus", eCommandFlagsNone, CommandUIButtonPress, { "btnCode The code of the button to press" });

#ifdef _DEBUG
		AddCommand("DumpBindings", "dumpbindings", "Test", eCommandFlagsNone, CommandDumpBindings);
		AddCommand("BindAction", "action", "Test", eCommandFlagsNone, CommandBindAction, { "action", "key" });
#endif

		// Default keybindings (TODO: port bind saving code from recode)
		bindings[Blam::Input::eKeyCodesEnter].command = { "ui_btn_press", "0" };  // A
		bindings[Blam::Input::eKeyCodesSpace].command = { "ui_btn_press", "0" };  // A
		bindings[Blam::Input::eKeyCodesEscape].command = { "ui_btn_press", "1" }; // B
		bindings[Blam::Input::eKeyCodesBack].command = { "ui_btn_press", "1" };   // B
		bindings[Blam::Input::eKeyCodesEnd].command = { "ui_btn_press", "1" };    // B
		bindings[Blam::Input::eKeyCodesHome].command = { "ui_btn_press", "2" };   // X
		bindings[Blam::Input::eKeyCodesDelete].command = { "forge_delete" };
	}
}

namespace
{
	// Key codes table
	std::map<std::string, Blam::Input::KeyCodes> keyCodes =
	{
		{ "escape", Blam::Input::eKeyCodesEscape },
		{ "f1", Blam::Input::eKeyCodesF1 },
		{ "f2", Blam::Input::eKeyCodesF2 },
		{ "f3", Blam::Input::eKeyCodesF3 },
		{ "f4", Blam::Input::eKeyCodesF4 },
		{ "f5", Blam::Input::eKeyCodesF5 },
		{ "f6", Blam::Input::eKeyCodesF6 },
		{ "f7", Blam::Input::eKeyCodesF7 },
		{ "f8", Blam::Input::eKeyCodesF8 },
		{ "f9", Blam::Input::eKeyCodesF9 },
		{ "f10", Blam::Input::eKeyCodesF10 },
		{ "f11", Blam::Input::eKeyCodesF11 },
		{ "f12", Blam::Input::eKeyCodesF12 },
		{ "printscreen", Blam::Input::eKeyCodesPrintScreen },
		{ "f14", Blam::Input::eKeyCodesF14 },
		{ "f15", Blam::Input::eKeyCodesF15 },
		{ "tilde", Blam::Input::eKeyCodesTilde },
		{ "1", Blam::Input::eKeyCodes1 },
		{ "2", Blam::Input::eKeyCodes2 },
		{ "3", Blam::Input::eKeyCodes3 },
		{ "4", Blam::Input::eKeyCodes4 },
		{ "5", Blam::Input::eKeyCodes5 },
		{ "6", Blam::Input::eKeyCodes6 },
		{ "7", Blam::Input::eKeyCodes7 },
		{ "8", Blam::Input::eKeyCodes8 },
		{ "9", Blam::Input::eKeyCodes9 },
		{ "0", Blam::Input::eKeyCodes0 },
		{ "minus", Blam::Input::eKeyCodesMinus },
		{ "plus", Blam::Input::eKeyCodesPlus },
		{ "back", Blam::Input::eKeyCodesBack },
		{ "tab", Blam::Input::eKeyCodesTab },
		{ "q", Blam::Input::eKeyCodesQ },
		{ "w", Blam::Input::eKeyCodesW },
		{ "e", Blam::Input::eKeyCodesE },
		{ "r", Blam::Input::eKeyCodesR },
		{ "t", Blam::Input::eKeyCodesT },
		{ "y", Blam::Input::eKeyCodesY },
		{ "u", Blam::Input::eKeyCodesU },
		{ "i", Blam::Input::eKeyCodesI },
		{ "o", Blam::Input::eKeyCodesO },
		{ "p", Blam::Input::eKeyCodesP },
		{ "lbracket", Blam::Input::eKeyCodesLBracket },
		{ "rbracket", Blam::Input::eKeyCodesRBracket },
		{ "pipe", Blam::Input::eKeyCodesPipe },
		{ "capital", Blam::Input::eKeyCodesCapital },
		{ "a", Blam::Input::eKeyCodesA },
		{ "s", Blam::Input::eKeyCodesS },
		{ "d", Blam::Input::eKeyCodesD },
		{ "f", Blam::Input::eKeyCodesF },
		{ "g", Blam::Input::eKeyCodesG },
		{ "h", Blam::Input::eKeyCodesH },
		{ "j", Blam::Input::eKeyCodesJ },
		{ "k", Blam::Input::eKeyCodesK },
		{ "l", Blam::Input::eKeyCodesL },
		{ "colon", Blam::Input::eKeyCodesColon },
		{ "quote", Blam::Input::eKeyCodesQuote },
		{ "enter", Blam::Input::eKeyCodesEnter },
		{ "lshift", Blam::Input::eKeyCodesLShift },
		{ "z", Blam::Input::eKeyCodesZ },
		{ "x", Blam::Input::eKeyCodesX },
		{ "c", Blam::Input::eKeyCodesC },
		{ "v", Blam::Input::eKeyCodesV },
		{ "b", Blam::Input::eKeyCodesB },
		{ "n", Blam::Input::eKeyCodesN },
		{ "m", Blam::Input::eKeyCodesM },
		{ "comma", Blam::Input::eKeyCodesComma },
		{ "period", Blam::Input::eKeyCodesPeriod },
		{ "question", Blam::Input::eKeyCodesQuestion },
		{ "rshift", Blam::Input::eKeyCodesRShift },
		{ "lcontrol", Blam::Input::eKeyCodesLControl },
		{ "lalt", Blam::Input::eKeyCodesLAlt },
		{ "space", Blam::Input::eKeyCodesSpace },
		{ "ralt", Blam::Input::eKeyCodesRAlt },
		{ "apps", Blam::Input::eKeyCodesApps },
		{ "rcontrol", Blam::Input::eKeyCodesRcontrol },
		{ "up", Blam::Input::eKeyCodesUp },
		{ "down", Blam::Input::eKeyCodesDown },
		{ "left", Blam::Input::eKeyCodesLeft },
		{ "right", Blam::Input::eKeyCodesRight },
		{ "insert", Blam::Input::eKeyCodesInsert },
		{ "home", Blam::Input::eKeyCodesHome },
		{ "pageup", Blam::Input::eKeyCodesPageUp },
		{ "delete", Blam::Input::eKeyCodesDelete },
		{ "end", Blam::Input::eKeyCodesEnd },
		{ "pagedown", Blam::Input::eKeyCodesPageDown },
		{ "numlock", Blam::Input::eKeyCodesNumLock },
		{ "divide", Blam::Input::eKeyCodesDivide },
		{ "multiply", Blam::Input::eKeyCodesMultiply },
		{ "numpad0", Blam::Input::eKeyCodesNumpad0 },
		{ "numpad1", Blam::Input::eKeyCodesNumpad1 },
		{ "numpad2", Blam::Input::eKeyCodesNumpad2 },
		{ "numpad3", Blam::Input::eKeyCodesNumpad3 },
		{ "numpad4", Blam::Input::eKeyCodesNumpad4 },
		{ "numpad5", Blam::Input::eKeyCodesNumpad5 },
		{ "numpad6", Blam::Input::eKeyCodesNumpad6 },
		{ "numpad7", Blam::Input::eKeyCodesNumpad7 },
		{ "numpad8", Blam::Input::eKeyCodesNumpad8 },
		{ "numpad9", Blam::Input::eKeyCodesNumpad9 },
		{ "subtract", Blam::Input::eKeyCodesSubtract },
		{ "add", Blam::Input::eKeyCodesAdd },
		{ "numpadenter", Blam::Input::eKeyCodesNumpadEnter },
		{ "decimal", Blam::Input::eKeyCodesDecimal },
		{ "shift", Blam::Input::eKeyCodesShift },
		{ "ctrl", Blam::Input::eKeyCodesCtrl },
		{ "alt", Blam::Input::eKeyCodesAlt },
	};
}