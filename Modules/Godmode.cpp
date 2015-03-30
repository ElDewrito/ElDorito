#include "Godmode.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Godmode::Godmode()
	:
	enabled(false),
	GodPatch(
	0x7555DC,
	{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
	{ 0xF3, 0x0F, 0x11, 0x97, 0x00, 0x01, 0x00, 0x00 })
{
}

Godmode::~Godmode()
{
}

std::string Godmode::Info()
{
	std::string Info = "Godmode: ";
	Info += (enabled ? "Enabled" : "Disabled");
	Info += "\nUsage: god (on|off)";
	Info += "\nEnables godmod, scripted deaths still occur(such as falling)\n";
	return Info;
}

void Godmode::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Godmode::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			std::cout << "Disabling god" << std::endl;
			enabled = false;
			GodPatch.Reset();
			return true;
		}
		else if( Args[1].compare("on") == 0 )
		{
			std::cout << "Enabling god" << std::endl;
			enabled = true;
			GodPatch.Apply();
			return true;
		}
	}

	return false;
}