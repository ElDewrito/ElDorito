#pragma once

#include "../Blam/Math/RealVector3D.hpp"
#include <cstdint>

namespace Forge
{
	namespace RotationSnap
	{
		void Update(uint32_t playerIndex, uint32_t objectIndex);
		void RotateSnapped(const Blam::Math::RealVector3D& axis);
	}
}
