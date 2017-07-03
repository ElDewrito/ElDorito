#pragma once

#include "BlamData.hpp"
#include "../Blam/Math/RealVector3D.hpp"

namespace Blam
{
	namespace Objects
	{
		enum ObjectType : int8_t
		{
			eObjectTypeNone = -1,
			eObjectTypeBiped,
			eObjectTypeVehicle,
			eObjectTypeWeapon,
			// [Newer builds have armor here]
			eObjectTypeEquipment,
			eObjectTypeArgDevice,
			eObjectTypeTerminal,
			eObjectTypeProjectile,
			eObjectTypeScenery,
			eObjectTypeMachine,
			eObjectTypeControl,
			eObjectTypeSoundScenery,
			eObjectTypeCrate,
			eObjectTypeCreature,
			eObjectTypeGiant,
			eObjectTypeEffectScenery
		};

		struct ObjectBase
		{
			uint32_t TagIndex;
			uint32_t Flags;
			uint32_t Unknown08;
			DatumIndex NextSibling;
			DatumIndex FirstChild;
			DatumIndex Parent;
			int16_t ParentMatrixIndex;
			int16_t ScnrPlacementBlockIndex;
			int16_t PlacementIndex;
			int16_t ClusterIndex;
			Blam::Math::RealVector3D Center;
			float Radius;
			uint32_t Unknown30;
			uint32_t Unknown34;
			uint32_t Unknown38;
			uint32_t Unknown3C;
			uint32_t Unknown40;
			uint32_t Unknown44;
			uint32_t Unknown48;
			uint32_t Unknown4C;
			uint32_t Unknown50;
			Blam::Math::RealVector3D Position;
			Blam::Math::RealVector3D Forward;
			Blam::Math::RealVector3D Up;
			Blam::Math::RealVector3D Velocity;
			Blam::Math::RealVector3D Velocity2;
			float Scale;
			uint32_t Unknown94;
			uint32_t Unknown98;
			ObjectType ObjectType;
			uint8_t Unknown9B;
			uint16_t ObjectNameIndex;
			uint16_t Unknown9E;
			uint16_t HavokComponent;
			uint32_t UnknownA4;
			uint32_t UnknownA8;
			uint32_t HavokFlags;
			uint32_t UnknownB0;
			uint32_t UnknownB4;
			uint32_t UnknownB8;
			uint32_t UnknownBC;
			uint32_t UnknownC0;
			uint32_t UnknownC4;
			uint32_t UnknownC8;
			uint32_t UnknownCC;
			DatumIndex SimulationEntity;
			// ...

			void* GetMultiplayerProperties() const
			{
				auto p = reinterpret_cast<const uint8_t*>(this);
				auto offset = *reinterpret_cast<const int16_t*>(p + 0x16c);
				if (offset == -1)
					return nullptr;
				return const_cast<uint8_t*>(p) + offset;
			}
		};

		// An object header datum.
		struct ObjectHeader : DatumBase
		{
			uint8_t Flags;
			ObjectType Type;
			uint32_t Unknown8;
			uint32_t PoolOffset; // Offset from the start of the object pool
			ObjectBase *Data;          // Pointer to the object data in the pool

			// Gets the tag index of the object.
			// Returns DatumIndex::Null if the Data pointer is null.
			DatumIndex GetTagIndex() const
			{
				if (!Data)
					return DatumIndex::Null;

				return Data->TagIndex;
			}
		};

		static_assert(sizeof(ObjectHeader) == 0x10, "Invalid ObjectHeader size");


		// Gets the global objects data array.
		DataArray<ObjectHeader>& GetObjects();

		template <typename T = ObjectBase>
		T* Get(uint32_t objectIndex)
		{
			auto objectHeader = GetObjects().Get(objectIndex);
			if (objectHeader)
				return static_cast<T*>(objectHeader->Data);
			return nullptr;
		}
	}
}