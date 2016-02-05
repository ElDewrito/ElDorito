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
	void FmodSystemInitHook();
	int __cdecl DualWieldHook(unsigned short objectIndex);
	void SprintInputHook();
	double GetAspectRatio();
	int GetEquipmentCountHook(uint32_t unitIndex, short equipmentIndex);
	void EquipmentHook();
	void EquipmentTestHook();
	void GrenadeLoadoutHook();
	void ScopeLevelHook();
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

			// Fix random colored lighting
			Patch(0x14F2FFC, { 0x0, 0x0, 0x0, 0x0 }).Apply();

			// Hook game ticks
			Hook(0x105E64, GameTickHook, HookFlags::IsCall).Apply();

			// Used to call Patches::ApplyAfterTagsLoaded when tags have loaded
			Hook(0x1030EA, TagsLoadedHook).Apply();

			// Prevent FOV from being overridden when the game loads
			Patch::NopFill(Pointer::Base(0x25FA79), 10);
			Patch::NopFill(Pointer::Base(0x25FA86), 5);
			Hook(0x10CA02, FovHook).Apply();

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
			//Hook(0x4EC0, FmodSystemInitHook2).Apply(); // this doesn't appear to get called by the game
			
			// Enable dual-wielding
			Hook(0x761550, DualWieldHook).Apply();

			// Hook sprint input for dual-wielding
			Hook(0x6DFBB, SprintInputHook).Apply();

			// Hook scope level for dual-wielding
			Hook(0x1D50CB, ScopeLevelHook).Apply();

			// Equipment patches
			Patch::NopFill(Pointer::Base(0x786CFF), 6);
			Patch::NopFill(Pointer::Base(0x786CF7), 6);

			Hook(0x7A21D4, GetEquipmentCountHook, HookFlags::IsCall).Apply();
			/*Hook(0x139888, EquipmentHook, HookFlags::IsJmpIfNotEqual).Apply();
			Hook(0x786CF2, EquipmentTestHook).Apply();*/

			// Prevent game variant weapons from being overridden
			Pointer::Base(0x1A315F).Write<uint8_t>(0xEB);
			Pointer::Base(0x1A31A4).Write<uint8_t>(0xEB);
			Hook(0x1A3267, GrenadeLoadoutHook).Apply();

			// Remove exception handlers
			/*Patch::NopFill(Pointer::Base(0x2EA2B), 6);
			Patch::NopFill(Pointer::Base(0x2EC10), 6);
			//Patch::NopFill(Pointer::Base(0x7FC411), 6);
			Patch(0x7FC40B, { 0xC3 }).Apply();
			Patch(0x7FC42E, { 0xC3 }).Apply();
			Patch::NopFill(Pointer::Base(0x106057), 5);*/

			//Fix aspect ratio not matching resolution
			Hook(0x6648C9, GetAspectRatio, HookFlags::IsCall).Apply();
			Hook(0x216487, GetAspectRatio, HookFlags::IsCall).Apply();

			//Disable converting the game's resolution to 16:9
			Patch::NopFill(Pointer::Base(0x62217D), 2);
			Patch::NopFill(Pointer::Base(0x622183), 6);

			//Allow the user to select any resolution that Windows supports in the settings screen.
			Patch::NopFill(Pointer::Base(0x10BF1B), 2);
			Patch::NopFill(Pointer::Base(0x10BF21), 6);
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

	__declspec(naked) void FovHook()
	{
		__asm
		{
			// Override the FOV that the memmove before this sets
			mov eax, ds:[0x189D42C]
			mov ds : [0x2301D98], eax
			mov ecx, [edi + 0x18]
			push 0x50CA08
			ret
		}
	}

	__declspec(naked) void FmodSystemInitHook()
	{
		__asm
		{
			push	0; extradriverdata
			push	ebx; flags
			push	0x800; maxchannels
			push	eax; FMOD_SYSTEM
			call	dword ptr[ecx]; FMOD::System::init
			push	0x404EA4
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

	// TODO: Set up a proper data_array class...this is disgusting.

	bool UnitIsDualWielding(uint32_t unitIndex)
	{
		auto objectHeaderArrayPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		auto unitDatumPtr = objectHeaderArrayPtr(0x44)[0]((unitIndex & 0xFFFF) * 0x10)(0xC)[0];
		if (!unitDatumPtr)
			return false;
		auto dualWieldWeaponIndex = unitDatumPtr(0x2CB).Read<int8_t>();
		if (dualWieldWeaponIndex < 0 || dualWieldWeaponIndex >= 4)
			return false;
		typedef uint32_t(*UnitGetWeaponPtr)(uint32_t unitObject, short weaponIndex);
		auto UnitGetWeapon = reinterpret_cast<UnitGetWeaponPtr>(0xB454D0);
		return UnitGetWeapon(unitIndex, dualWieldWeaponIndex) != 0xFFFFFFFF;
	}

	bool PlayerIsDualWielding(uint32_t playerIndex)
	{
		auto playerArrayPtr = ElDorito::GetMainTls(GameGlobals::Players::TLSOffset)[0];
		auto playerUnitIndex = playerArrayPtr(0x44)[0]((playerIndex & 0xFFFF) * 0x2F08)(0x30).Read<uint32_t>();
		if (playerUnitIndex == 0xFFFFFFFF)
			return false;
		return UnitIsDualWielding(playerUnitIndex);
	}

	bool LocalPlayerIsDualWielding()
	{
		typedef uint32_t(*GetLocalPlayerPtr)(int index);
		auto GetLocalPlayer = reinterpret_cast<GetLocalPlayerPtr>(0x589C30);
		auto localPlayerIndex = GetLocalPlayer(0);
		if (localPlayerIndex == 0xFFFFFFFF)
			return false;
		return PlayerIsDualWielding(localPlayerIndex);
	}

	uint32_t GetObjectDataAddress(uint32_t objectDatum)
	{
		uint32_t objectIndex = objectDatum & UINT16_MAX;
		Pointer objectHeaderPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		uint32_t objectAddress = objectHeaderPtr(0x44).Read<uint32_t>() + 0xC + objectIndex * 0x10;
		return *(uint32_t*)(objectAddress);
	}

	int __cdecl DualWieldHook(unsigned short objectIndex)
	{
		if (!Modules::ModuleServer::Instance().VarServerDualWieldEnabledClient->ValueInt)
			return 0;
		auto& dorito = ElDorito::Instance();
		uint32_t index = *(uint32_t*)GetObjectDataAddress(objectIndex);
		char* tagAddr = (char*)Blam::Tags::GetTagAddress('weap', index);
		return ((*(uint32_t*)(tagAddr + 0x1D4) >> 22) & 1) == 1;
	}

	__declspec(naked) void SprintInputHook()
	{
		__asm
		{
			push	eax 
			call	LocalPlayerIsDualWielding
			test	al, al
			pop		eax
			jz		enable                          ; leave sprint enabled(for now) if not dual wielding
			and		ax, 0FEFFh                      ; disable by removing the 8th bit indicating no sprint input press
		enable :
			mov		dword ptr ds : [esi + 8], eax
			mov		ecx, edi
			push	046DFC0h
			ret
		}
	}

	int GetEquipmentCountHook(uint32_t unitIndex, short equipmentIndex)
	{
		// Disable equipment use if dual wielding
		if (UnitIsDualWielding(unitIndex))
			return 0;

		// Call the original function if not dual wielding
		typedef int(__cdecl* GetEquipmentCountFunc)(uint32_t unitIndex, short equipmentIndex);
		GetEquipmentCountFunc GetEquipmentCount = reinterpret_cast<GetEquipmentCountFunc>(0xB440F0);
		return GetEquipmentCount(unitIndex, equipmentIndex);
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

	void EquipmentHookImpl(unsigned short playerIndex, unsigned short equipmentIndex)
	{
		BYTE unkData[0x40];
		BYTE b69Data[0x48];

		Pointer& playerHeaderPtr = ElDorito::GetMainTls(GameGlobals::Players::TLSOffset)[0];
		uint32_t playerStructAddress = playerHeaderPtr(0x44).Read<uint32_t>() + playerIndex * GameGlobals::Players::PlayerEntryLength;
		uint32_t playerObjectDatum = *(uint32_t*)(playerStructAddress + 0x30);

		Pointer &objectHeaderPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		uint32_t objectAddress = objectHeaderPtr(0x44).Read<uint32_t>() + 0xC + equipmentIndex * 0x10;
		uint32_t objectDataAddress = *(uint32_t*)objectAddress;
		uint32_t index = *(uint32_t*)objectDataAddress;

		memset(b69Data, 0, 0x48);
		*(uint32_t*)(b69Data) = 0x3D; // object type?
		*(uint32_t*)(b69Data + 4) = equipmentIndex;

		// add equipment to the player, also removes the object from gameworld
		typedef char(__cdecl* Objects_AttachPtr)(int a1, void* a2);
		auto Objects_Attach = reinterpret_cast<Objects_AttachPtr>(0xB69C50);
		Objects_Attach(playerObjectDatum, b69Data); // 82182C48

		// prints text to the HUD, taken from HO's code
		{
			typedef int(__thiscall* sub_589680Ptr)(void* thisPtr, int a2);
			auto sub_589680 = reinterpret_cast<sub_589680Ptr>(0x589680);
			sub_589680(&unkData, playerIndex);

			typedef BOOL(__thiscall* sub_589770Ptr)(void* thisPtr);
			auto sub_589770 = reinterpret_cast<sub_589770Ptr>(0x589770);
			while ((unsigned __int8)sub_589770(&unkData))
			{
				typedef int(__thiscall* sub_589760Ptr)(void* thisPtr);
				auto sub_589760 = reinterpret_cast<sub_589760Ptr>(0x589760);
				int v9 = sub_589760(&unkData);

				typedef int(__cdecl* sub_A95850Ptr)(unsigned int a1, short a2);
				auto sub_A95850 = reinterpret_cast<sub_A95850Ptr>(0xA95850);
				sub_A95850(v9, index);
			}
		}

		// unsure what these do, taken from HO code
		{
			typedef int(__cdecl *sub_B887B0Ptr)(unsigned short a1, unsigned short a2);
			auto sub_B887B0 = reinterpret_cast<sub_B887B0Ptr>(0xB887B0);
			sub_B887B0(playerIndex, index); // sub_82437A08

			typedef void(_cdecl *sub_4B31C0Ptr)(unsigned short a1, unsigned short a2);
			auto sub_4B31C0 = reinterpret_cast<sub_4B31C0Ptr>(0x4B31C0);
			sub_4B31C0(playerObjectDatum, index); // sub_8249A1A0
		}

		// called by powerup pickup func, deletes the item but also crashes the game when used with equipment
		// not needed since Objects_Attach removes it from the game world
		/*
		typedef int(__cdecl *Objects_DeletePtr)(int objectIndex);
		auto Objects_Delete = reinterpret_cast<Objects_DeletePtr>(0xB57090);
		Objects_Delete(equipmentIndex);*/
	}

	__declspec(naked) void EquipmentHook()
	{
		__asm
		{
			mov edx, 0x531D70
			call edx
			mov esi, [ebp + 8]
			push edi
			push esi
			test al, al
			jz DoEquipmentHook
			mov edx, 0x4B4A20
			call edx
			add esp, 8
			push 0x5397D8
			ret
		DoEquipmentHook :
			call EquipmentHookImpl
			add esp, 8
			push 0x5397D8
			ret
		}
	}

	// replaces some check that always fails and allows you to throw equipment, as long as it still has a use remaining
	// if you've used up all the uses and try throwing again it'll still destroy your first thrown equipment though
	// H3E likely has this check somewhere closer to the top of the hooked func, but I couldn't find it
	__declspec(naked) void EquipmentTestHook()
	{
		__asm
		{
			push dword ptr[ebp + 8]
			mov edx, 0xB89190 // Equipment_GetNumRemainingUses(int16 objectIdx)
			call edx
			add esp, 4
			push 0xB86CFD
			ret
		}
	}

	double GetAspectRatio()
	{
		int* gameResolution = reinterpret_cast<int*>(0x19106C0);
		return ((double)gameResolution[0] / (double)gameResolution[1]);
	}

	// scope level is an int16 with -1 indicating no scope, 0 indicating first level, 1 indicating second level etc.
	__declspec(naked) void ScopeLevelHook()
	{
		__asm
		{
			mov		word ptr ds : [edi + esi + 32Ah], 0FFFFh	; no scope by default
			push	eax
			push	ecx
			call	LocalPlayerIsDualWielding
			test	al, al
			pop		ecx
			pop		eax
			jnz		noscope                                     ; prevent scoping when dual wielding
			mov		word ptr ds : [edi + esi + 32Ah], ax        ; otherwise use intended scope level
		noscope:
			push	05D50D3h
			ret
		}
	}
}
