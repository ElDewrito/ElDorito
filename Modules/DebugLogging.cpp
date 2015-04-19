#include "DebugLogging.h"

#include "../ElDorito.h"
#include "../Patches/Logging.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

DebugLogging::DebugLogging() : enabledFlags(0)
{
}

DebugLogging::~DebugLogging()
{
}

std::string DebugLogging::Info(const std::string& Topic) const
{
	std::string Info = "Debug logging: ";
	if( enabledFlags == 0 )
		Info += "Disabled";
	else
	{
		Info += "Enabled - ";
		Info += (enabledFlags & 2) ? "Network " : "";
		Info += (enabledFlags & 4) ? "SSL " : "";
		Info += (enabledFlags & 8) ? "UI " : "";
		Info += (enabledFlags & 16) ? "Game1 " : "";
		Info += (enabledFlags & 32) ? "Game2 " : "";
	}
	Info += "\nUsage: debug (network|ssl|ui|game1|game2|all|off)\n"
		"Enables hooks for debug output\n"
		"Writes debug messages to dorito.log when enabled.";

	return Info;
}

std::string DebugLogging::Suggest(const std::vector<std::string>& Arguments) const
{
	if( Arguments.size() == 2 )
	{
		if( Arguments[1].empty() )
		{
			return "network";
		}
	}
	return "";
}

void DebugLogging::Tick(const std::chrono::duration<double>& Delta)
{
}

bool DebugLogging::Run(const std::vector<std::string>& Args)
{
	bool hookNetwork = false;
	bool hookSSL = false;
	bool hookUI = false;
	bool hookGame1 = false;
	bool hookGame2 = false;

	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			// Disable it.
			std::cout << "Disabling hooks" << std::endl;
			enabledFlags = 0;

			Patches::Logging::EnableNetworkLog(false);
			Patches::Logging::EnableSslLog(false);
			Patches::Logging::EnableUiLog(false);
			Patches::Logging::EnableGame1Log(false);
			Patches::Logging::EnableGame2Log(false);

			return true;
		}
		else
		{
			hookNetwork = Args[1].compare("network") == 0;
			hookSSL = Args[1].compare("ssl") == 0;
			hookUI = Args[1].compare("ui") == 0;
			hookGame1 = Args[1].compare("game1") == 0;
			hookGame2 = Args[1].compare("game2") == 0;
			if( Args[1].compare("all") == 0 || Args[1].compare("on") == 0 )
				hookNetwork = hookSSL = hookUI = hookGame1 = hookGame2 = true;

			if( hookNetwork )
			{
				std::cout << "Hooking network debug output..." << std::endl;
				enabledFlags |= 2;

				Patches::Logging::EnableNetworkLog(true);
			}

			if( hookSSL )
			{
				std::cout << "Hooking SSL debug output..." << std::endl;
				enabledFlags |= 4;

				Patches::Logging::EnableSslLog(true);
			}

			if( hookUI )
			{
				std::cout << "Hooking UI debug output..." << std::endl;
				enabledFlags |= 8;

				Patches::Logging::EnableUiLog(true);
			}

			if( hookGame1 )
			{
				std::cout << "Hooking Game1 debug output..." << std::endl;
				enabledFlags |= 16;

				Patches::Logging::EnableGame1Log(true);
			}

			if( hookGame2 )
			{
				std::cout << "Hooking Game2 debug output..." << std::endl;
				enabledFlags |= 32;

				Patches::Logging::EnableGame2Log(true);
			}

			if( hookNetwork || hookSSL || hookUI || hookGame1 || hookGame2 )
				return true;
		}
	}

	return false;
}