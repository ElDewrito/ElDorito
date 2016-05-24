#include "WebScoreboard.hpp"
#include "ScreenLayer.hpp"
#include "../../Blam/BlamNetwork.hpp"
#include "../../Patches/Input.hpp"
#include "../../Pointer.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"

#include <iomanip>

using namespace Blam::Input;

namespace
{
	void OnGameInputUpdated();
}

namespace Web
{
	namespace Ui
	{
		namespace WebScoreboard
		{
			void Init()
			{
				Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
			}

			void Show(bool locked)
			{
				rapidjson::StringBuffer jsonBuffer;
				rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

				jsonWriter.StartObject();
				jsonWriter.Key("locked");
				jsonWriter.Bool(locked);
				jsonWriter.EndObject();

				ScreenLayer::Show("scoreboard", jsonBuffer.GetString());
				//Only capture if the scoreboard is locked
				ScreenLayer::CaptureInput(locked);
			}

			void Hide()
			{
				ScreenLayer::Hide("scoreboard");
			}
		}
	}
}

namespace
{
	bool locked = false;

	void OnGameInputUpdated()
	{
		BindingsTable bindings;
		GetBindings(0, &bindings);

		if (GetKeyTicks(bindings.PrimaryKeys[eGameActionUiSelect], eInputTypeUi) == 1 || GetKeyTicks(bindings.SecondaryKeys[eGameActionUiSelect], eInputTypeUi) == 1)
		{
			if (strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") != 0)
			{
				//If shift is held down then lock the scoreboard
				if (GetKeyTicks(eKeyCodeShift, eInputTypeUi) == 0)
					locked = false;
				else
					locked = true;
				Web::Ui::WebScoreboard::Show(locked);
			}
		}

		if (!locked)
		{
			//Hide the scoreboard when you release tab. Only check when the scoreboard isn't locked.
			if (GetKeyTicks(bindings.PrimaryKeys[eGameActionUiSelect], eInputTypeUi) == 0 && GetKeyTicks(bindings.SecondaryKeys[eGameActionUiSelect], eInputTypeUi) == 0)
			{
				Web::Ui::WebScoreboard::Hide();
				locked = false;
			}
		}

	}
}