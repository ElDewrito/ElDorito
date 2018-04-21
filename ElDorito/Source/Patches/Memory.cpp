#include "Memory.hpp"
#include <cstdint>
#include "../Blam/BlamData.hpp"

namespace
{
	size_t globalCachIncrease = 100;
}

namespace Patches::Memory
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
		uint32_t subAllocation4ExtraSize = 0;

		uint32_t oldCachedObjectRenderStatesCount = 384;
		uint32_t newCachedObjectRenderStatesCount = 1000;
		uint32_t cachedObjectRenderStatesSize = 0x4D8;
		*reinterpret_cast<uint32_t*>(0xA452BC + 1) = newCachedObjectRenderStatesCount;
		subAllocation4ExtraSize += Blam::CalculateDatumArraySize(newCachedObjectRenderStatesCount, cachedObjectRenderStatesSize, 0) -
			Blam::CalculateDatumArraySize(oldCachedObjectRenderStatesCount, cachedObjectRenderStatesSize, 0);

		// research by xbox7887
		// expand unknown game state sub-allocations
		auto extraGlobalsSize = 0;
		uint32_t origUnknownSubAllocationSize = 0x4B000;
		uint32_t newUnknownSubAllocationSize = origUnknownSubAllocationSize;
		*reinterpret_cast<uint32_t*>(0x510A6D + 1) = newUnknownSubAllocationSize;
		*reinterpret_cast<uint32_t*>(0x510A80 + 6) = newUnknownSubAllocationSize;
		extraGlobalsSize += newUnknownSubAllocationSize - origUnknownSubAllocationSize;
		uint32_t origUnknownSubAllocation2Size = 0x30000;
		uint32_t newUnknownSubAllocation2Size = origUnknownSubAllocation2Size;
		*reinterpret_cast<uint32_t*>(0x510AD5 + 1) = newUnknownSubAllocation2Size;
		*reinterpret_cast<uint32_t*>(0x510AF1 + 2) = newUnknownSubAllocation2Size;
		extraGlobalsSize += newUnknownSubAllocation2Size - origUnknownSubAllocation2Size;
		uint32_t origUnknownSubAllocation3Size = 0x200000;
		uint32_t newUnknownSubAllocation3Size = origUnknownSubAllocation3Size;
		*reinterpret_cast<uint32_t*>(0x510AEC + 1) = newUnknownSubAllocation3Size;
		*reinterpret_cast<uint32_t*>(0x510B09 + 2) = newUnknownSubAllocation3Size;
		extraGlobalsSize += newUnknownSubAllocation3Size - origUnknownSubAllocation3Size;
		uint32_t origUnknownSubAllocation4Size = 0x2D0000;
		uint32_t newUnknownSubAllocation4Size = origUnknownSubAllocation4Size + (subAllocation4ExtraSize + 0xFFFF & 0xFFFF0000);
		*reinterpret_cast<uint32_t*>(0x510B04 + 1) = newUnknownSubAllocation4Size;
		*reinterpret_cast<uint32_t*>(0x510B21 + 2) = newUnknownSubAllocation4Size;
		extraGlobalsSize += newUnknownSubAllocation4Size - origUnknownSubAllocation4Size;
		uint32_t origUnknownSubAllocation5Size = 0x480000;
		uint32_t newUnknownSubAllocation5Size = origUnknownSubAllocation5Size;
		*reinterpret_cast<uint32_t*>(0x510B1C + 1) = newUnknownSubAllocation5Size;
		*reinterpret_cast<uint32_t*>(0x510B34 + 1) = newUnknownSubAllocation5Size;
		*reinterpret_cast<uint32_t*>(0x510B39 + 2) = newUnknownSubAllocation5Size;
		*reinterpret_cast<uint32_t*>(0x510B4C + 1) = newUnknownSubAllocation5Size;
		*reinterpret_cast<uint32_t*>(0x510B51 + 2) = newUnknownSubAllocation5Size;
		extraGlobalsSize += newUnknownSubAllocation5Size - origUnknownSubAllocation5Size;

		// expand game state globals
		// TODO: .text:00510A15                 push    980000h
		const uint32_t origGameStateGlobalsSize = 0x1280000;
		const uint32_t newGameStateGlobalsSize = origGameStateGlobalsSize + extraGlobalsSize + 0xFFFF & 0xFFFF0000;
		*reinterpret_cast<uint32_t*>(0x50FDC6 + 1) = newGameStateGlobalsSize;
		*reinterpret_cast<uint32_t*>(0x510336 + 1) = newGameStateGlobalsSize;
		*reinterpret_cast<uint32_t*>(0x510A1A + 1) = newGameStateGlobalsSize;
		*reinterpret_cast<uint32_t*>(0x510A2A + 2) = newGameStateGlobalsSize;

		return newGameStateGlobalsSize - origGameStateGlobalsSize;
	}

	void ExpandMainGlobalMemoryMap()
	{
		size_t dataSizeIncrease = 0;
		size_t cacheSizeIncrease = 1024 * 1024 * globalCachIncrease;
		dataSizeIncrease += ExpandGameStateGlobals();
		//dataSizeIncrease += ExpandRuntimeStateGlobals();
		// TODO: other allocations

		size_t newDataSize = origGlobalDataSize + dataSizeIncrease + 0xFFFFF & 0xFFF00000;
		size_t newCacheSize = origGlobalCacheSize + cacheSizeIncrease + 0xFFFFF & 0xFFF00000;
		*reinterpret_cast<uint32_t*>(0x51D6AF + 2) = newDataSize;
		*reinterpret_cast<uint32_t*>(0x51DB64 + 3) = newCacheSize;
		*reinterpret_cast<uint32_t*>(0x51D699 + 1) = newDataSize + newCacheSize;
	}

	void ExpandGlobalRenderGeometry()
	{
		auto oldUnknownGeometryCount = 8192;
		auto oldMaxObjectGeometryCount = 1024;
		auto newMaxObjectGeometryCount = oldMaxObjectGeometryCount * 3;
		auto objectGeometryIncrease = newMaxObjectGeometryCount - oldMaxObjectGeometryCount;
		auto objectGeometrySizeIncrease = objectGeometryIncrease * 0x10;

		// reserve one extra for the new render object geometry count
		*(uint32_t*)(0x00A7923F + 6) = objectGeometryIncrease + 1;
		*(uint32_t*)(0x00A77ADF + 2) = oldUnknownGeometryCount - objectGeometryIncrease - 1; 

		*(uint32_t*)(0x00A79790 + 2) = newMaxObjectGeometryCount;
		*(uint32_t*)(0x00A77CD2 + 2) = newMaxObjectGeometryCount;
		
		*(uint32_t*)(0x00A7977D + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A797BF + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A797C8 + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A79235 + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A78251 + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A78A82 + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A78218 + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A79326 + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A793BD + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A78C13 + 2) = 0x4000 + objectGeometrySizeIncrease;
		// camo
		*(uint32_t*)(0x00A77CC5 + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A77CFD + 2) = 0x4000 + objectGeometrySizeIncrease;
		*(uint32_t*)(0x00A77D06 + 2) = 0x4000 + objectGeometrySizeIncrease;
		
	}

	void SetGlobalCacheIncrease(size_t size)
	{
		// TODO: more mapping and testing is required before this is ready for prime time
		globalCachIncrease = size;
		ExpandMainGlobalMemoryMap();
	}

	void ApplyAll()
	{
		ExpandMainGlobalMemoryMap();
		ExpandGlobalRenderGeometry();
	}
}