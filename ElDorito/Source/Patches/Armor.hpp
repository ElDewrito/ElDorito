#pragma once
#include <string>
#include <vector>

namespace Patches
{
	namespace Armor
	{
		void ApplyAll();
		void RefreshUiPlayer();
		void ApplyAfterTagsLoaded();
		bool CommandPlayerListRenderWeapons(const std::vector<std::string>& Arguments, std::string& returnInfo);
	}
}
