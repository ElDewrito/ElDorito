#include "ScreenLayer.hpp"
#include "../../Patches/Input.hpp"
#include "../../Pointer.hpp"

#include "Xinput.h"
#include "../../Modules/ModuleInput.hpp"
#include <boost/thread/futures/future_status.hpp>

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
		// Due to start being bound to escape on the keyboard we can't use action states.
		XINPUT_STATE state;
		XInputGetState(0, &state);

		if (((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0 || GetKeyTicks(Blam::Input::eKeyCodeHome, Blam::Input::eInputTypeUi)) && strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") == 0)
			Web::Ui::WebSettings::Show();
	}
}
