#include "Godmode.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Godmode::Godmode()
{
	enabled = false;
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
	const size_t OffsetShield = 0x75636A;
	const size_t OffsetHealth = 0x7555DC;
	
	// Nop elements (Patch)
	const uint8_t god[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	// Set
	const uint8_t resetShield[8] = { 0xF3, 0x0F, 0x11, 0x97, 0x00, 0x01, 0x00, 0x00 };
	const uint8_t resetHealth[8] = { 0xF3, 0x0F, 0x11, 0x97, 0x00, 0x01, 0x00, 0x00 };



	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			// Disable it.
			std::cout << "Disabling god map" << std::endl;
			enabled = false;
			memcpy(((uint8_t*)GetModuleBase()) + OffsetShield, resetShield, sizeof(resetShield));
			memcpy(((uint8_t*)GetModuleBase()) + OffsetHealth, resetHealth, sizeof(resetHealth));
		}
		else if( Args[1].compare("on") == 0 )
		{
			// Enable
			std::cout << "Enabling god map" << std::endl;
			enabled = true;
			memcpy(((uint8_t*)GetModuleBase()) + OffsetShield, god, sizeof(god));
			memcpy(((uint8_t*)GetModuleBase()) + OffsetHealth, god, sizeof(god));
		}
	}
	return;
}