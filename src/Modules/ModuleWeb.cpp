#include "ModuleWeb.hpp"
#include "../Web/Ui/ScreenLayer.hpp"

namespace Modules
{
	bool CommandShowScreen(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		if (arguments.size() < 1)
		{
			returnInfo = "Usage: Web.ShowScreen <screen name>";
			return false;
		}
			
		Web::Ui::ScreenLayer::Show(arguments[0], "{}");
		return true;
	}

	bool CommandHideScreen(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		if (arguments.size() < 1)
		{
			returnInfo = "Usage: Web.HideScreen <screen name>";
			return false;
		}

		Web::Ui::ScreenLayer::Hide(arguments[0]);
		return true;
	}

	ModuleWeb::ModuleWeb() : ModuleBase("Web")
	{
		AddCommand("ShowScreen", "showscreen", "Displays a screen by it's name", eCommandFlagsNone, CommandShowScreen);
		AddCommand("HideScreen", "hidescreen", "Hides a screen by it's name", eCommandFlagsNone, CommandHideScreen);
	}
}