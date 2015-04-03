#include "Hud.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Hud::Hud() : enabled(false)
{
}

Hud::~Hud()
{
}

std::string Hud::Info() const
{
	std::string Info =
		"View Hud: ";
	Info += ( enabled ? "Enabled" : "Disabled" );
	Info += "\nUsage: hud (on|off)\n"
		"Turns hud on and off\n"
		"Does not remove watermark if Hide_Watermark is set to 0 in game.cfg\n";
	return Info;
}

std::string Hud::Suggest(const std::vector<std::string>& Arguments) const
{
	if( Arguments.size() == 2 )
	{
		if( Arguments[1].empty() )
		{
			return ( enabled ? "off" : "on" );
		}
	}
	return "";
}

void Hud::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Hud::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 2 )
	{
		DWORD temp;
		if( !Args[1].compare("off") )
		{
			std::cout << "Disabling hud" << std::endl;
			enabled = false;
			VirtualProtect(Pointer(0x016B5A5C), 4, PAGE_READWRITE, &temp);
			Pointer(0x016B5A5C).Write<float>(3.14f);
			return true;
		}
		else if( !Args[1].compare("on") )
		{
			std::cout << "Enabling hud" << std::endl;
			enabled = true;
			VirtualProtect(Pointer(0x016B5A5C), 4, PAGE_READWRITE, &temp);
			Pointer(0x016B5A5C).Write<uint32_t>(0x3F83DDE2);
			return true;
		}
	}

	return false;
}