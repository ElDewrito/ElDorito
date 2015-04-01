#include "LoadLevel.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"
#include "../Utils/Utils.h"

#include <Windows.h>
#include <iostream>
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
	for( size_t i = 0; i < MapList.size(); i++ )
	{
		Info += '\n';
		Info += (i == 0 ? '\xda' : i == MapList.size() - 1 ? '\xC0' : '\xc3');
		Info += MapList[i];
	}

	Info += "\nValid gametypes: \n";
	for( size_t i = 0; i < Blam::GameTypeCount - 1; i++ )
	{
		Info += (i == 0 ? '\xda' : i == Blam::GameTypeCount - 2 ? '\xC0' : '\xc3');
		Info += '[' + std::to_string(i) + ']';
		Info += Blam::GameTypeNames[i];
		Info += '\n';
	}

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
			Blam::GameType gameType = Blam::GameType::None; // No gametype

			if( Args.size() >= 3 )
			{
				//Look up gametype string.
				size_t i;
				for( i = 0; i < Blam::GameTypeCount; i++ )
				{
					// Todo: case insensiive
					if( !Blam::GameTypeNames[i].compare(Args[2]) )
					{
						gameType = Blam::GameType(i);
						break;
					}
				}

				if( i == Blam::GameTypeCount )
				{
					gameType = (Blam::GameType)std::atoi(Args[2].c_str());
				}
				if( gameType > Blam::GameTypeCount ) // only valid gametypes are 1 to 10
					gameType = Blam::GameType::Slayer;
			}
			std::cout << "Gametype: " << Blam::GameTypeNames[gameType] << std::endl;

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
