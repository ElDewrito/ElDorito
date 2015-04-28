#include "RawInput.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"
#include "../Patches/RawInput.h"

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
	Info += (Patches::RawInput::IsEnabled() ? "Enabled" : "Disabled");
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
			return (Patches::RawInput::IsEnabled() ? "off" : "on");
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
			Patches::RawInput::Enable(false);
			std::cout << "Raw input disabled.\n" << std::endl;
			return true;
		}
		else if (Args[1].compare("on") == 0)
		{
			Patches::RawInput::Enable(true);
			std::cout << "Raw input enabled.\n" << std::endl;
			return true;
		}
	}

	return false;
}