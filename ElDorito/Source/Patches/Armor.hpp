#pragma once
#include <string>
#include <vector>
#include "../Blam/Math/RealVector3D.hpp"

namespace Patches
{
	namespace Armor
	{
		void ApplyAll();
		void RefreshUiPlayer();
		void ApplyAfterTagsLoaded();
		bool CommandPlayerListRenderWeapons(const std::vector<std::string>& Arguments, std::string& returnInfo);
		void UpdateUiPlayerModelArmor();
		void SetUiPlayewrModelTransform(const Blam::Math::RealVector3D* newPosition, const float* rotationAngle);
	}
}
