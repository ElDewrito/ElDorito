#include "Disclaimer.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Disclaimer::Disclaimer()
{
}

Disclaimer::~Disclaimer()
{
}

std::string Disclaimer::Info(const std::string& Topic) const
{
	return "This module is intended to print out a disclaimer involving the \'ElDorito\' software.";
}

std::string Disclaimer::Suggest(const std::vector<std::string>& Arguments) const
{
	return "";
}

void Disclaimer::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Disclaimer::Run(const std::vector<std::string>& Args)
{
	std::string Info;

	Info += "Hello, this is a statement from a group of modders working on \"Halo Online\".\n"
		"First things, we are not a completely organized group. We consist of various people researching and working on different things to do with Halo Online, some of us are looking into how the new file formats work and some of us are looking into how the game functions.\n"
		"We are not known as the \"ElDorito\" group, or known as any name really, we're simply a collective of people who enjoy Halo games and would like to work on mods for the only Halo PC release in the past ~8 years.\n"
		"A small part of the group is also working on a tool known as \"ElDorito\", which has gained a lot of attention in the press in the last few weeks.\n"
		"Unfortunately due to people making statements(without consulting everybody) and the media misquoting them a lot of misinformation has been given in the last week, and presented as fact by people who are against the project.\n"
		"To start: ElDorito is not a tool to \"remove micro-transactions\", ElDorito is simply a tool that lets you explore the games maps offline and do various mods to them.\n"
		"If you connected to the games servers microtransactions would still be available for you to purchase.\n"
		"ElDorito is not a piracy tool neither, the game itself is available Free2Play with no obligations to purchase it, we simply make this Free2Play element available to people offline.\n"
		"No cracks/activation hacks are used, as there are no payment-checks to crack, besides the microtransactions which we have not modified."
		"This software makes no attempt to impede upon the online access controls that \"Halo Online\" provides.\n"
		"(April 8, 2015)";

	std::vector < std::string> Wrapped = Utils::String::Wrap(Info, 79);

	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD ConsoleMode;
	GetConsoleMode(hStd, &ConsoleMode);

	//re-enable wrapping.
	ConsoleMode |= ENABLE_WRAP_AT_EOL_OUTPUT;
	SetConsoleMode(hStd, ConsoleMode);

	for( auto Line : Wrapped )
	{
		std::cout << Line << std::endl;
	}

	ConsoleMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
	SetConsoleMode(hStd, ConsoleMode);

	return true;
}

void Disclaimer::PreferencesChanged()
{
}