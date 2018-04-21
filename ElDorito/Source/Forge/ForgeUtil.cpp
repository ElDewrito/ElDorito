#include "ForgeUtil.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Math/RealMatrix4x3.hpp"
#include "../Pointer.hpp"
#include "../ElDorito.hpp"

using namespace Blam;
using namespace Blam::Math;

namespace Forge
{
	Blam::MapVariant* GetMapVariant()
	{
		static auto GetMapVariant = (Blam::MapVariant* (__cdecl*)())(0x00583230);
		return GetMapVariant();
	}

	const AABB* GetObjectBoundingBox(uint32_t tagIndex)
	{
		using namespace Blam::Tags;
		auto objectDefPtr = Pointer(TagInstance(tagIndex).GetDefinition<void>());
		if (!objectDefPtr)
			return false;

		auto hlmtTagIndex = objectDefPtr(0x40).Read<uint32_t>();

		auto hlmtDefPtr = Pointer(TagInstance(hlmtTagIndex).GetDefinition<void>());
		if (!hlmtDefPtr)
			return false;

		auto modeTagIndex = hlmtDefPtr(0xC).Read<uint32_t>();

		auto modeDefPtr = Pointer(TagInstance(modeTagIndex).GetDefinition<void>());
		if (!modeDefPtr)
			return false;

		auto compressionInfoBlock = modeDefPtr(0x74).Read<Blam::Tags::TagBlock<uint8_t>>();
		if (compressionInfoBlock.Count < 1)
			return false;

		return (AABB*)Pointer(compressionInfoBlock.Elements)(0x4);
	}

	void DeleteObject(uint16_t playerIndex, int16_t placementIndex)
	{
		static auto Forge_DeleteObject = (void(*) (__int16 playerIndex, int placementIndex))(0x0059CBE0);
		Forge_DeleteObject(playerIndex, placementIndex);
	}

	uint32_t CloneObject(uint32_t playerIndex, uint32_t objectIndex, float depth, const RealVector3D &normal)
	{
		auto mapv = GetMapVariant();

		auto object = Blam::Objects::Get(objectIndex);

		if (!mapv || playerIndex == -1 || objectIndex == -1 || !object)
			return -1;

		auto boundsPtr = GetObjectBoundingBox(object->TagIndex);
		if (!boundsPtr)
			return -1;
		const auto& bounds = *boundsPtr;

		const auto& variantProperties = mapv->Placements[object->PlacementIndex].Properties;

		RealMatrix4x3 objectTransform;
		GetObjectTransformationMatrix(objectIndex, &objectTransform);

		RealVector3D intersectNormal;
		static auto Matrix4x3_TransformVec = (void(__cdecl*)(RealMatrix4x3* matrix, const RealVector3D *vec, RealVector3D *outVector))(0x005B2710);
		Matrix4x3_TransformVec(&objectTransform, &normal, &intersectNormal);

		RealVector3D displacement(0, 0, 0);
		if (std::abs(intersectNormal.K) > std::abs(intersectNormal.J) && std::abs(intersectNormal.K) > std::abs(intersectNormal.I))
		{
			auto dz = (bounds.MaxZ - bounds.MinZ);
			displacement = intersectNormal.K > 0 ? objectTransform.Up * dz : objectTransform.Up * -dz;
		}
		else if (std::abs(intersectNormal.J) > std::abs(intersectNormal.K) && std::abs(intersectNormal.J) > std::abs(intersectNormal.I))
		{
			auto dy = (bounds.MaxY - bounds.MinY);
			displacement = intersectNormal.J > 0 ? objectTransform.Left * dy : objectTransform.Left * -dy;
		}
		else if (std::abs(intersectNormal.I) > std::abs(intersectNormal.J) && std::abs(intersectNormal.I) > std::abs(intersectNormal.K))
		{
			auto dx = (bounds.MaxX - bounds.MinX);
			displacement = intersectNormal.I > 0 ? objectTransform.Forward * dx : objectTransform.Forward * -dx;
		}

		auto displacedPosition = objectTransform.Position + displacement * depth;

		if (!((displacedPosition.I > mapv->WorldBoundsXMin && displacedPosition.I < mapv->WorldBoundsXMax) &&
			(displacedPosition.J > mapv->WorldBoundsYMin && displacedPosition.J < mapv->WorldBoundsYMax) &&
			(displacedPosition.K > mapv->WorldBoundsZMin && displacedPosition.K < mapv->WorldBoundsZMax)))
		{
			PrintKillFeedText(0, L"Object out of World Bounds", 0);
			return -1;
		}

		return SpawnObject(GetMapVariant(), object->TagIndex, 0, -1, &displacedPosition, &objectTransform.Forward,
			&objectTransform.Up, -1, -1, &variantProperties, 0);
	}

	void ThrowObject(uint32_t playerIndex, uint32_t objectIndex, float throwForce)
	{
		static auto Object_UpdateTransform = (void(__cdecl *)(bool a1, int objectIndex))(0x0059E340);
		static auto Object_GetVelocity = (void(__cdecl *)(uint32_t objectIndex, RealVector3D* a2, RealVector3D *a3))(0x00B2EB30);
		static auto Object_SetVelocity = (void(__cdecl *)(uint32_t objectIndex, RealVector3D* a2, RealVector3D *a3))(0x00B34040);
		static auto Object_Wake = (void(__cdecl *)(uint32_t objectIndex, bool a2))(0x00B327F0);
		static auto Unit_GetHeadPosition = (void(__cdecl*)(uint32_t unitObjectIndex, RealVector3D *position))(0x00B439D0);
		static auto sub_B34040 = (void(__cdecl*)(uint32_t objectIndex, RealVector3D* v1, RealVector3D* v2))(0xB34040);
		static auto sub_580AD0 = (void(__cdecl*)(uint32_t unitObjectIndex, int a2))(0x580AD0);
		static auto sub_59EA10 = (void(__cdecl*)(uint32_t objectIndex))(0x59EA10);

		RealMatrix4x3 objectTransform;
		GetObjectTransformationMatrix(objectIndex, &objectTransform);

		auto& players = Blam::Players::GetPlayers();
		auto& objects = Blam::Objects::GetObjects();

		auto player = players.Get(playerIndex);
		if (!player)
			return;

		Object_Wake(objectIndex, 0);

		if (throwForce > 0.000099999997f)
		{
			auto unitObjectPtr = Blam::Objects::Get(player->SlaveUnit);
			if (!unitObjectPtr)
				return;

			RealVector3D unitHeadPos;
			Unit_GetHeadPosition(player->SlaveUnit, &unitHeadPos);

			auto vec = objectTransform.Position - unitHeadPos;
			RealVector3D::Normalize(vec);

			vec.I = throwForce * 15.0f * vec.I;
			vec.J = throwForce * 15.0f * vec.J;
			vec.K = throwForce * 15.0f * vec.K;

			RealVector3D v2, v3;
			Object_GetVelocity(objectIndex, &v2, &v3);
			sub_B34040(objectIndex, &v2, &v3);
			sub_580AD0(objectIndex, 4);

		}
		sub_59EA10(objectIndex);
	}


	RealVector3D GetClosestCardianalAxix(const RealVector3D& v)
	{
		auto xn = std::abs(v.I), yn = std::abs(v.J), zn = std::abs(v.K);

		if (xn >= yn && xn >= zn)
			return v.I > 0 ? RealVector3D(1, 0, 0) : RealVector3D(-1, 0, 0);
		else if (yn > xn && yn > zn)
			return v.J > 0 ? RealVector3D(0, 1, 0) : RealVector3D(0, -1, 0);
		else if (zn > xn && zn > yn)
			return v.K > 0 ? RealVector3D(0, 0, 1) : RealVector3D(0, 0, -1);

		return RealVector3D(0, 0, 0);
	}

	ForgeObjectQuota CalculateObjectQuota()
	{
		uint16_t totalAvailable = 0;
		uint16_t totalUsed = 0;

		auto mapv = GetMapVariant();
		if (mapv)
		{
			for (auto i = mapv->ScnrPlacementsCount; i < 640; i++)
			{
				auto &placement = mapv->Placements[i];

				totalAvailable++;
				if (placement.PlacementFlags & 1)
				{
					totalUsed++;
				}
			}
		}

		return { totalAvailable, totalUsed };
	}

	bool PointInWorldBounds(Blam::Math::RealVector3D &point)
	{
		auto mapv = GetMapVariant();
		return point.I >= mapv->WorldBoundsXMin && point.I <= mapv->WorldBoundsXMax
			&& point.J >= mapv->WorldBoundsYMin && point.J <= mapv->WorldBoundsYMax
			&& point.K >= mapv->WorldBoundsZMin && point.K <= mapv->WorldBoundsZMax;
	}

	bool ObjectInWorldBounds(uint32_t objectIndex)
	{
		Blam::Math::RealVector3D worldPos;
		Forge::GetObjectPosition(objectIndex, &worldPos);
		return PointInWorldBounds(worldPos);
	}
}
