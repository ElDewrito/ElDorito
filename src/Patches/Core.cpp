#include "Core.hpp"

#include "../ElDorito.hpp"
#include "../ElPatches.hpp"
#include "../Patch.hpp"
#include "../Blam/Tags/Scenario.hpp"

namespace
{
	void GameTickHook(int frames, float *deltaTimeInfo);
	void TagsLoadedHook();
	void FovHook();
	void GrenadeLoadoutHook();
	void HostObjectShieldHook();
	void HostObjectHealthHook();
	void ClientObjectShieldHook();
	void ClientObjectShieldHook2();
	void ClientObjectHealthHook();
}

namespace Patches
{
	namespace Core
	{
		void ApplyAll()
		{
			// Enable tag edits
			Patch(0x101A5B, { 0xEB }).Apply();
			Patch::NopFill(Pointer::Base(0x102874), 2);
			Patch::NopFill(Pointer::Base(0x1030AA), 2);

			// No --account args patch
			Patch(0x43731A, { 0xEB, 0x0E }).Apply();
			Patch(0x4373AD, { 0xEB, 0x03 }).Apply();

			// Remove preferences.dat hash check
			Patch::NopFill(Pointer::Base(0x10C99A), 0x6);

			// Patch to allow spawning AI through effects
			Patch::NopFill(Pointer::Base(0x1033321), 2);

			// Prevent game variant weapons from being overridden
			Pointer::Base(0x1A315F).Write<uint8_t>(0xEB);
			Pointer::Base(0x1A31A4).Write<uint8_t>(0xEB);
			Hook(0x1A3267, GrenadeLoadoutHook).Apply();

			// Hook game ticks
			Hook(0x105E64, GameTickHook, HookFlags::IsCall).Apply();

			// Prevent FOV from being overridden when the game loads
			Patch::NopFill(Pointer::Base(0x25FA79), 10);
			Patch::NopFill(Pointer::Base(0x25FA86), 5);
			Hook(0x10CA02, FovHook).Apply();

			// Used to call Patches::ApplyAfterTagsLoaded when tags have loaded
			Hook(0x1030EA, TagsLoadedHook).Apply();

			// player damage descope hooks
			Hook(0x7553A0, HostObjectShieldHook).Apply();		// host object shield
			Hook(0x754B4E, HostObjectHealthHook).Apply();		// host object health
			Hook(0x733F13, ClientObjectShieldHook).Apply();		// client object shield
			Hook(0x756FB7, ClientObjectShieldHook2).Apply();	// client object shield
			Hook(0x7329CE, ClientObjectHealthHook).Apply();		// client object health

			// Adds the FMOD WASAPI output fix from FMODEx 4.44.56, which stops weird popping sound at startup
			// TODO: maybe find a way to update HO's FMOD, HO is using 4.26.6 which is ancient
			Patch(0x100DA75, { 0x2 }).Apply();

			// Remove exception handlers
			/*Patch::NopFill(Pointer::Base(0x2EA2B), 6);
			Patch::NopFill(Pointer::Base(0x2EC10), 6);
			//Patch::NopFill(Pointer::Base(0x7FC411), 6);
			Patch(0x7FC40B, { 0xC3 }).Apply();
			Patch(0x7FC42E, { 0xC3 }).Apply();
			Patch::NopFill(Pointer::Base(0x106057), 5);*/
		}
	}
}

namespace
{
	void GameTickHook(int frames, float *deltaTimeInfo)
	{
		// Tick ElDorito
		float deltaTime = *deltaTimeInfo;
		ElDorito::Instance().Tick(std::chrono::duration<double>(deltaTime));

		// Tick the game
		typedef void (*GameTickFunc)(int frames, float *deltaTimeInfo);
		GameTickFunc GameTick = reinterpret_cast<GameTickFunc>(0x5336F0);
		GameTick(frames, deltaTimeInfo);
	}

	__declspec(naked) void FovHook()
	{
		__asm
		{
			// Override the FOV that the memmove before this sets
			mov eax, ds:[0x189D42C]
			mov ds:[0x2301D98], eax
			mov ecx, [edi + 0x18]
			push 0x50CA08
			ret
		}
	}

	__declspec(naked) void TagsLoadedHook()
	{
		__asm
		{
			call Patches::ApplyAfterTagsLoaded
			push 0x6D617467
			push 0x5030EF
			ret
		}
	}

	void GrenadeLoadoutHookImpl(uint8_t* unit)
	{
		// Based off of 0x8227B48C in H3 non-TU

		// TODO: Clean this up, hardcoded offsets are hacky
		const size_t GrenadeCountOffset = 0x320;
		const size_t ControllingPlayerOffset = 0x198;
		auto grenadeCounts = unit + GrenadeCountOffset; // 0 = frag, 1 = plasma, 2 = spike, 3 = firebomb
		auto playerIndex = *reinterpret_cast<int16_t*>(unit + ControllingPlayerOffset);
		if (playerIndex < 0)
		{
			memset(grenadeCounts, 0, 4);
			return;
		}

		// Get the player's grenade setting (haxhaxhax)
		const size_t DatumArrayPtrOffset = 0x44;
		const size_t PlayerSize = 0x2F08;
		const size_t GrenadeSettingOffset = 0x2DB4;
		auto grenadeSettingPtr = ElDorito::GetMainTls(GameGlobals::Players::TLSOffset)[0][DatumArrayPtrOffset](PlayerSize * playerIndex + GrenadeSettingOffset);
		auto grenadeSetting = grenadeSettingPtr.Read<int16_t>();

		// Get the current scenario tag
		auto scenario = Blam::Tags::GetCurrentScenario();

		// If the setting is none (2) or the scenario has invalid starting
		// profile data, set the grenade counts to 0 and return
		if (grenadeSetting == 2 || !scenario->StartingProfile)
		{
			memset(grenadeCounts, 0, 4);
			return;
		}

		// Load the grenade counts from the scenario tag
		auto profile = &scenario->StartingProfile[0];
		grenadeCounts[0] = profile->FragGrenades;
		grenadeCounts[1] = profile->PlasmaGrenades;
		grenadeCounts[2] = profile->SpikeGrenades;
		grenadeCounts[3] = profile->FirebombGrenades;
	}

	__declspec(naked) void GrenadeLoadoutHook()
	{
		__asm
		{
			push edi // Unit object data
			call GrenadeLoadoutHookImpl
			add esp, 4
			push 0x5A32C7
			ret
		}
	}

	// hook @ 0xB553A0
	__declspec(naked) void HostObjectShieldHook()
	{
		__asm
		{
			pushad

			; get tls info
			mov		eax, dword ptr fs:[02Ch]	; tls array address
			mov		eax, dword ptr ds:[eax]		; slot 0 tls address

			; get local player object offset
			mov		ebx, dword ptr ds:[eax + 05Ch]	; local player mappings
			mov		ebx, dword ptr ds:[ebx + 014h]	; local player datum
			and		ebx, 0FFFFh						; local player index
			shl		ebx, 4							; multiply by object entry size of 16 bytes
			add		ebx, 0Ch						; add object header size

			; get local player object data address
			mov		ecx, dword ptr ds:[eax + 0448h]	; object header address
			mov		ecx, dword ptr ds:[ecx + 044h]	; first object address
			add		ecx, ebx						; local player object address
			mov		ecx, [ecx]						; local player object data address

			; check if damaging local player object and descope if so
			cmp		edi, ecx
			jne		orig
		
			; descope local player
			mov		edx, dword ptr ds:[eax + 0C4h]		; player control globals
			mov		word ptr ds:[edx + 032Ah], 0FFFFh	; descope

			orig:
			popad
			ucomiss	xmm1, xmm0
			movss   dword ptr ds:[edi + 100h], xmm1
			push	0B553ABh
			ret
		}
	}

	// hook @ 0xB54B4E
	__declspec(naked) void HostObjectHealthHook()
	{
		__asm
		{
			pushad

			; get tls info
			mov		eax, dword ptr fs:[02Ch]	; tls array address
			mov		eax, dword ptr ds:[eax]		; slot 0 tls address

			; get local player object offset
			mov		ebx, dword ptr ds:[eax + 05Ch]	; local player mappings
			mov		ebx, dword ptr ds:[ebx + 014h]	; local player datum
			and		ebx, 0FFFFh						; local player index
			shl		ebx, 4							; multiply by object entry size of 16 bytes
			add		ebx, 0Ch						; add object header size

			; get local player object data address
			mov		ecx, dword ptr ds:[eax + 0448h]	; object header address
			mov		ecx, dword ptr ds:[ecx + 044h]	; first object address
			add		ecx, ebx						; local player object address
			mov		ecx, [ecx]						; local player object data address

			; check if damaging local player object and descope if so
			cmp		edi, ecx
			jne		orig

			; descope local player
			mov		edx, dword ptr ds:[eax + 0C4h]		; player control globals
			mov		word ptr ds:[edx + 032Ah], 0FFFFh	; descope

			orig:
			popad
			movss   dword ptr ds:[edi + 0FCh], xmm1
			push	0B54B56h
			ret
		}
	}

	// hook @ 0xB33F13
	__declspec(naked) void ClientObjectShieldHook()
	{
		__asm
		{
			pushad

			; get tls info
			mov		eax, dword ptr fs:[02Ch]	; tls array address
			mov		eax, dword ptr ds:[eax]		; slot 0 tls address

			; get local player object offset
			mov		ebx, dword ptr ds:[eax + 05Ch]	; local player mappings
			mov		ebx, dword ptr ds:[ebx + 014h]	; local player datum
			and		ebx, 0FFFFh						; local player index
			shl		ebx, 4							; multiply by object entry size of 16 bytes
			add		ebx, 0Ch						; add object header size

			; get local player object data address
			mov		ecx, dword ptr ds:[eax + 0448h]	; object header address
			mov		ecx, dword ptr ds:[ecx + 044h]	; first object address
			add		ecx, ebx						; local player object address
			mov		ecx, [ecx]						; local player object data address

			; check if damaging local player object
			cmp		edi, ecx
			jne		orig

			; only descope if shield is decreasing at a rate greater than epsilon
			mov		esi, 03C23D70Ah						; use an epsilon of 0.01f
			movd	xmm7, esi
			movss	xmm6, dword ptr ds:[edi + 100h]		; get original shield
			subss	xmm6, xmm0							; get negative shield delta (orig - new)
			comiss	xmm6, xmm7							; compare shield delta with epsilon
			jb		orig								; skip descope if delta is less than epsilon

			; descope local player
			mov		edx, dword ptr ds:[eax + 0C4h]		; player control globals
			mov		word ptr ds:[edx + 032Ah], 0FFFFh	; descope

			orig:
			popad
			movss   dword ptr ds:[edi + 100h], xmm0
			push	0B33F1Bh
			ret
		}
	}

	// hook @ 0xB56FB7 - used in the case of an instant shield depletion from a charged plasma pistol shot
	__declspec(naked) void ClientObjectShieldHook2()
	{
		__asm
		{
			push	eax

			; get tls info
			mov		eax, dword ptr fs:[02Ch]	; tls array address
			mov		eax, dword ptr ds:[eax]		; slot 0 tls address

			; descope local player
			mov		eax, dword ptr ds:[eax + 0C4h]		; player control globals
			mov		word ptr ds:[eax + 032Ah], 0FFFFh	; descope

			pop		eax

			mov		dword ptr ds:[edi + 100h], 0
			push	0B56FC1h
			ret
		}
	}

	// hook @ 0xB329CE
	__declspec(naked) void ClientObjectHealthHook()
	{
		__asm
		{
			pushad

			; get tls info
			mov		eax, dword ptr fs:[02Ch]	; tls array address
			mov		eax, dword ptr ds:[eax]		; slot 0 tls address

			; get local player object offset
			mov		ebx, dword ptr ds:[eax + 05Ch]	; local player mappings
			mov		ebx, dword ptr ds:[ebx + 014h]	; local player datum
			and		ebx, 0FFFFh						; local player index
			shl		ebx, 4							; multiply by object entry size of 16 bytes
			add		ebx, 0Ch						; add object header size

			; get local player object data address
			mov		edx, dword ptr ds:[eax + 0448h]	; object header address
			mov		edx, dword ptr ds:[edx + 044h]	; first object address
			add		edx, ebx						; local player object address
			mov		edx, [edx]						; local player object data address

			; check if damaging local player object
			cmp		ecx, edx
			jne		orig

			; only descope if health is decreasing at a rate greater than epsilon
			mov		esi, 03C23D70Ah						; use an epsilon of 0.01f
			movd	xmm7, esi
			movss	xmm6, dword ptr ds:[ecx + 0FCh]		; get original health
			subss	xmm6, xmm0							; get negative health delta (orig - new)
			comiss	xmm6, xmm7							; compare health delta with epsilon
			jb		orig								; skip descope if delta is less than epsilon

			; descope local player
			mov		edx, dword ptr ds:[eax + 0C4h]		; player control globals
			mov		word ptr ds:[edx + 032Ah], 0FFFFh	; descope

			orig:
			popad
			movss   dword ptr ds:[ecx + 0FCh], xmm0
			push	0B329D6h
			ret
		}
	}
}