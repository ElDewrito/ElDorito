#include "PrematchCamera.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Forge/ForgeUtil.hpp"

namespace Forge::PrematchCamera
{
	using namespace Blam::Math;

	uint32_t FindCameraObject()
	{
		if (CAMERA_OBJECT_TAG_INDEX == -1)
			return -1;

		auto objects = Blam::Objects::GetObjects();
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->Data->TagIndex == CAMERA_OBJECT_TAG_INDEX)
				return it.CurrentDatumIndex;
		}

		return -1;
	}

	void PlaceCameraObject()
	{
		const auto unit_get_head_position = (void(*)(int unitObjectIndex, RealVector3D *outPosition))(0x00B439D0);
		const auto object_set_transform = (void(*)(bool a1, uint32_t objectIndex, RealVector3D *position, RealVector3D *forward, RealVector3D *up))(0x0059E340);

		if (CAMERA_OBJECT_TAG_INDEX == -1)
			return;

		Blam::Players::PlayerDatum *player{ nullptr };
		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == Blam::DatumHandle::Null || !(player = Blam::Players::GetPlayers().Get(playerIndex)))
			return;

		auto mapv = Forge::GetMapVariant();
		if (!mapv)
			return;

		Blam::Math::RealMatrix4x3 unitTransform;
		Forge::GetObjectTransformationMatrix(player->SlaveUnit, &unitTransform);

		auto cameraObjectIndex = FindCameraObject();
		if (cameraObjectIndex == -1)
		{
			cameraObjectIndex = Forge::SpawnObject(mapv, CAMERA_OBJECT_TAG_INDEX, -1, -1,
				&unitTransform.Position, &unitTransform.Forward, &unitTransform.Up, -1, -1, nullptr, 0);
		}
		else
		{
			object_set_transform(0, cameraObjectIndex, &unitTransform.Position, &unitTransform.Forward, &unitTransform.Up);
		}
	}
}
