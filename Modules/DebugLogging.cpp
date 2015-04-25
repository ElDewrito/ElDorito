#include "DebugLogging.h"

#include "../ElDorito.h"
#include "../Patches/Logging.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

DebugLogging::DebugLogging() : EnabledFlags(0)
{
}

DebugLogging::~DebugLogging()
{
}

std::string DebugLogging::Info(const std::string& Topic) const
{
	std::string Info = "Debug logging: ";
	if (EnabledFlags == 0)
		Info += "Disabled";
	else
	{
		Info += "Enabled - ";
		Info += (EnabledFlags & DebugLoggingModes::Network) ? "Network " : "";
		Info += (EnabledFlags & DebugLoggingModes::SSL) ? "SSL " : "";
		Info += (EnabledFlags & DebugLoggingModes::UI) ? "UI " : "";
		Info += (EnabledFlags & DebugLoggingModes::Game1) ? "Game1 " : "";
		Info += (EnabledFlags & DebugLoggingModes::Game2) ? "Game2 " : "";

		if (FiltersInclude.size() > 0)
		{
			Info += "\n\nInclude filters:\n";
			for (auto filter : FiltersInclude)
			{
				Info += "  " + filter + "\n";
			}
		}

		if (FiltersExclude.size() > 0)
		{
			Info += "\n\nExclude filters:\n";
			for (auto filter : FiltersExclude)
			{
				Info += "  " + filter + "\n";
			}
		}
	}
	Info += "\nUsage: debug [mode/include/exclude] (options)\n\n"
		"debug mode (network|ssl|ui|game1|game2|all|off):\n"
		"  Enables hooks for debug output\n"
		"  Writes debug messages to dorito.log when enabled.\n\n"
		"debug [include/exclude] (filters):\n"
		"  include and exclude options allow you to specify words the debug messages\n"
		"  must/must not contain\n\n"
		"  eg. \"debug exclude ONLINE/CLIENT\" will exclude strings that contain\n"
		"  \"ONLINE/CLIENT\" from output\n\n"
		"  \"debug include reset_peer_state\" will only output debug strings that contain\n"
		"  \"reset_peer_state\"\n\n"
		"  Multiple filters can be added, they must all be specified in the initial\n"
		"  call to debug\n\n"
		"  eg. \"debug exclude ONLINE/CLIENT peer\" will exclude strings that contain\n"
		"  \"ONLINE/CLIENT\" and \"peer\"\n"
		"  calling \"debug exclude peer\" afterward will reset the exclude filters and\n"
		"  only exclude strings containing \"peer\"";

	return Info;
}

std::string DebugLogging::Suggest(const std::vector<std::string>& Arguments) const
{
	if( Arguments.size() == 2 )
		if( Arguments[1].empty() )
			return "mode";

	if (Arguments.size() == 3)
		if (Arguments[1] == "mode")
			return "off";

	return "";
}

void DebugLogging::Tick(const std::chrono::duration<double>& Delta)
{
}

bool DebugLogging::SetMode(std::string mode)
{
	bool hookNetwork = false;
	bool hookSSL = false;
	bool hookUI = false;
	bool hookGame1 = false;
	bool hookGame2 = false;

	if (mode.compare("off") == 0)
	{
		// Disable it.
		std::cout << "Disabling hooks" << std::endl;
		EnabledFlags = 0;

		Patches::Logging::EnableNetworkLog(false);
		Patches::Logging::EnableSslLog(false);
		Patches::Logging::EnableUiLog(false);
		Patches::Logging::EnableGame1Log(false);
		Patches::Logging::EnableGame2Log(false);

		return true;
	}
	else
	{
		hookNetwork = mode.compare("network") == 0;
		hookSSL = mode.compare("ssl") == 0;
		hookUI = mode.compare("ui") == 0;
		hookGame1 = mode.compare("game1") == 0;
		hookGame2 = mode.compare("game2") == 0;
		if (mode.compare("all") == 0 || mode.compare("on") == 0)
			hookNetwork = hookSSL = hookUI = hookGame1 = hookGame2 = true;

		if (hookNetwork)
		{
			std::cout << "Hooking network debug output..." << std::endl;
			EnabledFlags |= DebugLoggingModes::Network;

			Patches::Logging::EnableNetworkLog(true);
		}

		if (hookSSL)
		{
			std::cout << "Hooking SSL debug output..." << std::endl;
			EnabledFlags |= DebugLoggingModes::SSL;

			Patches::Logging::EnableSslLog(true);
		}

		if (hookUI)
		{
			std::cout << "Hooking UI debug output..." << std::endl;
			EnabledFlags |= DebugLoggingModes::UI;

			Patches::Logging::EnableUiLog(true);
		}

		if (hookGame1)
		{
			std::cout << "Hooking Game1 debug output..." << std::endl;
			EnabledFlags |= DebugLoggingModes::Game1;

			Patches::Logging::EnableGame1Log(true);
		}

		if (hookGame2)
		{
			std::cout << "Hooking Game2 debug output..." << std::endl;
			EnabledFlags |= DebugLoggingModes::Game2;

			Patches::Logging::EnableGame2Log(true);
		}

		if (hookNetwork || hookSSL || hookUI || hookGame1 || hookGame2)
			return true;
	}

	return false;
}

bool DebugLogging::Run(const std::vector<std::string>& Args)
{
	if (Args.size() >= 2)
	{
		if (Args[1] == "include")
		{
			FiltersInclude.clear();
			if (Args.size() >= 3)
			{
				for (auto i = Args.begin() + 2; i != Args.end(); i++)
				{
					FiltersInclude.push_back(*i);
					std::cout << "Added " << *i << " to include filters." << std::endl;
				}
				return true;
			}
			std::cout << "Cleared include filters." << std::endl;
			return true;
		}

		if (Args[1] == "exclude")
		{
			FiltersExclude.clear();
			if (Args.size() >= 3)
			{
				for (auto i = Args.begin() + 2; i != Args.end(); i++)
				{
					FiltersExclude.push_back(*i);
					std::cout << "Added " << *i << " to exclude filters." << std::endl;
				}
				return true;
			}
			std::cout << "Cleared exclude filters." << std::endl;
			return true;
		}

		return SetMode((Args[1] == "mode" && Args.size() >= 3) ? Args[2] : Args[1]);
	}

	return false;
}