#include "../Patch.hpp"
#include "Audio.hpp"
#include "../Modules/ModuleSettings.hpp"

namespace
{
	const auto MAX_CHANNELS = 1024;

	int FmodChannelCountHook();
	int __stdcall FMOD_System_Init_Hook(void* system, int maxchannels, int flags, int extradriverdata, int a5);
	int __fastcall snd_SYSTEM_FMOD_Init_Hook(int thisptr, void* unusded, int a2, int a3);
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
	}

	bool SetOutputDevice(int deviceIndex)
	{
		static auto FMOD_System_SetDriver = (int(__thiscall *)(void *fmodSoundSystem, int driver))(0x139771C);

		auto haloSoundSystemPtr = (void**)0x018BC9C8;
		if (!haloSoundSystemPtr)
			return false;
		auto fmodSystem = *((void**)(*haloSoundSystemPtr) + 1);
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

	int __fastcall snd_SYSTEM_FMOD_Init_Hook(int thisptr, void* unused, int a2, int a3)
	{
		static auto snd_SYSTEM_FMOD_Init = (int(__thiscall *)(int thisptr, int a2, int a3))(0x004047B0);
		auto result = snd_SYSTEM_FMOD_Init(thisptr, a2, a3);

		auto outputDevice = Modules::ModuleSettings::Instance().VarAudioOutputDevice->ValueInt;
		if (result && outputDevice)
			Patches::Audio::SetOutputDevice(outputDevice);

		return result;
	}
}
