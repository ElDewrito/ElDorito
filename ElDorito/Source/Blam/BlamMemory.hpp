#pragma once

namespace Blam
{
	struct GlobalMemoryMapStage
	{
		char* DataAddress;
		char* CacheAddress;
		uint32_t IsInitialized;
		uint32_t DataAllocationCount;
		uint32_t CacheAllocationCount;
	};

	struct GlobalMemoryMap
	{
		void** Allocator;
		uint32_t CurrentStageIndex;
		char* DataBaseAddress;
		char* CacheBaseAddress;
		uint32_t IsInitialized;
		GlobalMemoryMapStage Stages[8];
	};
	static_assert(sizeof(GlobalMemoryMap) == 0xB4, "Invalid GlobalMemoryMap size");

	inline GlobalMemoryMap* GetGlobalMemoryMap()
	{
		return reinterpret_cast<GlobalMemoryMap*>(0x238EC50);
	}
}