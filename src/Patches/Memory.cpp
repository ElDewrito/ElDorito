#include "Memory.hpp"
#include <cstdint>
#include "../Blam/BlamData.hpp"

namespace Patches
{
	namespace Memory
	{
		const uint32_t origGlobalDataSize = 0xAE00000;
		const uint32_t origGlobalCacheSize = 0x24B00000;

		// TODO: map out everything else in here so we don't have to waste previous padding space
		int ExpandRuntimeStateGlobals()
		{
			const uint32_t origRuntimeGlobalsSize = 0x380000;
			uint32_t extraSize = 0;

			// expand xbox sound
			uint32_t oldXboxSoundCount = 8192;
			uint32_t newXboxSoundCount = oldXboxSoundCount * 2;
			uint32_t xboxSoundSize = 12;
			*reinterpret_cast<uint32_t*>(0x66009A + 1) = newXboxSoundCount;
			extraSize += Blam::CalculateDatumArraySize(newXboxSoundCount, xboxSoundSize, 0) - 
				Blam::CalculateDatumArraySize(oldXboxSoundCount, xboxSoundSize, 0);

			// expand sound sources
			uint32_t oldSoundSourceCount = 384;
			uint32_t newSoundSourceCount = oldSoundSourceCount * 2;
			uint32_t soundSourceSize = 200;
			*reinterpret_cast<uint32_t*>(0x517AEC + 1) = newSoundSourceCount;
			extraSize += Blam::CalculateDatumArraySize(newSoundSourceCount, soundSourceSize, 0) -
				Blam::CalculateDatumArraySize(oldSoundSourceCount, soundSourceSize, 0);

			// sound playback controllers
			uint8_t oldSoundPlaybackControllersCount = 64;
			uint8_t newSoundPlaybackControllersCount = 120;	// NOTE: max of 127 unless we hook it
			uint32_t soundPlaybackControllerSize = 28;
			*reinterpret_cast<uint8_t*>(0x669712 + 1) = newSoundPlaybackControllersCount;
			extraSize += Blam::CalculateDatumArraySize(newSoundPlaybackControllersCount, soundPlaybackControllerSize, 0) -
				Blam::CalculateDatumArraySize(oldSoundPlaybackControllersCount, soundPlaybackControllerSize, 0);

			// expand looping sounds
			uint32_t oldLoopingSoundCount = 128;
			uint32_t newLoopingSoundCount = oldLoopingSoundCount * 2;
			uint32_t loopingSoundSize = 296;
			*reinterpret_cast<uint32_t*>(0x664745 + 1) = newLoopingSoundCount;
			extraSize += Blam::CalculateDatumArraySize(newLoopingSoundCount, loopingSoundSize, 0) -
				Blam::CalculateDatumArraySize(oldLoopingSoundCount, loopingSoundSize, 0);

			// sounds effects
			uint32_t oldSoundEffectsCount = 16;
			uint32_t newSoundEffectsCount = oldSoundEffectsCount * 2;	// NOTE: max of 127 unless we hook it
			uint32_t soundEffectsSize = 100;
			*reinterpret_cast<uint8_t*>(0x6689B2 + 1) = newSoundEffectsCount;
			extraSize += Blam::CalculateDatumArraySize(newSoundEffectsCount, soundEffectsSize, 0) -
				Blam::CalculateDatumArraySize(oldSoundEffectsCount, soundEffectsSize, 0);

			// sound tracker data
			uint32_t oldSoundTrackerDataCount = 384;
			uint32_t newSoundTrackerDataCount = oldSoundTrackerDataCount * 2;
			uint32_t soundTrackerDataSize = 64;
			*reinterpret_cast<uint32_t*>(0x66A312 + 1) = newSoundTrackerDataCount;
			extraSize += Blam::CalculateDatumArraySize(newSoundTrackerDataCount, soundTrackerDataSize, 0) -
				Blam::CalculateDatumArraySize(oldSoundTrackerDataCount, soundTrackerDataSize, 0);

			// expand memory map allocation
			uint32_t newRuntimeGlobalsSize = origRuntimeGlobalsSize + extraSize + 0xFFFF & 0xFFFF0000;
			*reinterpret_cast<uint32_t*>(0x509F30 + 1) = newRuntimeGlobalsSize;
			*reinterpret_cast<uint32_t*>(0x509CD6 + 1) = newRuntimeGlobalsSize;
			*reinterpret_cast<uint32_t*>(0x509D69 + 1) = newRuntimeGlobalsSize;
			*reinterpret_cast<uint32_t*>(0x509E62 + 1) = newRuntimeGlobalsSize;
			*reinterpret_cast<uint32_t*>(0x509EC6 + 1) = newRuntimeGlobalsSize;

			return newRuntimeGlobalsSize - origRuntimeGlobalsSize;
		}

		int ExpandGameStateGlobals()
		{
			// TODO: item expansions


			// TODO: memory map allocation expansion


			// TODO: return total size increase
			return 0;
		}

		void ExpandMainGlobalMemoryMap()
		{
			size_t dataSizeIncrease = 0;
			size_t cacheSizeIncrease = 0;

			dataSizeIncrease += ExpandGameStateGlobals();
			dataSizeIncrease += ExpandRuntimeStateGlobals();
			// TODO: other allocations

			size_t newDataSize = origGlobalDataSize + dataSizeIncrease + 0xFFFFF & 0xFFF00000;
			size_t newCacheSize = origGlobalCacheSize + cacheSizeIncrease + 0xFFFFF & 0xFFF00000;
			*reinterpret_cast<uint32_t*>(0x51D6AF + 2) = newDataSize;
			*reinterpret_cast<uint32_t*>(0x51DB64 + 3) = newCacheSize;
			*reinterpret_cast<uint32_t*>(0x51D699 + 1) = newDataSize + newCacheSize;
		}

		void ApplyAll()
		{
			// TODO: more mapping and testing is required before this is ready for prime time
			//ExpandMainGlobalMemoryMap();
		}
	}
}