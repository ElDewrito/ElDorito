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

	std::string ArgList((char*)Pointer(0x199C0A4)[0]);
	std::cout << "Command line args: " << (ArgList.empty() ? "(null)" : ArgList) << std::endl;

	std::string LocalSecureKey((char*)Pointer(0x50CCDB4 + 1));
	std::cout << "Local Secure Key: " << (LocalSecureKey.empty() ? "(null)" : LocalSecureKey) << std::endl;

	std::string Build((char*)Pointer(0x199C0F0));
	std::cout << "Build: " << (Build.empty() ? "(null)" : Build) << std::endl;

	std::string SystemID((char*)Pointer(0x199C130));
	std::cout << "SystemID: " << (SystemID.empty() ? "(null)" : SystemID) << std::endl;

	std::string SessionID((char*)Pointer(0x199C1D0));
	std::cout << "SessionID: " << (SessionID.empty() ? "(null)" : SessionID) << std::endl;

	std::string DevKitName((char*)Pointer(0x160C8C8));
	std::string DevKitVersion((char*)Pointer(0x199C350));
	std::cout << "SDK Info: " << (DevKitName.empty() ? "(null)" : DevKitName) << '|';
	std::cout << (DevKitVersion.empty() ? "(null)" : DevKitVersion) << std::endl;

	std::string FlashVersion((char*)Pointer(0x199C350));
	std::cout << "Flash Version: " << (FlashVersion.empty() ? "(null)" : FlashVersion) << std::endl;

	return true;
}