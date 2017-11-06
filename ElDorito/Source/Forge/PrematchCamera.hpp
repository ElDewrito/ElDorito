#pragma once

#include <cstdint>

namespace Forge::PrematchCamera
{
	const auto CAMERA_OBJECT_TAG_INDEX = 0x00005A86;

	uint32_t FindCameraObject();
	void PlaceCameraObject();
}
