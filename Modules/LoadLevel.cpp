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
	std::string Info = 
		"Usage: load (mapname)"
		"Available Maps\n";
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
	static size_t Ticker = 0;
	static double Timer = 0;

	Timer += Delta.count();
	if( Timer >= 2.0)
	{
		Ticker++;
		Timer = 0;
		const std::string Msg[] = { 
			":^)", ";^)", ":^)", 
			"THIS IS A FAKE SCREENSHOT",
			"THIS SCREENSHOT IS FAKE",
			"DO NOT BELIEVE THIS IMAGE",
			"STOP RAIDING #MODACITY",
			"FAKE 0.4.432",
			"FAKE IMAGE BETA: V0.0.4",
			"THIS SCREENSHOT IS FAKE",
			"DO NOT BELIEVE THIS IMAGE",
			">Being a pretentious pissbaby", ">Hoarding info for twitter followers", ">Being a pizza shit", };
		std::string TickStr = Msg[Ticker % (sizeof(Msg) / sizeof(std::string)) ];
		DWORD temp;
		VirtualProtect((uint8_t*)GetBasePointer() + 0x120CCB8, TickStr.length() + 1, PAGE_EXECUTE_READWRITE, &temp);
		memcpy((uint8_t*)GetBasePointer() + 0x120CCB8, TickStr.c_str(), TickStr.size()+1);
	}
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