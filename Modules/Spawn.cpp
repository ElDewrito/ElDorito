#include "Spawn.h"

#include "../ElDorito.h"
#include <iostream>

Spawn::Spawn()
{
}

Spawn::~Spawn()
{
}

std::string Spawn::Info() const
{
	std::string Info = "Usage: spawn proj (hex meta)\n";

	//char HexBuf[9]{};
	//_itoa_s(Pointer(0xB5DBA6).Read<uint32_t>(), HexBuf, 16);
	//Info += "Current Projectile meta: ";
	//Info += HexBuf;

	Info += "Causes certain activites to spawn entities using the designated hex value\n";
	return Info;
}

std::string Spawn::Suggest(const std::vector<std::string>& Arguments) const
{
	if( Arguments.size() == 2 )
	{
		if( Arguments[1].empty() )
		{
			return "proj";
		}
	}
	return "";
}

void Spawn::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Spawn::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 3 )
	{
		if( Args[1].compare("proj") == 0 )
		{
			uint32_t meta = std::stoi(Args[2], 0, 16);
			//std::cout << std::hex << meta << std::endl;
			if( meta == 0 )
			{
				std::cout << "Error parsing hex" << std::endl;
				return false;
			}

			char proj[] = { 0xBA, 0, 0, 0, 0, 0x90 };

			( *(int*)( proj + 1 ) ) = meta;
			Pointer(0xB5DBA5).Write(proj, sizeof(proj));

			std::cout << "Projectile meta set to: " << std::hex <<
				Pointer(0xB5DBA6).Read<uint32_t>() << std::endl;

			return true;
		}
	}
	return false;
}