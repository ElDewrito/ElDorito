#include "Hud.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Hud::Hud()
{
	enabled = false;
}

Hud::~Hud()
{
}

std::string Hud::Info()
{
	std::string Info =
		"View Hud: ";
	Info += (enabled ? "Enabled" : "Disabled");
	Info += "Usage: hud on|off\n"
		"Turns hud on and off\n"
		"Does not remove watermark if Hide_Watermark is set to 1 in game.cfg\n";

	return Info;
}

void Hud::Tick(const std::chrono::duration<double>& Delta)
{
}

void Hud::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 2 )
	{
		DWORD temp;
		if( !Args[1].compare("off") )
		{
			enabled = false;
			VirtualProtect(Pointer(0x016B5A5C), 4, PAGE_READWRITE, &temp);
			Pointer(0x016B5A5C).Write<float>(3.14f);
		}
		else if( !Args[1].compare("on") )
		{
			enabled = true;
			VirtualProtect(Pointer(0x016B5A5C), 4, PAGE_READWRITE, &temp);
			Pointer(0x016B5A5C).Write<uint32_t>(0x3F83DDE2);
		}
		else
		{
			std::cout << "Unknown option: " << Args[1] << std::endl;
		}
	}
	else
	{
		std::cout << Info() << std::endl;
	}
	return;
}