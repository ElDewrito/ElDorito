#include "Information.h"

#include "../ElDorito.h"

#include <iostream>

Information::Information()
{
}

Information::~Information()
{
}

std::string Information::Info(const std::string& Topic) const
{
	return "Provides various information about the the current Halo Online session";
}

std::string Information::Suggest(const std::vector<std::string>& Arguments) const
{
	return "";
}

void Information::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Information::Run(const std::vector<std::string>& Args)
{
	std::cout << std::hex << "ThreadLocalStorage: " << std::hex << (size_t)(void*)ElDorito::GetMainTls() << std::endl;

	std::string LocalSecureKey((char*)Pointer(0x50CCDB4 + 1));
	std::cout << "Local Secure Key: " << (LocalSecureKey.empty() ? "(null)" : LocalSecureKey) << std::endl;
	return true;
}