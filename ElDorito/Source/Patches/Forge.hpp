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
		void ForgeVariant_Set_Name(const std::wstring name);
		void ForgeVariant_Set_Description(const std::string description);
	}
}