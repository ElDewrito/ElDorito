#include "Godmode.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Godmode::Godmode()
{
	enabled = false;

	// original bytes
	byte reset[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	byte god[] = { 0xF3, 0x0F, 0x11, 0x97, 0x00, 0x01, 0x00, 0x00 };
}

Godmode::~Godmode()
{
}

std::string Godmode::Info()
{
	std::cout << "Godmode: " << (enabled ? "Enabled" : "Disabled") << std::endl;

	std::string Info = "Invulnerable to damage\n";

	return Info;
}

void Godmode::Tick(const std::chrono::duration<double>& Delta)
{
}

void Godmode::Run(const std::vector<std::string>& Args)
{
	const size_t Offset = 0x75636A;
	const uint8_t reset[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	const uint8_t god[8] = { 0xF3, 0x0F, 0x11, 0x97, 0x00, 0x01, 0x00, 0x00 };

	if(Args.size() >= 2)
	{
		if(Args[1].compare("off") == 0)
		{
			// Disable it.
			std::cout << "Disabling god map" << std::endl;
			enabled = false;
			memcpy(((uint8_t*)GetModuleBase()) + Offset, reset, sizeof(reset));
		}
		else if(Args[1].compare("on") == 0)
		{
			// Enable
			std::cout << "Enabling god map" << std::endl;
			enabled = true;
			memcpy(((uint8_t*)GetModuleBase()) + Offset, god, sizeof(god));
		}
	}
	return;
}