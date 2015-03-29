#include "Test.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Test::Test()
{
}

Test::~Test()
{
}

std::string Test::Info()
{
	return "Sandbox module for testing stuff.\n"
		"If you find this in the release build you probably shouldn't run it.\n";
}

void Test::Tick(const std::chrono::duration<double>& Delta)
{
	static size_t Ticker = 0;
	static double Timer = 0;

	Timer += Delta.count();
	if( Timer >= 2.0 )
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
		std::string TickStr = Msg[Ticker % (sizeof(Msg) / sizeof(std::string))];
		DWORD temp;
		VirtualProtect((uint8_t*)GetBasePointer() + 0x120CCB8, TickStr.length() + 1, PAGE_EXECUTE_READWRITE, &temp);
		memcpy((uint8_t*)GetBasePointer() + 0x120CCB8, TickStr.c_str(), TickStr.size() + 1);
	}
}

void Test::Run(const std::vector<std::string>& Args)
{
	if( Args.size() <= 1 )
	{
		std::cout << Info();
	}
	if( Args.size() >= 2 )
	{
	}
	return;
}