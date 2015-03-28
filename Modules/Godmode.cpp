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
	if (Args.size() >= 2)
	{

		if (Args[1] == "disable") {
			// Disable it.
			std::cout << "Disabling god map" << std::endl;
			enabled = false;
			memcpy((uint8_t*)GetModuleBase() + 0x075636A, reset, 8);

		}
		else if (Args[1] == "enable") {
			// Enable
			std::cout << "Enabling god map" << std::endl;
			enabled = true;
			memcpy((uint8_t*)GetModuleBase() + 0x075636A, god, 8);
		}

	}
	return;
}