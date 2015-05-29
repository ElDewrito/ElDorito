#include "ModuleInput.h"
#include <sstream>
#include "../ElDorito.h"
#include "../ElPreferences.h"

namespace Modules
{
	ModuleInput::ModuleInput() : ModuleBase("Input")
	{
		VarInputRawInput = AddVariableInt("RawInput", "rawinput", "Enables raw mouse input with no acceleration applied", 1, VariableInputRawInputUpdate);
		VarInputRawInput->ValueIntMin = 0;
		VarInputRawInput->ValueIntMax = 1;
	}
}

namespace
{
	std::string VariableInputRawInputUpdate(const std::vector<std::string>& Arguments)
	{
		unsigned long value = Modules::ModuleInput::Instance().VarInputRawInput->ValueInt;

		std::string status = "disabled.";
		bool statusBool = value != 0;
		if (statusBool)
			status = "enabled.";

		ElPreferences::Instance().setRawMouse(statusBool);
		ElPreferences::Instance().save();

		return "Raw input " + status;
	}
}
