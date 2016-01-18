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
	void FmodSystemInitHook();
	//void FmodSystemInitHook2();
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

			// Force descoping for all weapons
			Pointer::Base(0x73F1E6).Write<uint8_t>(0);

			// Adds the FMOD WASAPI output fix from FMODEx 4.44.56, which stops weird popping sound at startup
			// TODO: maybe find a way to update HO's FMOD, HO is using 4.26.6 which is ancient
			Patch(0x100DA75, { 0x2 }).Apply();

			// this doesn't get called by the game and manually calling it doesn't seem to work, leaving for future reference
			// tweak dsp buffer settings for lower latency
			// http://www.fmod.org/docs/content/generated/FMOD_System_SetDSPBufferSize.html
			//Patch(0x4E7E, { 0x4 }).Apply();		// numbuffers = 4
			//Patch(0x4E81, { 0x1 }).Apply();		// bufferlength = 256

			// increase max virtual audio channels from 64 to 2048
			// http://www.fmod.org/docs/content/generated/FMOD_System_Init.html
			Hook(0x4E9C, FmodSystemInitHook).Apply();
			//Hook(0x4EC0, FmodSystemInitHook2).Apply();	// this doesn't appear to get called by the game

			// Fix random colored lighting
			Patch(0x14F2FFC, { 0x0, 0x0, 0x0, 0x0 }).Apply();

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
	
	__declspec(naked) void FmodSystemInitHook()
 	{
 		__asm
 		{
 			push	0				; extradriverdata
 			push	ebx				; flags
 			push	0x800			; maxchannels
 			push	eax				; FMOD_SYSTEM
 			call	dword ptr [ecx]	; FMOD::System::init
 			push	0x404EA4
 			ret
 		}
 	}

	//__declspec(naked) void FmodSystemInitHook2()
 //	{
 //		__asm
 //		{
 //			push	0				; extradriverdata
 //			push	ebx				; flags
 //			push	0x800			; maxchannels
 //			push	eax				; FMOD_SYSTEM
 //			call	dword ptr [ecx]	; FMOD::System::init
	//		push	0x404EC8
 //			ret
 //		}
 //	}

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
}
