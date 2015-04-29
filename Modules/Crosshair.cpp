#include "Crosshair.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"
#include "../ElPreferences.h"
#include "../Patches/Ui.h"

#include <iostream>

Crosshair::Crosshair()
{
}

Crosshair::~Crosshair()
{
}

std::string Crosshair::Info(const std::string& Topic) const
{
	std::string Info = "Crosshair mode: ";
	Info += (ElPreferences::Instance().getCrosshairCentered() ? "Centered" : "Normal");
	Info += "\nUsage: crosshair (center|normal)\n"
		"Allows the player to change the crosshair position.";

	return Info;
}

std::string Crosshair::Suggest(const std::vector<std::string>& Arguments) const
{
	if (Arguments.size() == 2)
	{
		if (Arguments[1].empty())
		{
			return (ElPreferences::Instance().getCrosshairCentered() ? "normal" : "center");
		}
	}
	return "";
}

void Crosshair::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Crosshair::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		if (Args[1].compare("off") == 0 || Args[1].compare("default") == 0 || Args[1].compare("normal") == 0)
		{
			ElPreferences::Instance().setCrosshairCentered(false);
			ElPreferences::Instance().save();

			Patches::Ui::EnableCenteredCrosshairPatch(false);

			std::cout << "Crosshair positioning set to normal.\n" << std::endl;
			return true;
		}
		else if (Args[1].compare("on") == 0 || Args[1].compare("center") == 0 || Args[1].compare("centered") == 0)
		{
			ElPreferences::Instance().setCrosshairCentered(true);
			ElPreferences::Instance().save();

			Patches::Ui::EnableCenteredCrosshairPatch(true);

			std::cout << "Crosshair positioning set to centered.\n" << std::endl;
			return true;
		}
	}

	return false;
}