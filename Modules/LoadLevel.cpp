#include "LoadLevel.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

LoadLevel::LoadLevel()
{
	// Level load patch
	const uint8_t NOP[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	memcpy((uint8_t*)GetBasePointer() + 0x2D26DF, NOP, sizeof(NOP));
}

LoadLevel::~LoadLevel()
{
}

std::string LoadLevel::Info()
{
	std::cout << "Current map: " << (char*)(0x2391824) << std::endl;
	std::string Info = "Available Maps\n";
	WIN32_FIND_DATA Finder;
	HANDLE hFind = FindFirstFile((ElDorito::Instance().GetDirectory() + "\\maps\\*.map").c_str(), &Finder);
	if( hFind == INVALID_HANDLE_VALUE )
	{
		std::cout << "No map files found" << std::endl;
		return Info;
	}
	do
	{
		if( !(Finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			std::string MapName(Finder.cFileName);
			//remove extension
			MapName = MapName.substr(0, MapName.find_last_of('.'));
			Info += 'Ä';
			Info += MapName + '\n';
		}
	} while( FindNextFile(hFind, &Finder) != 0 );
	return Info;
}

void LoadLevel::Tick(const std::chrono::duration<double>& Delta)
{
}

void LoadLevel::Run(const std::vector<std::string>& Args)
{
	if( Args.size() <= 1 )
	{
		std::cout << Info();
	}
	if( Args.size() >= 2 )
	{
		std::cout << "Loading map: " + Args[1] << std::endl;
		std::string MapName = "maps\\" + Args[1];

		// Todo:	Check if map exists
		//			GameType

		// Game Type
		*((uint32_t*)(0x2391800)) = 0x2;

		// Map Name
		memcpy((char*)0x2391824, MapName.c_str(), MapName.length() + 1);

		// Map Reset
		*((uint8_t*)(0x23917F0)) = 0x1;
	}
	return;
}