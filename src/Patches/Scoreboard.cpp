#include "Scoreboard.hpp"

#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include "../Blam/BlamTypes.hpp"

namespace
{
	void GLScoreboardPlayerAllocatorHook();
	void GLScoreboardPlayerConstructorHook();
	void ScoreboardUpdateHook();
	void SetLocalPlayerHook();
}

namespace Patches
{
	namespace Scoreboard
	{
		void ApplyAll()
		{
			// Scoreboard hooks
			Hook(0x301A47, GLScoreboardPlayerAllocatorHook).Apply();
			Hook(0x2F8ED7, GLScoreboardPlayerConstructorHook).Apply();
			Hook(0x310163, ScoreboardUpdateHook).Apply();

			// Local player UID hook
			Hook(0x18A1D6, SetLocalPlayerHook).Apply();

			// Set scoreboard UIDs to player datum indexes
			Patch(0x31000F, { 0x3E, 0x8B, 0x4D, 0xC4 }).Apply(); // mov ecx, [ebp - 0x3C]
			Patch::NopFill(Pointer::Base(0x310013), 2);          // nop out leftover data
			Patch(0x310018, { 0x31, 0xC9 }).Apply();             // xor ecx, ecx
			Patch::NopFill(Pointer::Base(0x31001A), 4);          // nop out leftover data

			// Set podium UIDs to player datum indexes
			Patch(0x2E323E, { 0x8B, 0x47, 0x08 }).Apply(); // mov eax, [edi + 8]
			Patch::NopFill(Pointer::Base(0x2E3241), 4);    // nop out leftover data
			Patch(0x2E3248, { 0x31, 0xC0 }).Apply();       // xor eax, eax
			Patch::NopFill(Pointer::Base(0x2E324A), 5);    // nop out leftover data
		}
	}
}

namespace
{
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

	int GetUtf16Length(const char16_t *str, int maxLength)
	{
		int length = 0;
		while (length < maxLength && str[length])
			length++;
		return length;
	}

	PlayerNameString* GetPlayerName(void *playerData, int index)
	{
		if (index == -1)
			return nullptr;

		// Get function pointers
		typedef void* (*AllocateFunc)(size_t size);
		AllocateFunc Allocate = (AllocateFunc)0xD874A0;
		typedef void (*FreeFunc)(void *ptr);
		FreeFunc Free = (FreeFunc)0xD873C0;

		// Get the player's display name
		Pointer playerName = Pointer(playerData)(GameGlobals::Players::DisplayNameOffset);
		if (playerName.Read<char16_t>() == 0)
			return nullptr; // Don't update to a blank name

		// If the player name hasn't changed, don't bother updating it
		PlayerNameString* result = playerNames[index];
		if (result && memcmp(result->name, playerName, 32) == 0)
			return nullptr;

		// If the name is already allocated, decrease its reference count and free it if necessary
		if (result)
		{
			result->header.refCount--;
			if (!result->header.refCount)
			{
				Free(result);
				result = nullptr;
			}
		}

		// Allocate a new string for the name
		result = (PlayerNameString*)Allocate(sizeof(PlayerNameString));
		memset(result, 0, sizeof(result));
		result->header.refCount = 1;
		playerNames[index] = result;

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
			movzx eax, word ptr[ebp - 0x3C] // Player index
			push eax
			push esi                         // Player data
			call GetPlayerName
			add esp, 8
			test eax, eax
			jz done

			// Push the name string and increment the reference count
			mov[ebp - 0x14], eax // Temp variable we can use
			lea eax, [ebp - 0x14]
			push eax              // Push double-pointer to name string as an argument to sub_6AC840
			mov eax, [eax]        // Get the PlayerNameString object
			inc[eax]             // Increment reference count

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
			dec[eax]

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
}