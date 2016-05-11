#pragma once

#include "ModuleBase.hpp"
#include "../Utils/UPnP.hpp"

#include <miniupnpc/miniupnpc.h>

namespace Modules
{
	class ModuleUPnP : public Utils::Singleton<ModuleUPnP>, public ModuleBase
	{
	public:
		Command* VarUPnPEnabled;

		ModuleUPnP();
		Utils::UPnPResult UPnPForwardPort(bool tcp, int externalport, int internalport, const std::string & ruleName);
	private:
		int upnpDiscoverError;
		UPNPDev* upnpDevice = nullptr;
	};
}