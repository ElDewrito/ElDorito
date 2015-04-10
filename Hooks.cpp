#include "Hooks.h"

#include <iostream>
#include <fstream>

TODO("Move dbglog into a header that the whole project can use")
void dbglog(const char* module, char* format, ...)
{
	char* backupFormat = "";
	if (!format)
		format = backupFormat;

	if (module != 0 && strcmp(module, "game_tick") == 0)
		return; // filter game_tick spam

	va_list ap;
	va_start(ap, format);

	char buff[4096];
	vsprintf_s(buff, 4096, format, ap);
	va_end(ap);

	std::ofstream outfile;
	outfile.open("dorito.log", std::ios_base::app);
	outfile << '[' << module << "] " << buff << '\n';
	outfile.close();
}


#pragma region Debug Log Hooks
void debuglog_string(char* name, char* value)
{
	dbglog("Debug", "%s: %s", name, value);
}

void debuglog_int(char* name, int value)
{
	dbglog("Debug", "%s: %d", name, value);
}

void debuglog_float(char* name, float value)
{
	dbglog("Debug", "%s: %f", name, value);
}

int networkLogHook(char* format, ...)
{
	if ((uint32_t)format == 0x1628578) // fix for crash
		return 1;

	char dstBuf[4096];
	memset(dstBuf, 0, 4096);

	va_list args;
	va_start(args, format);
	vsnprintf_s(dstBuf, 4096, 4096, format, args);
	//vsnprintf(dstBuf, 4095, format, args);
	va_end(args);

	dbglog("Network", "%s", dstBuf);

	return 1;
}

void __cdecl sslLogHook(char a1, int a2, void* a3, void* a4, char a5)
{
	char* logData1 = (*(char**)(a3));
	char* logData2 = (*(char**)((DWORD_PTR)a3 + 0x8));
	if (logData1 != 0)
		logData1 += 0xC;
	if (logData2 != 0)
		logData2 += 0xC;
	if (logData1 == 0)
		logData1 = "";
	if (logData2 == 0)
		logData2 = "";
	dbglog((const char*)logData1, (char*)logData2);
	return;
}

void __cdecl uiLogHook(char a1, int a2, void* a3, void* a4, char a5)
{
	char* logData1 = (*(char**)(a3));
	if (logData1 != 0)
		logData1 += 0xC;
	if (logData1 == 0)
		logData1 = "";
	dbglog("UiLog", (char*)logData1);
	return;
}

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