#include "Core.hpp"

#include "../ElDorito.hpp"
#include "../ElPatches.hpp"
#include "../Patch.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"
#include "../Modules/ModuleGame.hpp"
#include "../Modules/ModuleServer.hpp"

namespace
{
	void GameTickHook();
	void TagsLoadedHook();
	void FovHook();
	void FmodSystemInitHook();
	void FmodSystemInitHook2();
	int __cdecl DualWieldHook(unsigned short objectIndex);
	void SprintInputHook();
	double GetAspectRatio();
	int GetEquipmentCountHook(uint32_t unitIndex, short equipmentIndex);
	void GrenadeLoadoutHook();
	void ScopeLevelHook();
	void ShutdownHook();
	const char *GetMapsFolderHook();
	bool LoadMapHook(void *data);
	void LoadLevelHook(uint8_t* data, char n2, int n3, int n4);
	void GameStartHook();

	std::vector<Patches::Core::ShutdownCallback> shutdownCallbacks;
	std::string MapsFolder;
	std::string MapFormatString;
	std::string StringIdsPath;
	std::string TagsPath;
	std::string TagListPath;
	std::string ResourcesPath;
	std::string TexturesPath;
	std::string TexturesBPath;
	std::string AudioPath;
	std::string VideoPath;
	std::string FontsPath;

	std::vector<Patches::Core::MapLoadedCallback> mapLoadedCallbacks;
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

			// prevent hf2p services from being registered
			Patch(0x003B8810, { 0xC3 }).Apply();

			// Remove preferences.dat hash check
			Patch::NopFill(Pointer::Base(0x10C99A), 0x6);

			// Patch to allow spawning AI through effects
			Patch::NopFill(Pointer::Base(0x1033321), 2);

			// Fix random colored lighting
			Patch(0x14F2FFC, { 0x0, 0x0, 0x0, 0x0 }).Apply();

			// Hook game ticks
			Hook(0x105ABA, GameTickHook, HookFlags::IsCall).Apply();
			Hook(0x105AD7, GameTickHook, HookFlags::IsCall).Apply();
			Hook(0x1063E6, GameTickHook, HookFlags::IsCall).Apply();

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

			// TODO: experiment with this to prevent lag
			// https://www.fmod.org/docs/content/generated/FMOD_System_SetStreamBufferSize.html
			//*reinterpret_cast<uint8_t*>(0x404F42 + 1) = 8; // FMOD_TIMEUNIT_RAWBYTES
			//*reinterpret_cast<uint32_t*>(0x404F44 + 1) = 1024 * 1024; // filebuffersize
			//*reinterpret_cast<uint8_t*>(0x404F42 + 1) = 1; // FMOD_TIMEUNIT_RAWBYTES
			//*reinterpret_cast<uint32_t*>(0x404F44 + 1) = 1000; // filebuffersize

			// TODO: experiment with this to potentially improve playback latency
			// http://www.fmod.org/docs/content/generated/FMOD_System_SetDSPBufferSize.html
			//Patch::NopFill(0x404E7B, 2); // allow it to be called
			//*reinterpret_cast<uint8_t*>(0x404E7D + 1) = 4; // numbuffers
			//*reinterpret_cast<uint32_t*>(0x404E7F + 1) = 2048; // bufferlength

			// increase max virtual audio channels from 64 to 256
			// http://www.fmod.org/docs/content/generated/FMOD_System_Init.html
			Hook(0x4E9C, FmodSystemInitHook).Apply();
			Hook(0x4EC0, FmodSystemInitHook2).Apply();

			// increase software channels from 192 to 256
			// http://www.fmod.org/docs/content/generated/FMOD_System_SetSoftwareChannels.html
			*reinterpret_cast<uint32_t*>(0x404DF8 + 1) = 256;

			// Enable dual-wielding
			Hook(0x761550, DualWieldHook).Apply();

			// Hook sprint input for dual-wielding
			Hook(0x6DFBB, SprintInputHook).Apply();

			// Hook scope level for dual-wielding
			Hook(0x1D50CB, ScopeLevelHook).Apply();

			Hook(0x7A21D4, GetEquipmentCountHook, HookFlags::IsCall).Apply();

			// Prevent game variant weapons from being overridden
			Pointer::Base(0x1A315F).Write<uint8_t>(0xEB);
			Pointer::Base(0x1A31A4).Write<uint8_t>(0xEB);
			Hook(0x1A3267, GrenadeLoadoutHook).Apply();

			// fix spectating
			Patch::NopFill(Pointer::Base(0x192FFD), 6);

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
			
			// Maps folder override
			Hook(0x101FC0, GetMapsFolderHook).Apply();
			SetMapsFolder("maps\\");

			// Run callbacks on engine shutdown
			Hook(0x2EBD7, ShutdownHook, HookFlags::IsCall).Apply();

			// Map loading
			Hook(0x10FC2C, LoadMapHook, HookFlags::IsCall).Apply();
			Hook(0x1671BE, LoadMapHook, HookFlags::IsCall).Apply();
			Hook(0x167B4F, LoadMapHook, HookFlags::IsCall).Apply();

			Hook(0x14C7FF, LoadLevelHook, HookFlags::IsCall).Apply();
			Hook(0x133069, GameStartHook, HookFlags::IsCall).Apply();
		}

		void OnShutdown(ShutdownCallback callback)
		{
			shutdownCallbacks.push_back(callback);
		}

		void ExecuteShutdownCallbacks()
		{
			for (auto &&callback : shutdownCallbacks)
				callback();
		}

		void OnMapLoaded(MapLoadedCallback callback)
		{
			mapLoadedCallbacks.push_back(callback);
		}
		
		void SetMapsFolder(const std::string &path)
		{
			MapsFolder = path;
			MapFormatString = MapsFolder + "%s.map";
			StringIdsPath = MapsFolder + "string_ids.dat";
			TagsPath = MapsFolder + "tags.dat";
			TagListPath = MapsFolder + "tag_list.csv";
			ResourcesPath = MapsFolder + "resources.dat";
			TexturesPath = MapsFolder + "textures.dat";
			TexturesBPath = MapsFolder + "textures_b.dat";
			AudioPath = MapsFolder + "audio.dat";
			VideoPath = MapsFolder + "video.dat";
			FontsPath = MapsFolder + "fonts\\";

			Pointer::Base(0x1AC050).Write(MapFormatString.c_str());

			Pointer::Base(0x149CFEC).Write(StringIdsPath.c_str());
			Pointer::Base(0x149CFF0).Write(TagsPath.c_str());
			Pointer::Base(0x149CFF4).Write(TagListPath.c_str());
			Pointer::Base(0x149CFF8).Write(ResourcesPath.c_str());
			Pointer::Base(0x149CFFC).Write(TexturesPath.c_str());
			Pointer::Base(0x149D000).Write(TexturesBPath.c_str());
			Pointer::Base(0x149D004).Write(AudioPath.c_str());
			Pointer::Base(0x149D008).Write(VideoPath.c_str());

			Pointer::Base(0x149D358).Write(FontsPath.c_str());
			Pointer::Base(0x149D35C).Write(FontsPath.c_str());

			//Update the list of maps
			Modules::ModuleGame::Instance().UpdateMapList();
			Modules::ModuleGame::Instance().UpdateCustomMapList();
		}
	}
}

namespace
{
	void GameTickHook()
	{
		// Tick ElDorito
		ElDorito::Instance().Tick();

		// Call replaced function
		typedef void(*sub_5547F0_Ptr)();
		auto sub_5547F0 = reinterpret_cast<sub_5547F0_Ptr>(0x5547F0);
		sub_5547F0();
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
			push	100h; maxchannels
			push	eax; FMOD_SYSTEM
			call	dword ptr[ecx]; FMOD::System::init
			push	0x404EA4
			ret
		}
	}

	__declspec(naked) void FmodSystemInitHook2()
	{
		__asm
		{
			push	0; extradriverdata
			push	ebx; flags
			push	100h; maxchannels
			push	eax; FMOD_SYSTEM
			call	dword ptr[ecx]; FMOD::System::init
			push	0x404EC8
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

		// Get the player's grenade setting
		auto &players = Blam::Players::GetPlayers();
		auto grenadeSetting = players[playerIndex].SpawnGrenadeSetting;

		// Get the current scenario tag
		auto scenario = Blam::Tags::Scenario::GetCurrentScenario();

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

	bool UnitIsDualWielding(Blam::DatumIndex unitIndex)
	{
		if (!unitIndex)
			return false;
		auto objectHeaderArrayPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		auto unitDatumPtr = objectHeaderArrayPtr(0x44)[0](unitIndex.Index() * 0x10)(0xC)[0];

		if (!unitDatumPtr)
			return false;

		auto dualWieldWeaponIndex = unitDatumPtr(0x2CB).Read<int8_t>();

		if (dualWieldWeaponIndex < 0 || dualWieldWeaponIndex >= 4)
			return false;

		typedef uint32_t(*UnitGetWeaponPtr)(uint32_t unitObject, short weaponIndex);
		auto UnitGetWeapon = reinterpret_cast<UnitGetWeaponPtr>(0xB454D0);

		return UnitGetWeapon(unitIndex, dualWieldWeaponIndex) != 0xFFFFFFFF;
	}

	bool PlayerIsDualWielding(Blam::DatumIndex playerIndex)
	{
		auto &players = Blam::Players::GetPlayers();
		return UnitIsDualWielding(players[playerIndex].SlaveUnit);
	}

	bool LocalPlayerIsDualWielding()
	{
		auto localPlayer = Blam::Players::GetLocalPlayer(0);
		if (localPlayer == Blam::DatumIndex::Null)
			return false;
		return PlayerIsDualWielding(localPlayer);
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
		using Blam::Tags::TagInstance;
		using Blam::Tags::Items::Weapon;

		if (!Modules::ModuleServer::Instance().VarServerDualWieldEnabledClient->ValueInt)
			return 0;

		auto index = *(uint32_t*)GetObjectDataAddress(objectIndex);
		auto *weapon = TagInstance(index).GetDefinition<Weapon>();

		return ((int32_t)weapon->WeaponFlags1 & (int32_t)Weapon::Flags1::CanBeDualWielded) != 0;
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

	void ShutdownHook()
	{
		Patches::Core::ExecuteShutdownCallbacks();

		typedef void(*EngineShutdownPtr)();
		auto EngineShutdown = reinterpret_cast<EngineShutdownPtr>(0x42E410);
		EngineShutdown();
	}
	
	const char* GetMapsFolderHook()
	{
		return MapsFolder.c_str();
	}

	bool LoadMapHook(void *data)
	{
		typedef bool(*LoadMapPtr)(void *data);
		auto LoadMap = reinterpret_cast<LoadMapPtr>(0x566EF0);
		if (!LoadMap(data))
			return false;

		for (auto &&callback : mapLoadedCallbacks)
			callback(static_cast<const char*>(data) + 0x24); // hax

		return true;
	}

	void LoadLevelHook(uint8_t* data, char n2, int n3, int n4)
	{
		typedef int(__cdecl *LoadLevelPtr)(uint8_t* data, char n2, int n3, int n4);
		auto LoadLevel = reinterpret_cast<LoadLevelPtr>(0x0054A6C0);

		*reinterpret_cast<uint32_t*>(data + 0x111C) = 0x08081002;
		*reinterpret_cast<uint32_t*>(data + 0x1120) = 0x08080808;
		*reinterpret_cast<uint32_t*>(data + 0x1124) = 0x08080808;

		LoadLevel(data, n2, n3, n4);
	}

	void GameStartHook()
	{
		typedef void(*GameStartPtr)();
		auto GameStart = reinterpret_cast<GameStartPtr>(0x0054EA60);

		GameStart();

		auto engineGlobalsPtr = ElDorito::GetMainTls(0x48);
		if (!engineGlobalsPtr)
			return;

		auto engineGobals = engineGlobalsPtr[0];

		// fix in-game team switching for engines that support it
		engineGobals(0x8).Write(engineGobals(0x4).Read<uint32_t>());
	}
}
