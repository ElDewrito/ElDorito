#include "ModuleInput.h"
#include <sstream>
#include "../ElDorito.h"

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
}
