#pragma once

#include <string>

namespace Patches
{
	namespace Forge
	{
		void ApplyAll();
		void Tick();

		bool SavePrefab(const std::string& name, const std::string& path);
		bool LoadPrefab(const std::string& path);
	}
}