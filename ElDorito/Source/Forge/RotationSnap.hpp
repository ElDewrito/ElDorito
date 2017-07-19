#pragma once

#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Math/RealQuaternion.hpp"
#include <cstdint>

namespace Forge
{
	namespace RotationSnap
	{
		void Update(uint32_t playerIndex, uint32_t objectIndex);
		void RotateSnapped(const Blam::Math::RealVector3D& axis);
		void RotateToScripted(const Blam::Math::RealQuaternion& rotation);
	}
}
