#include "ScreenLayer.hpp"
#include "../../Patches/Input.hpp"

#include "../../Modules/ModuleInput.hpp"
#include "../../Modules/ModuleSettings.hpp"

namespace
{
	void OnGameInputUpdated();
}

namespace Web::Ui::WebSettings
{
	void Init()
	{
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
	}

	void Show()
	{
		ScreenLayer::Show("settings", "{}");
	}
}

namespace
{
	void OnGameInputUpdated()
	{
		if (Modules::ModuleSettings::Instance().VarGamepadEnabled->ValueInt == 1)
		{
			if(Blam::Input::GetActionState(Blam::Input::eGameActionUiStart)->Ticks == 1 && strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") == 0)
				Web::Ui::WebSettings::Show();
		}
		
		if(GetKeyTicks(Blam::Input::eKeyCodeHome, Blam::Input::eInputTypeUi) && strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") == 0)
			Web::Ui::WebSettings::Show();
	}
}
