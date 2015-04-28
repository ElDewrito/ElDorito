#include "RawInput.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"
#include "../ElPreferences.h"

#include <iostream>

RawInput::RawInput()
{
}

RawInput::~RawInput()
{
}

std::string RawInput::Info(const std::string& Topic) const
{
	std::string Info = "Raw Input: ";
	Info += (ElPreferences::Instance().getRawMouse() ? "Enabled" : "Disabled");
	Info += "\nUsage: rawinput (on|off)\n"
		"Bypasses the games mouse processing with our own RawInput implementation.";

	return Info;
}

std::string RawInput::Suggest(const std::vector<std::string>& Arguments) const
{
	if (Arguments.size() == 2)
	{
		if (Arguments[1].empty())
		{
			return (ElPreferences::Instance().getRawMouse() ? "off" : "on");
		}
	}
	return "";
}

void RawInput::Tick(const std::chrono::duration<double>& Delta)
{
}

bool RawInput::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		if (Args[1].compare("off") == 0)
		{
			ElPreferences::Instance().setRawMouse(false);
			ElPreferences::Instance().save();

			std::cout << "Raw input disabled.\n" << std::endl;
			return true;
		}
		else if (Args[1].compare("on") == 0)
		{
			ElPreferences::Instance().setRawMouse(true);
			ElPreferences::Instance().save();

			std::cout << "Raw input enabled.\n" << std::endl;
			return true;
		}
	}

	return false;
}