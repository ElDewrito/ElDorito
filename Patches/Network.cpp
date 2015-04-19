#include "Network.h"
#include "../Patch.h"
#include "../Utils/VersionInfo.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace
{
	char* Network_GetIPStringFromInAddr(void* inaddr);
	char Network_XnAddrToInAddrHook(void* pxna, void* pxnkid, void* in_addr);
	char Network_InAddrToXnAddrHook(void* ina, void * pxna, void * pxnkid);
}

namespace Patches
{
	namespace Network
	{
		void ApplyAll()
		{
			// Fix network debug strings having (null) instead of an IP address
			Hook(0x3F6F0, false, &Network_GetIPStringFromInAddr).Apply();

			// Fix for XnAddrToInAddr to try checking syslink-menu data for XnAddr->InAddr mapping before consulting XNet
			Hook(0x30B6C, true, &Network_XnAddrToInAddrHook).Apply();
			Hook(0x30F51, true, &Network_InAddrToXnAddrHook).Apply();

			// Patch version subs to return version of this DLL, to make people with older DLLs incompatible
			uint32_t verNum = Utils::Version::GetVersionInt();
			Pointer::Base(0x101421).Write<uint32_t>(verNum);
			Pointer::Base(0x10143A).Write<uint32_t>(verNum);
		}
	}
}

namespace
{
	char* Network_GetIPStringFromInAddr(void* inaddr)
	{
		static char ipAddrStr[64];
		memset(ipAddrStr, 0, 64);

		uint8_t ip3 = *(uint8_t*)inaddr;
		uint8_t ip2 = *((uint8_t*)inaddr + 1);
		uint8_t ip1 = *((uint8_t*)inaddr + 2);
		uint8_t ip0 = *((uint8_t*)inaddr + 3);

		uint16_t port = *(uint16_t*)((uint8_t*)inaddr + 0x10);
		uint16_t type = *(uint16_t*)((uint8_t*)inaddr + 0x12);

		sprintf_s(ipAddrStr, 64, "%hd.%hd.%hd.%hd:%hd (%hd)", ip0, ip1, ip2, ip3, port, type);

		return ipAddrStr;
	}
#pragma endregion

	char Network_XnAddrToInAddrHook(void* pxna, void* pxnkid, void* in_addr)
	{
		//printf("XnAddrToInAddrHook...");
		uint32_t maxMachines = *(uint32_t*)(0x228E6D8);
		uint8_t* syslinkDataPtr = (uint8_t*)*(uint32_t*)(0x228E6D8 + 0x4);

		for (uint32_t i = 0; i < maxMachines; i++)
		{
			uint8_t* entryPtr = syslinkDataPtr;
			syslinkDataPtr += 0x164F8;
			uint8_t entryStatus = *entryPtr;
			if (entryStatus == 0)
				continue;

			uint8_t* xnkidPtr = entryPtr + 0x9E;
			uint8_t* xnaddrPtr = entryPtr + 0xAE;
			uint8_t* ipPtr = entryPtr + 0x170;

			if (memcmp(pxna, xnaddrPtr, 0x10) == 0 && memcmp(pxnkid, xnkidPtr, 0x10) == 0)
			{
				// in_addr struct:
				// 0x0 - 0x10: IP (first 4 bytes for IPv4, 0x10 for IPv6)
				// 0x10 - 0x12: Port number
				// 0x12 - 0x14: IP length (4 for IPv4, 0x10 for IPv6)

				memset(in_addr, 0, 0x14);
				memcpy(in_addr, ipPtr, 4);

				*(uint16_t*)((uint8_t*)in_addr + 0x10) = 11774;
				*(uint16_t*)((uint8_t*)in_addr + 0x12) = 4;

				return 1;
			}
		}

		typedef char(*Network_XnAddrToInAddrFunc)(void* pxna, void* pxnkid, void* in_addr);
		Network_XnAddrToInAddrFunc XnAddrToInAddr = (Network_XnAddrToInAddrFunc)0x52D840;
		return XnAddrToInAddr(pxna, pxnkid, in_addr);
	}


	char Network_InAddrToXnAddrHook(void* ina, void * pxna, void * pxnkid)
	{
		uint32_t maxMachines = *(uint32_t*)(0x228E6D8);
		uint8_t* syslinkDataPtr = (uint8_t*)*(uint32_t*)(0x228E6D8 + 0x4);

		for (uint32_t i = 0; i < maxMachines; i++)
		{
			uint8_t* entryPtr = syslinkDataPtr;
			syslinkDataPtr += 0x164F8;
			uint8_t entryStatus = *entryPtr;
			if (entryStatus == 0)
				continue;

			uint8_t* xnkidPtr = entryPtr + 0x9E;
			uint8_t* xnaddrPtr = entryPtr + 0xAE;
			uint8_t* ipPtr = entryPtr + 0x170;

			if (memcmp(ipPtr, ina, 0x4) == 0)
			{
				memcpy(pxna, xnaddrPtr, 0x10);
				memcpy(pxnkid, xnkidPtr, 0x10);

				return 1;
			}
		}

		typedef char(*Network_InAddrToXnAddrFunc)(void* ina, void * pxna, void * pxnkid);
		Network_InAddrToXnAddrFunc InAddrToXnAddr = (Network_InAddrToXnAddrFunc)0x52D840;
		return InAddrToXnAddr(ina, pxna, pxnkid);
	}
}