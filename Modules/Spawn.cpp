#include "Spawn.h"

#include "../ElDorito.h"
#include <iostream>

Spawn::Spawn()
{
}

Spawn::~Spawn()
{
}

std::string Spawn::Info()
{
	return "";
}

void Spawn::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Spawn::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 3 )
	{
		if( Args[1].compare("gun") == 0 )
		{
			uint32_t meta = std::stoi(Args[2], 0, 16);
			//std::cout << std::hex << meta << std::endl;
			if( meta == 0 )
			{
				std::cout << "Error parsing hex" << std::endl;
				return false;
			}

			char proj[] = { 0xBA, 0, 0, 0, 0, 0x90 };

			(*(int*)(proj + 1)) = meta;
			Pointer(0xB5DBA5).Write(proj, sizeof(proj));

			std::cout << "Projectile meta set to: " << std::hex <<
				Pointer(0xB5DBA6).Read<uint32_t>() << std::endl;

			return true;
		}
	}
	return false;
}