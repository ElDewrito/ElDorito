#include "Hooks.h"
#include "BlamTypes.h"

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
	TODO("find out why these network debug messages cause crashes")
	if ((uint32_t)format == 0x1628578) // fix for crash
		return 1;

	if ((uint32_t)format == 0x1627DD8) // another crash
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

struct SslWStringHeader
{
	int refCount;
	int length1;
	int length2;
};

struct PlayerNameString
{
	SslWStringHeader header;
	char16_t name[16];
};

// Player name string cache
PlayerNameString* playerNames[16];

static int GetUtf16Length(const char16_t *str, int maxLength)
{
	int length = 0;
	while (length < maxLength && str[length])
		length++;
	return length;
}

static PlayerNameString* GetPlayerName(void *playerData, int index)
{
	if (index == -1)
		return nullptr;

	// Get function pointers
	typedef void* (*AllocateFunc)(size_t size);
	AllocateFunc Allocate = (AllocateFunc)0xD874A0;

	// Get the player's display name
	Pointer playerName = Pointer(playerData)(GameGlobals::Players::DisplayNameOffset);

	// Allocate a string for the name if one hasn't been already
	PlayerNameString* result = playerNames[index];
	if (!result)
	{
		result = (PlayerNameString*)Allocate(sizeof(PlayerNameString));
		memset(result, 0, sizeof(result));
		result->header.refCount = 1;
		playerNames[index] = result;
	}

	// Copy the name in
	memcpy(result->name, playerName, 32);

	// Set the string length and return
	int length = GetUtf16Length(result->name, 16);
	result->header.length1 = length;
	result->header.length2 = length;
	return playerNames[index];
}

__declspec(naked) void GLScoreboardPlayerAllocatorHook()
{
	__asm
	{
		// Change the allocation size from 0x7C to 0x88 to make room for the nickname property reference
		// We can't just poke to the push instruction's immediate operand because otherwise 0x88 gets sign extended <_<
		push 0x88
		push 0x701A4E     // Return address
		mov edx, 0xD87130
		jmp edx
	}
}

__declspec(naked) void GLScoreboardPlayerConstructorHook()
{
	__asm
	{
		// Register the nickname property
		push esi
		lea ecx, [esi + 0x7C]        // Nickname property offset
		mov byte ptr[ebp - 0x4], 0xB // Nickname property index
		mov edx, 0x6B1590            // Register a UTF-16 "nickname" property (stdcall)
		call edx

		// Execute replaced code
		mov ecx, [ebp - 0xC]
		mov eax, esi
		mov edx, 0x6F8EDC
		jmp edx
	}
}

__declspec(naked) void ScoreboardUpdateHook()
{
	__asm
	{
		// Get player name string
		movzx eax, word ptr [ebp - 0x3C] // Player index
		push eax
		push esi                         // Player data
		call GetPlayerName
		add esp, 8
		test eax, eax
		jz done

		// Push the name string and increment the reference count
		mov [ebp - 0x14], eax // Temp variable we can use
		lea eax, [ebp - 0x14]
		push eax              // Push double-pointer to name string as an argument to sub_6AC840
		mov eax, [eax]        // Get the PlayerNameString object
		inc [eax]             // Increment reference count

		// Get a reference to the nickname property
		lea ecx, [edi + 0x7C] // Nickname property
		mov eax, 0x6C4B30     // Get nickname property reference
		call eax

		// Set it to the player's name
		mov ecx, eax      // this = nickname property
		mov edx, 0x6AC840 // Set wstring (stdcall)
		call edx

		// Decrement the name's reference count
		mov eax, [ebp - 0x14]
		dec [eax]

	done:
		// Execute replaced code
		lea eax, [ebp - 0x40]
		push eax
		push 0x71016C         // Return address
		mov edx, 0x55AE30
		jmp edx
	}
}

__declspec(naked) void SetLocalPlayerHook()
{
	__asm
	{
		// If the player index isn't 0, ignore it
		mov eax, [ebp + 0xC] // Argument 0 = player index
		test eax, eax
		jnz done

		// Get $UserDataStorage
		mov ecx, 0x50CE29C
		mov ecx, [ecx]

		// Get the local_user property
		add ecx, 0x10
		mov edx, 0x7EC2B0
		call edx
		lea ecx, [eax + 0x10]

		// Push a pointer to the player's datum index
		// Fun hack: since player index is 0, then the two arguments together make up a 64-bit player datum index
		lea eax, [ebp + 0x8]
		push eax

		// Get a reference to the uid property
		mov edx, 0x800780
		call edx

		// Set it to the player's datum index
		mov ecx, eax
		mov edx, 0x641C60 // Set int64 (stdcall)
		call edx

	done:
		// Execute replaced code
		mov ecx, 0x528A2B8
		mov ecx, [ecx]
		mov edx, 0x58A1DC
		jmp edx
	}
}

std::chrono::high_resolution_clock::time_point PrevTime = std::chrono::high_resolution_clock::now();
char __fastcall UI_Forge_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct)
{
	uint32_t btnCode = *(uint32_t*)(controllerStruct + 0x1C);

	auto CurTime = std::chrono::high_resolution_clock::now();
	auto timeSinceLastAction = std::chrono::duration_cast<std::chrono::milliseconds>(CurTime - PrevTime);

	if (btnCode == 0x12 || btnCode == 0x13)
	{
		if (timeSinceLastAction.count() < 200) // 200ms between button presses otherwise it spams the key
			return 1;

		PrevTime = CurTime;

		if (btnCode == 0x12) // analog left / arrow key left
			*(uint32_t*)(controllerStruct + 0x1C) = 0x5;

		if (btnCode == 0x13) // analog right / arrow key right
			*(uint32_t*)(controllerStruct + 0x1C) = 0x4;
	}

	typedef char(__thiscall *UI_Forge_ButtonPressHandler)(void* a1, void* controllerStruct);
	UI_Forge_ButtonPressHandler buttonHandler = (UI_Forge_ButtonPressHandler)0xAE2180;
	return buttonHandler(a1, controllerStruct);
}

__declspec(naked) void AutoAimHook()
{
	__asm
	{
		// Check if the player is using a mouse
		mov edx, 0x244DE98
		mov edx, [edx]
		test edx, edx
		jnz controller

		// Set magnetism angle to 0
		xor edx, edx
		mov [edi + 0x14], edx

		// Skip past magnetism angle code
		mov edx, 0x58AA23
		jmp edx

	controller:
		// Load magnetism angle normally
		movss xmm0, dword ptr [ebx + 0x388]
		mov edx, 0x58AA1F
		jmp edx
	}
}

static bool LocalizedStringHookImpl(int tagIndex, int stringId, wchar_t *outputBuffer)
{
	const size_t MaxStringLength = 0x400;

	switch (stringId)
	{
	case 0x1010A: // start_new_campaign
		{
			// Get the version string, convert it to uppercase UTF-16, and return it
			std::string version = Utils::Version::GetVersionString();
			std::transform(version.begin(), version.end(), version.begin(), toupper);
			std::wstring unicodeVersion(version.begin(), version.end());
			swprintf(outputBuffer, MaxStringLength, L"ELDEWRITO %s", unicodeVersion.c_str());
			return true;
		}
	}
	return false;
}

__declspec(naked) void LocalizedStringHook()
{
	__asm
	{
		// Run the hook implementation function and fallback to the original if it returned false
		push ebp
		mov ebp, esp
		push [ebp + 0x10]
		push [ebp + 0xC]
		push [ebp + 0x8]
		call LocalizedStringHookImpl
		add esp, 0xC
		test al, al
		jz fallback

		// Don't run the original function
		mov esp, ebp
		pop ebp
		ret

	fallback:
		// Execute replaced code and jump back to original function
		sub esp, 0x800
		mov edx, 0x51E049
		jmp edx
	}
}