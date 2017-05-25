#pragma once

#include "BlamData.hpp"

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

		// An object header datum.
		struct ObjectHeader : DatumBase
		{
			uint8_t Flags;
			ObjectType Type;
			uint32_t Unknown8;
			uint32_t PoolOffset; // Offset from the start of the object pool
			void *Data;          // Pointer to the object data in the pool

			// Gets the tag index of the object.
			// Returns DatumIndex::Null if the Data pointer is null.
			DatumIndex GetTagIndex() const
			{
				if (!Data)
					return DatumIndex::Null;

				// HACK: The first value in the object data struct is the tag index.
				return *static_cast<DatumIndex*>(Data);
			}
		};
		static_assert(sizeof(ObjectHeader) == 0x10, "Invalid ObjectHeader size");

		// Gets the global objects data array.
		DataArray<ObjectHeader>& GetObjects();
	}
}