#include "ModuleUPnP.hpp"

#include <miniupnpc/upnpcommands.h>
#include <string>

namespace Modules
{
	ModuleUPnP::ModuleUPnP() : ModuleBase("UPnP")
	{
		upnpDevice = upnpDiscover(2000, NULL, NULL, 0, 0, 2, &upnpDiscoverError);

		VarUPnPEnabled = AddVariableInt("Enabled", "upnp_enabled", "Enables UPnP to automatically port forward when hosting a game.", eCommandFlagsArchived, 1);
		VarUPnPEnabled->ValueIntMin = 0;
		VarUPnPEnabled->ValueIntMax = 1;
	}

	Utils::UPnPResult ModuleUPnP::UPnPForwardPort(bool tcp, int externalport, int internalport, const std::string & ruleName)
	{
		struct UPNPUrls urls;
		struct IGDdatas data;
		char lanaddr[16];

		if (upnpDiscoverError != UPNPDISCOVER_SUCCESS || upnpDevice == nullptr)
			return Utils::UPnPResult(Utils::UPnPErrorType::DiscoveryError, upnpDiscoverError);

		int ret = UPNP_GetValidIGD(upnpDevice, &urls, &data,
			lanaddr, sizeof(lanaddr));

		if (ret != Utils::UPNP_IGD_VALID_CONNECTED)
			return Utils::UPnPResult(Utils::UPnPErrorType::IGDError, ret);

		ret = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
			std::to_string(externalport).c_str(), std::to_string(internalport).c_str(),
			lanaddr, ruleName.c_str(), tcp ? "TCP" : "UDP", NULL, NULL);

		Utils::UPnPErrorType type = Utils::UPnPErrorType::None;
		if (ret != UPNPCOMMAND_SUCCESS)
			type = Utils::UPnPErrorType::PortMapError;

		return Utils::UPnPResult(type, ret);
	}
}
