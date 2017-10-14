#pragma once

#include <string>

namespace Patches::Forge
{
	void ApplyAll();
	void Tick();

	bool SavePrefab(const std::string& name, const std::string& path);
	bool LoadPrefab(const std::string& path);

	void SetPrematchCamera();

	void SpawnItem(uint32_t tagIndex);
	using ItemSpawnedCallback = void(*)(uint32_t objectIndex);
	void OnItemSpawned(ItemSpawnedCallback callback);
}
