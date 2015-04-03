#include "Test.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <TlHelp32.h> //GetModuleBase

#include <iostream>

Test::Test()
{
	//std::cout << "Test: " << std::hex << static_cast<size_t>(*(char*)(0x006E5207)) << std::endl;
	//std::cout << (char*)Pointer(0x006E51E7)[0] << std::endl;
	//SetPageAccess(0x006E5207, 1, PAGE_EXECUTE_READWRITE);
	//Pointer(0x006E5207).Write<uint8_t>(128);
	//const char* Honk = "YOU'VE DIDDLED ME FOR THE LAST TIME";
	//Pointer(0x006E51E7).Write<uint32_t>((uint32_t)Honk);
	//std::cout << (char*)Pointer(0x006E51E7)[0] << std::endl;
}

Test::~Test()
{}

std::string Test::Info() const
{
	return "Sandbox module for testing stuff.\n"
		"If you find this in the release build you probably shouldn't run it.\n";
}

std::string Test::Suggest(const std::vector<std::string>& Arguments) const
{
	return "aa";
}

void Test::Tick(const std::chrono::duration<double>& Delta)
{
	static size_t Ticker = 0;
	static double Timer = 0;
	Timer += Delta.count();

	//std::cout << TlsAddress << std::endl;
	//std::cout << Pointer(TlsAddress + 0x32C).Read<uint32_t>() << std::endl;
	if( ElDorito::GetMainTls(0x32C).Read<uint32_t>() )
	{
		ElDorito::GetMainTls(0x32C)[0].Write<float>(sinf(Timer*3.14f * 2)*4.17126f);
		//printf("%f\n", ElDorito::GetMainTls(0x32C)[0].Read<float>());
	}
}

bool Test::Run(const std::vector<std::string>& Args)
{
	// Gravity
	std::cout << ElDorito::GetMainTls(0x32C)[0].Read<float>() << std::endl;

	return true;
}