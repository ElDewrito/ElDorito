#include "Godmode.h"

#include "../ElDorito.h"
#include "../HexConstants.h"

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

	std::string Info = "usage: god off|on\n";

	return Info;
}

void Godmode::Tick(const std::chrono::duration<double>& Delta)
{
}

void Godmode::Run(const std::vector<std::string>& Args)
{
	const size_t OffsetHealth = 0x7555DC;
	const uint8_t god[] = { Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP, Hex::NOP };
	const uint8_t resetHealth[] = { 0xF3, 0x0F, 0x11, 0x97, 0x00, 0x01, 0x00, 0x00 };

	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			std::cout << "Disabling god" << std::endl;
			enabled = false;
			Pointer::Base()(OffsetHealth).Write(resetHealth, sizeof(resetHealth));
		}
		else if( Args[1].compare("on") == 0 )
		{
			std::cout << "Enabling god" << std::endl;
			enabled = true;
			Pointer::Base()(OffsetHealth).Write(god, sizeof(god));
		}
	}
	return;
}