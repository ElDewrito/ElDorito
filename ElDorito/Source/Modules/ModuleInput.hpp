#pragma once

#include "ModuleBase.hpp"

namespace Blam
{
	namespace Input
	{
		struct BindingsTable;
	}
}

namespace Modules
{
	class ModuleInput : public Utils::Singleton<ModuleInput>, public ModuleBase
	{
	public:
		Command* VarInputRawInput;
		Command* VarInputControllerPort;
		Command* VarControllerSensitivityX;
		Command* VarControllerSensitivityY;
		Command* VarControllerVibrationIntensity;
		Command* VarControllerInvertY;
		Command* VarControllerStickLayout;
		Command* VarSpectateSensitivity;
		Command* VarToggleSprint;
		Command* VarTapScoreboard;

		Command* CommandBindControllerAction;
		Command* CommandBindKeyboardAction;

		ModuleInput();

		static Blam::Input::BindingsTable *GetBindings();
		static void UpdateBindings();
		std::string ExportBindings() const;
		bool IsCommandBound(std::string command);
	};
}
