#pragma once

#include <string>

namespace Patches::Forge
{
	void ApplyAll();
	void Tick();

	using ItemSpawnedCallback = void(*)(uint32_t objectIndex);
	void OnItemSpawned(ItemSpawnedCallback callback);
}
