#include "Ammo.h"

#include "../ElDorito.h"
#include "../HexConstants.h"

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
	std::string Info = "Unlimited ammo: ";
	Info += (enabled ? "Enabled" : "Disabled");
	Info += "\nUsage: " + Usage() + "\n"
		"Bottomless clip on weapon ammo and grenades\n"
		"Respawn to have all grenade types when enabled.\n";

	return Info;
}

std::string Ammo::Usage()
{
	return "ammo on|off";
}

void Ammo::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Ammo::Run(const std::vector<std::string>& Args)
{
	const size_t OffsetWeapon = 0x75FACD;
	const size_t OffsetGrenades = 0x7A3BFF;

	// Nop elements (Patch)
	const uint8_t unlimitedAmmo[] = { Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP };
	const uint8_t unlimitedGrenades[] = { Hex::NOP, Hex::NOP };

	// Set
	const uint8_t ammoReset[] = { 0x66, 0x29, 0x94, 0x31, 0x8E, 0x02, 0x00, 0x00 };
	const uint8_t grenadesReset[] = { 0xFE, 0xC8 };

	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			// Disable it.
			std::cout << "Disabling unlimited ammo" << std::endl;
			enabled = false;

			Pointer(Pointer::Base()(OffsetWeapon)).Write(ammoReset, sizeof(ammoReset));
			Pointer(Pointer::Base()(OffsetGrenades)).Write(grenadesReset, sizeof(grenadesReset));
		}
		else if( Args[1].compare("on") == 0 )
		{
			// Nade patch
			const uint8_t allGrenades[] = { 0xC6, 0x84, 0x37, 0x20, 0x03, 0x00, 0x00, 0x63 };
			//eldorado.exe + 1A3273 - C6 84 37 20 03 00 00 00 - mov byte ptr[edi + esi + 00000320], 00
			//SetPageAccess(Pointer::Base() + 0x1a3273, sizeof(allGrenades), PAGE_EXECUTE_READWRITE);
			Pointer(Pointer::Base()(0x1a3273)).Write(allGrenades, sizeof(allGrenades));

			//For some reason doesnt freezes up when using Write. Stick to memcpy for now. Look into later.
			//memcpy(Pointer::Base()(0x1a3273), allGrenades, sizeof(allGrenades));

			// Enable
			std::cout
				<< "Enabling unlimited ammo\n"
				"(Respawn/suicide to get all grenades)\n"
				<< std::endl;
			enabled = true;

			Pointer(Pointer::Base()(OffsetWeapon)).Write(unlimitedAmmo, sizeof(unlimitedAmmo));
			Pointer(Pointer::Base()(OffsetGrenades)).Write(unlimitedGrenades, sizeof(unlimitedGrenades));
		}

		return true;
	}

	return false;
}