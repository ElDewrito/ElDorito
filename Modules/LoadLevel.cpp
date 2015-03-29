#include "LoadLevel.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

LoadLevel::LoadLevel()
{
	// Level load patch
	const uint8_t NOP[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	memcpy((uint8_t*)GetBasePointer() + 0x2D26DF, NOP, sizeof(NOP));

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
	Info += "\nUsage: " + Usage();
	
	return Info;
}

std::string LoadLevel::Usage()
{
	std::string usage = "load (mapname)\n"
		"Available Maps";
	for(std::vector<std::string>::iterator map = MapList.begin(); map != MapList.end(); ++map)
	{
		usage += '\n';
		usage += 'Ä';
		usage += *map;
	}

	return usage;
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
		if( std::find(MapList.begin(), MapList.end(),Args[1]) != MapList.end())
		{
			std::cout << "Loading map: " + Args[1] << std::endl;
			std::string MapName = "maps\\" + Args[1];

			// Todo: Gametype

			// Game Type

			Pointer(0x2391800).Write<uint32_t>(0x2);

			//*((uint32_t*)(0x2391800)) = 0x2;

			// Map Name
			//memcpy((char*)0x2391824, MapName.c_str(), MapName.length() + 1);
			Pointer(0x2391824).Write(MapName.c_str(), MapName.length() + 1);

			// Map Reset
			Pointer(0x23917F0).Write<uint8_t>(0x1);
			//*((uint8_t*)(0x23917F0)) = 0x1;
		}
		else {
			std::cout << "Unknown map." << std::endl;
			std::cout << Info() << std::endl;
		}

	
	}
	return;
}