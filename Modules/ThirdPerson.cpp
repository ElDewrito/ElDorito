#include "ThirdPerson.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

ThirdPerson::ThirdPerson()
	:
	enabled(false),
	ThirdPatchX(0xA644AC,
	{ 0x90, 0x90, 0x90, 0x90, 0x90},
	{ 0x66, 0x0F, 0xD6, 0x47, 0x5C }),
	
	ThirdPatchY(0x63B952,
	{0x90},
	{0x50}),

	ThirdPatchZ(0x6644B4, 
	{ 0x90, 0x90, 0x90 },
	{0x89,0x47,0x64}),

	ThirdPatchXX(0x1933F5,
	{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
	{ 0x39, 0x94, 0xB1, 0x88, 0x05, 0x00, 0x00 }),

	ThirdPatchYY(0x193401,
	{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
	{ 0x89, 0x9C, 0xB1, 0x84, 0x05, 0x00, 0x00 }),

	ThirdPatchZZ(0x7EF288,
	{ 0x90, 0x90},
	{ 0x73, 0x07 })
{
}

ThirdPerson::~ThirdPerson()
{
}

std::string ThirdPerson::Info()
{
	std::string Info = "Third person: ";
	Info += (enabled ? "Enabled" : "Disabled");
	Info += "\nUsage: third (on|off)\n"
		"Bottomless clip on weapon ammo and grenades\n"
		"Respawn to have all grenade types when enabled.\n";

	return Info;
}

void ThirdPerson::Tick(const std::chrono::duration<double>& Delta)
{
}

bool ThirdPerson::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		if (Args[1].compare("off") == 0)
		{
			// Disable it.
			std::cout << "Disabling third person" << std::endl;
			enabled = false;

			ThirdPatchX.Reset();
			ThirdPatchY.Reset();
			ThirdPatchZ.Reset();
			ThirdPatchXX.Reset();
			ThirdPatchYY.Reset();
			ThirdPatchZZ.Reset();

			return true;
		}
		else if (Args[1].compare("on") == 0)
		{
			
			// Enable
			std::cout
				<< "Enabling third person\n"
				"(Respawn/suicide to get all grenades)\n"
				<< std::endl;
			enabled = true;

			ThirdPatchX.Apply();
			ThirdPatchY.Apply();
			ThirdPatchZ.Apply();
			ThirdPatchXX.Apply();
			ThirdPatchYY.Apply();
			ThirdPatchZZ.Apply();

			return true;
		}
	}

	return false;
}