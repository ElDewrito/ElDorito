#include "LoadLevel.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

LoadLevel::LoadLevel()
{
	// Level load patch
	//const uint8_t NOP[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	//memcpy((uint8_t*)GetBasePointer() + 0x2D26DF, NOP, sizeof(NOP));
	Patch::NopFill(Pointer::Base(0x2D26DF), 5);

	//populate map list on load
	WIN32_FIND_DATA Finder;
	HANDLE hFind = FindFirstFile((ElDorito::Instance().GetDirectory() + "\\maps\\*.map").c_str(), &Finder);
	if( hFind == INVALID_HANDLE_VALUE )
	{
		std::cout << "No map files found" << std::endl;
	}
	do
	{
		if( !(Finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			std::string MapName(Finder.cFileName);
			//remove extension
			MapList.push_back(MapName.substr(0, MapName.find_last_of('.')));
		}
	} while( FindNextFile(hFind, &Finder) != 0 );
}

LoadLevel::~LoadLevel()
{
}

std::string LoadLevel::Info()
{
	std::string Info = "Current map: ";
	Info += (char*)(0x2391824);
	Info += "\nUsage: load (mapname) [gametype]\n"
		"Available Maps";
	for( auto map : MapList )
	{
		Info += '\n';
		Info += 'Ä';
		Info += map;
	}
	Info += "\nValid gametypes are 1 to 10";

	return Info;
}

void LoadLevel::Tick(const std::chrono::duration<double>& Delta)
{
}

bool LoadLevel::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 2 )
	{
		if( std::find(MapList.begin(), MapList.end(), Args[1]) != MapList.end() )
		{
			std::cout << "Loading map: " + Args[1] << std::endl;
			std::string MapName = "maps\\" + Args[1];

			// Game Type
			int gameType = 2; // slayer

			if (Args.size() >= 3)
			{
				gameType = std::atoi(Args[2].c_str());
				if (gameType <= 0 || gameType > 10) // only valid gametypes are 1 to 10
					gameType = 2;
			}
			std::cout << "Gametype: " << gameType << std::endl;

			Pointer(0x2391B2C).Write<uint32_t>(gameType);

			// Game Mode

			Pointer(0x2391800).Write<uint32_t>(0x2);

			//*((uint32_t*)(0x2391800)) = 0x2;

			// Map Name
			//memcpy((char*)0x2391824, MapName.c_str(), MapName.length() + 1);
			Pointer(0x2391824).Write(MapName.c_str(), MapName.length() + 1);

			// Map Reset
			Pointer(0x23917F0).Write<uint8_t>(0x1);
			//*((uint8_t*)(0x23917F0)) = 0x1;

			return true;
		}
	}

	return false;
}