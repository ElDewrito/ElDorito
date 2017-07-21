#pragma once

#include <string>

namespace Patches::Forge
{
	void ApplyAll();
	void Tick();

	bool SavePrefab(const std::string& name, const std::string& path);
	bool LoadPrefab(const std::string& path);
}
