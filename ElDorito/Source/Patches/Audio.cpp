#include "../Patch.hpp"
#include "Audio.hpp"

namespace
{
	void FmodSystemInitHook();
	void FmodSystemInitHook2();
	int FmodChannelCountHook();
}

namespace Patches
{
	namespace Audio
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
			Hook(0x4E9C, FmodSystemInitHook).Apply();
			Hook(0x4EC0, FmodSystemInitHook2).Apply();
			Hook(0x25076B, FmodChannelCountHook, HookFlags::IsCall).Apply();

			// increase fmod software channel count
			// http://www.fmod.org/docs/content/generated/FMOD_System_SetSoftwareChannels.html
			*reinterpret_cast<uint32_t*>(0x404DF8 + 1) = FmodChannelCountHook();
		}
	}
}

namespace
{
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

	int FmodChannelCountHook()
	{
		return 256;
	}
}