#pragma once

#include <string>

namespace Patches
{
	namespace Forge
	{
		void ApplyAll();
		void Tick();

		void DeleteAll();
		void CanvasMap();

		void DeselectAll();
		void SelectAll();

		bool SavePrefab(const std::string& name, const std::string& path);
		bool LoadPrefab(const std::string& path);
	}
}