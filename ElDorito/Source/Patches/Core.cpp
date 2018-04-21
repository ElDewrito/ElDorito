#include "Core.hpp"

#include "../ElDorito.hpp"
#include "../ElPatches.hpp"
#include "../Patch.hpp"
#include "../Blam/Math/RealColorARGB.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Items/DefinitionWeapon.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Modules/ModuleGame.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Console.hpp"
#include "boost/filesystem.hpp"
#include <codecvt>
#include <Shlobj.h>

namespace
{
	void GameTickHook();
	void TagsLoadedHook();
	void FovHook();
	double AspectRatioHook();
	void ActiveCamoViewModelClipHook(float *nearPlane, float *farPlane);
	void GrenadeLoadoutHook();
	void ShutdownHook();
	const char *GetMapsFolderHook();
	bool LoadMapHook(void *data);
	void LoadLevelHook(uint8_t* data, char n2, int n3, int n4);
	void GameStartHook();
	void __fastcall EdgeDropHook(void* thisptr, void* unused, int a2, int a3, int a4, float* a5);
	void __cdecl BipedFeetZoneOffsetHook(uint32_t bipedObjectIndex, Blam::Math::RealVector3D *position, float *height, float *radius);
	char GetBinkVideoPathHook(int p_VideoID, char *p_DestBuf);
	void DirtyDiskErrorHook();
	int __cdecl GetScreenshotFolderHook(wchar_t *path);
	void __cdecl HsPrintHook(const char *message);
	void ContrailFixHook();
	void HillColorHook();

	std::vector<Patches::Core::ShutdownCallback> shutdownCallbacks;
	std::string MapsFolder;
	std::string MapFormatString;
	std::string StringIdsPath;
	std::string TagsPath;
	std::string TagListPath;
	std::string ResourcesPath;
	std::string ResourcesBPath;
	std::string TexturesPath;
	std::string TexturesBPath;
	std::string AudioPath;
	std::string FontsPath;

	std::vector<Patches::Core::MapLoadedCallback> mapLoadedCallbacks;
	std::vector<Patches::Core::GameStartCallback> gameStartCallbacks;
}

namespace Patches::Core
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

		Hook(0x152C15, GameStartHook, HookFlags::IsCall).Apply();
		Hook(0x14EB62, GameStartHook, HookFlags::IsCall).Apply();
		Hook(0x14EB54, GameStartHook, HookFlags::IsCall).Apply();

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
		Hook(0x663B36, ActiveCamoViewModelClipHook, HookFlags::IsCall).Apply();
		// fix active camo issue with low shadow quality (local unit shadows will still be rendered)
		Patch(0x66B0CE, 0x90, 6).Apply();

		//Fix aspect ratio not matching resolution
		Hook(0x6648C9, AspectRatioHook, HookFlags::IsCall).Apply();
		Hook(0x216487, AspectRatioHook, HookFlags::IsCall).Apply();

		//Disable converting the game's resolution to 16:9
		Patch::NopFill(Pointer::Base(0x62217D), 2);
		Patch::NopFill(Pointer::Base(0x622183), 6);

		//Allow the user to select any resolution that Windows supports in the settings screen.
		Patch::NopFill(Pointer::Base(0x10BF1B), 2);
		Patch::NopFill(Pointer::Base(0x10BF21), 6);

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

		Hook(0x324701, EdgeDropHook, HookFlags::IsCall).Apply();
		// Fixes an issue where biped feet are just below the zone bottom causing 
		// it not to register flag caps, teleporter usage etc..
		Hook(0x7A111B, BipedFeetZoneOffsetHook, HookFlags::IsCall).Apply();

		Hook(0x10590B, GetBinkVideoPathHook, HookFlags::IsCall).Apply();

		Hook(0x20F4AD, GetScreenshotFolderHook, HookFlags::IsCall).Apply();
		Hook(0x20F44B, GetScreenshotFolderHook, HookFlags::IsCall).Apply();

		Pointer(0x530FAA).Write<float>(7); // podium duration in seconds

		// fixes the amd freeze
		Hook(0x658061, ContrailFixHook).Apply();
		// prevent hill zone luminosity from dropping below the visible threshold
		Hook(0x5D6B1C, HillColorHook).Apply();
		

#ifndef _DEBUG
		// Dirty disk error at 0x0xA9F6D0 is disabled in this build
		Hook(0x69F6C0, DirtyDiskErrorHook).Apply();
#else
		// hsc print functionality
		Hook(0x32FE9A, HsPrintHook, HookFlags::IsCall).Apply();
#endif
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
		ResourcesBPath = MapsFolder + "resources_b.dat";
		TexturesPath = MapsFolder + "textures.dat";
		TexturesBPath = MapsFolder + "textures_b.dat";
		AudioPath = MapsFolder + "audio.dat";
		FontsPath = MapsFolder + "fonts\\";

		Pointer::Base(0x1AC050).Write(MapFormatString.c_str());

		Pointer::Base(0x149CFEC).Write(StringIdsPath.c_str());
		Pointer::Base(0x149CFF0).Write(TagsPath.c_str());
		Pointer::Base(0x149CFF4).Write(TagListPath.c_str());
		Pointer::Base(0x149CFF8).Write(ResourcesPath.c_str());
		Pointer::Base(0x149D008).Write(ResourcesBPath.c_str());
		Pointer::Base(0x149CFFC).Write(TexturesPath.c_str());
		Pointer::Base(0x149D000).Write(TexturesBPath.c_str());
		Pointer::Base(0x149D004).Write(AudioPath.c_str());

		Pointer::Base(0x149D358).Write(FontsPath.c_str());
		Pointer::Base(0x149D35C).Write(FontsPath.c_str());

		//Update the list of maps
		Modules::ModuleGame::Instance().UpdateMapList();
		Modules::ModuleGame::Instance().UpdateCustomMapList();
	}

	void OnGameStart(GameStartCallback callback)
	{
		gameStartCallbacks.push_back(callback);
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
		auto GameStart = reinterpret_cast<GameStartPtr>(0x551590);

		GameStart();

		auto engineGlobalsPtr = ElDorito::GetMainTls(0x48);
		if (!engineGlobalsPtr)
			return;

		auto engineGobals = engineGlobalsPtr[0];

		// fix in-game team switching for engines that support it
		engineGobals(0x8).WriteFast(engineGobals(0x4).Read<uint32_t>());

		for (auto& callback : gameStartCallbacks)
			callback();
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

	void ActiveCamoViewModelClipHook(float *nearPlane, float *farPlane)
	{
		// not a proper fix, but it'll work for now

		const auto view_get_clip_planes = (void(*)(float *nearPlane, float *farPlane))(0x00A25AA0);

		const auto playerIndex = Blam::Players::GetLocalPlayer(0);
		const Blam::Players::PlayerDatum *player;
		if (playerIndex != Blam::DatumHandle::Null && (player = Blam::Players::GetPlayers().Get(playerIndex)))
		{
			auto unitObject = Blam::Objects::Get(player->SlaveUnit);
			float activeCamoPower = 0.0f;

			if (unitObject)
			{
				const auto globalNear = *(float*)0x0191068C;
				const auto globalFar = *(float*)0x01910690;

				if ((activeCamoPower = *(float*)((uint8_t*)unitObject + 0x3F4)) > 0)
				{
					*nearPlane = globalNear * 3.25f;
					*farPlane = globalFar;
					return;
				}

				if (unitObject->Scale < 0.4)
				{
					*nearPlane = globalNear * std::max(2.0f, 10.0f * (unitObject->Scale / 0.4f));
					*farPlane = globalFar;
					return;
				}
			}
		}

		view_get_clip_planes(nearPlane, farPlane);
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
		if (grenadeSetting == 2 || !scenario->PlayerStartingProfile)
		{
			memset(grenadeCounts, 0, 4);
			return;
		}

		// Load the grenade counts from the scenario tag
		auto profile = &scenario->PlayerStartingProfile[0];
		grenadeCounts[0] = profile->StartingFragGrenadeCount;
		grenadeCounts[1] = profile->StartingPlasmaGrenadeCount;
		grenadeCounts[2] = profile->StartingSpikeGrenadeCount;
		grenadeCounts[3] = profile->StartingFirebombGrenadeCount;
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

	double AspectRatioHook()
	{
		int* gameResolution = reinterpret_cast<int*>(0x19106C0);
		return ((double)gameResolution[0] / (double)gameResolution[1]);
	}

	void __fastcall EdgeDropHook(void* thisptr, void* unused, int a2, int a3, int a4, float* a5)
	{
		static auto& modulePlayer = Modules::ModulePlayer::Instance();
		
		Pointer(a3)(0xAC).WriteFast<float>(0.5f);

		static auto sub_724BB0 = (void(__thiscall*)(void* thisptr, int a2, int a3, int a4, float* a5))(0x724BB0);
		sub_724BB0(thisptr, a2, a3, a4, a5);
	}

	void __cdecl BipedFeetZoneOffsetHook(uint32_t bipedObjectIndex, Blam::Math::RealVector3D *position, float *height, float *radius)
	{
		const auto sub_B6E850 = (void(*)(uint32_t unitObjectIndex, Blam::Math::RealVector3D *outPosition, float *a3, float *outRadius))(0xB6E850);
		sub_B6E850(bipedObjectIndex, position, height, radius);
		auto bipedObject = Blam::Objects::Get(bipedObjectIndex);
		if (bipedObject)
			*position += bipedObject->Up * 0.05f; // offset feet
	}

	const auto GetBinkVideoPath = reinterpret_cast<char(*)(int, char*)>(0xA99120);

	char GetBinkVideoPathHook(int p_VideoID, char *p_DestBuf)
	{
		if (Modules::ModuleGame::Instance().VarSkipIntroVideos->ValueInt == 1)
			// Tell the game that there is no video with that ID
			return 0;

		return GetBinkVideoPath(p_VideoID, p_DestBuf);
	}

	void DirtyDiskErrorHookImpl()
	{
		MessageBoxA(NULL, "Dirty Disk Error! Your tags are invalid or corrupted.\nIf you have any mods installed please verify that\nthey are installed in the correct order.\n\nElDewrito will now exit.", "There once was a man from Bungie...", MB_ICONERROR);
		Patches::Core::ExecuteShutdownCallbacks();
		std::exit(0);
	}

	__declspec(naked) void DirtyDiskErrorHook()
	{
		// "There once was a man from Bungie...... Nothing rhymes with Bungie.but he got a dirty disc error for the Alpha or Profile Build and exploded.  The End."
		__asm
		{
			call DirtyDiskErrorHookImpl;
		}
	}

	int __cdecl GetScreenshotFolderHook(wchar_t *path)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;
		std::wstring unprocessed_path = wstring_to_string.from_bytes(Modules::ModuleGame::Instance().VarScreenshotsFolder->ValueString);

		DWORD return_code = ExpandEnvironmentStringsW(unprocessed_path.c_str(), path, MAX_PATH);

		if (return_code == 0 || return_code > MAX_PATH) {// fall back to default
			static auto GetScreenshotsFolder = (int(__cdecl*)(wchar_t *path))(0x724BB0);
			return GetScreenshotsFolder(path);
		}
		boost::filesystem::path dir(path);
		dir = boost::filesystem::weakly_canonical(dir);
		wcsncpy(path, dir.c_str(), 0x100);
		return SHCreateDirectoryExW(NULL, path, NULL);
	}

	void __cdecl HsPrintHook(const char *message)
	{
		if (message == nullptr)
			return;

		Console::WriteLine(message);
	}

	__declspec(naked) void ContrailFixHook()
	{
		__asm
		{
			add ecx, [0x00A58061]
			cmp ecx, -1
			jg render
			push 0xA580BE
			retn
			render:
			push 0xA58067
			retn
		}
	}

	void HillColor(Blam::Math::RealColorARGB &color)
	{
		// if the color luminosity is less than the visible threshold, bump it up
		auto l = 0.2126f*color.Red + 0.7152*color.Green + 0.0722*color.Blue;
		if (l < 0.1f)
		{
			color.Red += 0.1f;
			color.Green += 0.1f;
			color.Blue += 0.1f;
		}
	}

	__declspec(naked) void HillColorHook()
	{
		__asm
		{
			lea edi, [ebp - 0x1C]
			push ebx
			push ecx
			push edi	
			call HillColor
			pop edi
			pop ecx
			pop ebx
			movq xmm0, qword ptr[edi]
			push 0x009D6B21
			retn
		}
	}
}
