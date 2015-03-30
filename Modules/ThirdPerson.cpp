#include "ThirdPerson.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

ThirdPerson::ThirdPerson()
	:
	enabled(false),
	ThirdPatchX(0x6644AC,
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
	{ 0x73, 0x07 }),

	ThirdPatchXXX(0x5717EE,
	{ 0x90, 0x90, 0x90, 0x90, 0x90 },
	{ 0x66, 0x0F, 0xD6, 0x42, 0xFC }),

	ThirdPatchZZZ(0x193401,
	{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
	{ 0x89, 0x9C, 0xB1, 0x84, 0x05, 0x00, 0x00 })

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
		"Freezes the camera in place and player can move freely around\n";

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
			ThirdPatchXXX.Reset();
			ThirdPatchZZZ.Reset();

			return true;
		}
		else if (Args[1].compare("on") == 0)
		{
			
			// Enable
			std::cout
				<< "Enabling third person\n"
				"(Remember to move the player)\n"
				<< std::endl;
			enabled = true;

			ThirdPatchX.Apply();
			ThirdPatchY.Apply();
			ThirdPatchZ.Apply();
			ThirdPatchXX.Apply();
			ThirdPatchYY.Apply();
			ThirdPatchZZ.Apply();
			ThirdPatchXXX.Apply();
			ThirdPatchZZZ.Apply();
			return true;
		}
	}

	return false;
}