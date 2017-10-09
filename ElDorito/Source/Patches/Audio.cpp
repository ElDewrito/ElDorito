#include "../Patch.hpp"
#include "Audio.hpp"
#include "../Modules/ModuleSettings.hpp"

namespace
{
	const auto MAX_CHANNELS = 1024;

	int FmodChannelCountHook();
	int __stdcall FMOD_System_Init_Hook(void* system, int maxchannels, int flags, int extradriverdata, int a5);
	bool __fastcall snd_SYSTEM_FMOD_Init_Hook(uint8_t* thisptr, void* unused, int a2, int a3);
	int sub_661C20_hook(void *sndDef, float pitchModifier, int fallbackIndex);

	const auto s_HaloSoundSystemPtr = (void**)0x018BC9C8;
}

namespace Patches::Audio
{
	void ApplyAll()
	{
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

		// increase fmod max virtual audio channel count
		// http://www.fmod.org/docs/content/generated/FMOD_System_Init.html
		//Hook(0x4E9C, FmodSystemInitHook).Apply();
		//Hook(0x4EC0, FmodSystemInitHook2).Apply();
		Hook(0x25076B, FmodChannelCountHook, HookFlags::IsCall).Apply();

		// increase fmod software channel count
		// http://www.fmod.org/docs/content/generated/FMOD_System_SetSoftwareChannels.html
		*reinterpret_cast<uint32_t*>(0x404DF8 + 1) = MAX_CHANNELS;

		Pointer(0x01750794).Write(uint32_t(&FMOD_System_Init_Hook));
		Pointer(0x0176CA18).Write(uint32_t(&snd_SYSTEM_FMOD_Init_Hook));

		// prevent invalid pitch range index from being returned and causing crashes
		Hook(0x264E39, sub_661C20_hook, HookFlags::IsCall).Apply();
	}

	bool SetOutputDevice(int deviceIndex)
	{
		static auto FMOD_System_SetDriver = (int(__thiscall *)(void *fmodSoundSystem, int driver))(0x139771C);

		auto soundSystem = *s_HaloSoundSystemPtr;
		if (!soundSystem)
			return false;
		auto fmodSystem = *((void**)(soundSystem) + 1);
		if (!fmodSystem)
			return false;

		return FMOD_System_SetDriver(fmodSystem, deviceIndex) == 0;
	}
}

namespace
{
	int FmodChannelCountHook()
	{
		return MAX_CHANNELS;
	}

	int __stdcall FMOD_System_Init_Hook(void* system, int maxchannels, int flags, int extradriverdata, int a5)
	{
		static auto FMOD_System_Init = (int(__stdcall *)(void* system, int maxchannels, int flags, int extradriverdata, int a5))(0x1369AA0);
		return FMOD_System_Init(system, MAX_CHANNELS, flags, extradriverdata, a5);
	}

	bool __fastcall snd_SYSTEM_FMOD_Init_Hook(uint8_t* thisptr, void* unused, int a2, int a3)
	{
		static auto snd_SYSTEM_FMOD_PrepareInit = (bool(*)())(0x00404D10);
		static auto sub_1353A80 = (bool(__stdcall*)(void *a1))(0x01353A80);
		static auto snd_SYSTEM_FMOD_InitEventSystem = (bool(__thiscall*)(void *thisptr, void* a2))(0x00404D70);
		static auto sub_64D9A0 = (bool(__thiscall *)(void* thisptr, int a2, int a3))(0x64D9A0);

		*(uint32_t*)0x0698D054 = GetCurrentThreadId(); // g_SoundThreadId

		if (!snd_SYSTEM_FMOD_PrepareInit() ||
			sub_1353A80((void*)(thisptr + 0x30)) ||
			(*(int(__stdcall **)(uint32_t, void *))(**(uint32_t **)(thisptr + 0x30) + 28))(*(uint32_t *)(thisptr + 0x30), (void*)0x69AD05C) ||
			!snd_SYSTEM_FMOD_InitEventSystem(thisptr, &a2))
		{
			return false;
		}

		auto haloSoundSystem = *s_HaloSoundSystemPtr;
		if (!haloSoundSystem || !sub_64D9A0(haloSoundSystem, a2, 1))
			return false;

		*(uint32_t *)(thisptr + 4) |= 1u;

		auto outputDevice = Modules::ModuleSettings::Instance().VarAudioOutputDevice->ValueInt;
		if (outputDevice)
			Patches::Audio::SetOutputDevice(outputDevice);

		return true;
	}

	int sub_661C20_hook(void *sndDef, float pitchModifier, int fallbackIndex)
	{
		const auto sub_661C20 = (int(*)(void *sndDef, float pitchModifier, int fallbackIndex))(0x661C20);
		return std::max(0, sub_661C20(sndDef, pitchModifier, fallbackIndex));
	}
}
