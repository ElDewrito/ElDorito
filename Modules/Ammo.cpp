#include "Ammo.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Ammo::Ammo()
{
	enabled = false;
}

Ammo::~Ammo()
{
}

std::string Ammo::Info()
{
	std::cout << "Unlimited ammo: " << (enabled ? "Enabled" : "Disabled") << std::endl;

	std::string Info = "Bottomless clip on weapon ammo and grenades\n";

	return Info;
}

void Ammo::Tick(const std::chrono::duration<double>& Delta)
{
}

void Ammo::Run(const std::vector<std::string>& Args)
{
	const size_t OffsetWeapon = 0x75FACD;

	// Nop elements (Patch)
	const uint8_t unlimitedAmmo[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	// Set
	const uint8_t ammoReset[8] = { 0x66, 0x29, 0x94, 0x31, 0x8E, 0x02, 0x00, 0x00 };

	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			// Disable it.
			std::cout << "Disabling unlimited ammo" << std::endl;
			enabled = false;
			memcpy(((uint8_t*)GetModuleBase()) + OffsetWeapon, ammoReset, sizeof(ammoReset));
		}
		else if( Args[1].compare("on") == 0 )
		{
			// Enable
			std::cout << "Enabling unlimited ammo" << std::endl;
			enabled = true;
			memcpy(((uint8_t*)GetModuleBase()) + OffsetWeapon, unlimitedAmmo, sizeof(unlimitedAmmo));
		}
	}
	return;
}