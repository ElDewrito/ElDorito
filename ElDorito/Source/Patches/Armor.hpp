#pragma once
#include <string>
#include <vector>
#include "../Blam/Math/RealVector3D.hpp"

namespace Patches::Armor
{
	void ApplyAll();
	void RefreshUiPlayer();
	void ApplyAfterTagsLoaded();
	void UpdateUiPlayerModelArmor();
	void SetUiPlayerModelTransform(const Blam::Math::RealVector3D* newPosition, const float* rotationAngle);
}