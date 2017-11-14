#pragma once
#include "../Tags.hpp"
#include "../../Math/Angle.hpp"
#include "../../Math/Bounds.hpp"
#include "../../Math/RealEulerAngles2D.hpp"
#include "../../Math/RealPoint3D.hpp"
#include "../../Math/RealVector2D.hpp"
#include "../../Math/RealVector3D.hpp"
#include "../../Text/StringID.hpp"
#include "../Objects/Damage.hpp"
#include "../Sounds/Noise.hpp"
#include "Item.hpp"

namespace Blam
{
	namespace Tags
	{
		namespace Items
		{
			using Blam::Math::Angle;
			using Blam::Math::Bounds;
			using Blam::Math::RealEulerAngles2D;
			using Blam::Math::RealPoint3D;
			using Blam::Math::RealVector2D;
			using Blam::Math::RealVector3D;
			using Blam::Tags::Tag;
			using Blam::Tags::TagBlock;
			using Blam::Tags::TagData;
			using Blam::Tags::TagGroup;
			using Blam::Tags::TagReference;
			using Blam::Text::StringID;
			using Blam::Tags::Objects::DamageReportingType;
			using Blam::Tags::Sounds::NoiseLevel;

			struct Equipment : TagGroup<'eqip'>
			{
				Item Item;

				//Tagblocks
				struct EquipmentCameraBlock;
				struct CameraTrack;
				struct UnknownBlock;
				struct HealthPackBlock;
				struct PowerupBlock;
				struct ObjectCreationBlock;
				struct DestructionBlock;
				struct RadarManipulationBlock;
				struct InvisibilityBlock;
				struct InvincibilityBlock;
				struct RegeneratorBlock;
				struct ForcedReloadBlock;
				struct ConcussiveBlastBlock;
				struct TankModeBlock;
				struct MagPulseBlock;
				struct HologramBlock;
				struct ReactiveArmorBlock;
				struct BombRunBlock;
				struct ArmorLockBlock;
				struct AdrenalineBlock;
				struct LightningStrikeBlock;
				struct ScramblerBlock;
				struct WeaponJammerBlock;
				struct AmmoPackBlock;
				struct VisionBlock;
				struct WeaponBlock;

				float useDuration;
				unsigned int Unknown8;
				short NumberOfUses;
				unsigned short Flags3;
				unsigned int Unknown9;
				unsigned int Unknown10;
				unsigned int Unknown11;
				TagBlock<EquipmentCameraBlock> EquipmentCamera;
				TagBlock<HealthPackBlock> HealthPack;
				TagBlock<PowerupBlock> Powerup;
				TagBlock<ObjectCreationBlock> ObjectCreation;
				TagBlock<DestructionBlock> Destruction;
				TagBlock<RadarManipulationBlock> RadarManipulation;
				unsigned int Unknown12;
				unsigned int Unknown13;
				unsigned int Unknown14;
				TagBlock<InvisibilityBlock> Invisibility;
				TagBlock<InvincibilityBlock> Invincibility;
				TagBlock<RegeneratorBlock> Regenerator;
				unsigned int Unknown15;
				unsigned int Unknown16;
				unsigned int  Unknown17;
				TagBlock<ForcedReloadBlock> ForcedReload;
				TagBlock<ConcussiveBlastBlock> ConcussiveBlast;
				TagBlock<TankModeBlock> TankMode;
				TagBlock<MagPulseBlock> MagPulse;
				TagBlock<HologramBlock> Hologram;
				TagBlock<ReactiveArmorBlock> ReactiveArmor;
				TagBlock<BombRunBlock> BombRun;
				TagBlock<ArmorLockBlock> ArmorLock;
				TagBlock<AdrenalineBlock> Adrenaline;
				TagBlock<LightningStrikeBlock> LightningStrike;
				TagBlock<ScramblerBlock> Scrambler;
				TagBlock<WeaponJammerBlock> WeaponJammer;
				TagBlock<AmmoPackBlock> AmmoPack;
				TagBlock<VisionBlock> Vision;
				TagReference HudInterface;
				TagReference PickupSound;
				TagReference EmptySound;
				TagReference ActivationEffect;
				TagReference ActiveEffect;
				TagReference DeactivationEffect;
				StringID EnterAnimation;
				StringID IdleAnimation;
				StringID ExitAnimation;

				struct EquipmentCameraBlock
				{
					short Flags;
					short Unknown;
					StringID CameraMarkerName;
					StringID CameraSubmergedMarkerName;
					float PitchAutoLevel;
					float PitchRangeMin;
					float PitchRangeMax;
					TagBlock<CameraTrack> CameraTracks;
					float Unknown2;
					float Unknown3;
					float Unknown4;
					TagBlock<UnknownBlock> Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(EquipmentCameraBlock, 0x3C);

				struct CameraTrack
				{
					TagReference Track;
				};
				TAG_STRUCT_SIZE_ASSERT(CameraTrack, 0x10);

				struct UnknownBlock
				{
					unsigned int Unknown;
					unsigned int Unknown2;
					unsigned int Unknown3;
					unsigned int Unknown4;
					unsigned int Unknown5;
					unsigned int Unknown6;
					unsigned int Unknown7;
					unsigned int Unknown8;
					unsigned int Unknown9;
					unsigned int Unknown10;
					unsigned int Unknown11;
					unsigned int Unknown12;
					unsigned int Unknown13;
					unsigned int Unknown14;
					unsigned int Unknown15;
					unsigned int Unknown16;
					unsigned int Unknown17;
					unsigned int Unknown18;
					unsigned int Unknown19;
				};
				TAG_STRUCT_SIZE_ASSERT(UnknownBlock, 0x4C);

				struct HealthPackBlock
				{
					unsigned int Unknown;
					unsigned int Unknown2;
					float ShieldsGiven;
					TagReference Unknown3;
					TagReference Unknown4;
					TagReference Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(HealthPackBlock, 0x3C);

				enum PowerupTraitSetValue : int
				{
					Red,
					Blue,
					Yellow,
				};

				struct PowerupBlock
				{
					PowerupTraitSetValue PowerupTraitSet;
				};
				TAG_STRUCT_SIZE_ASSERT(PowerupBlock, 0x4);

				struct ObjectCreationBlock
				{
					TagReference Object;
					TagReference Unknown;
					unsigned int Unknown2;
					unsigned int Unknown3;
					unsigned int Unknown4;
					float ObjectForce;
					unsigned int Unknown5;
				};
				TAG_STRUCT_SIZE_ASSERT(ObjectCreationBlock, 0x34);

				struct DestructionBlock
				{
					TagReference DestroyEffect;
					TagReference DestroyDamageEffect;
					unsigned int Unknown;
					float SelfDestructionTime;
					unsigned int Unknown2;
					unsigned int Unknown3;
				};
				TAG_STRUCT_SIZE_ASSERT(DestructionBlock, 0x30);

				struct RadarManipulationBlock
				{
					unsigned int Unknown;
					float FakeBlipRadius;
					int FakeBlipCount;
					unsigned int Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(RadarManipulationBlock, 0x10);

				struct InvisibilityBlock
				{
					unsigned int Unknown;
					unsigned int Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(InvisibilityBlock, 0x8);

				struct InvincibilityBlock
				{
					StringID NewPlayerMaterial;
					short NewPlayerMaterialGlobalIndex;
					short Unknown;
					unsigned int Unknown2;
					TagReference Unknown3;
					TagReference Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(InvincibilityBlock, 0x2C);

				struct RegeneratorBlock
				{
					TagReference RegeneratingEffect;
				};
				TAG_STRUCT_SIZE_ASSERT(RegeneratorBlock, 0x10);

				struct ForcedReloadBlock
				{
					TagReference Effect;
					unsigned int Unknown;
				};
				TAG_STRUCT_SIZE_ASSERT(ForcedReloadBlock, 0x14);

				struct ConcussiveBlastBlock
				{
					TagReference Unknown;
					TagReference Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(ConcussiveBlastBlock, 0x20);


				struct TankModeBlock
				{
					StringID NewPlayerMaterial;
					unsigned int Unknown;
					unsigned int Unknown2;
					unsigned int Unknown3;
					unsigned int Unknown4;
					unsigned int Unknown5;
					TagReference ActiveHud;
				};
				TAG_STRUCT_SIZE_ASSERT(TankModeBlock, 0x28);

				struct MagPulseBlock
				{
					TagReference Unknown;
					TagReference Unknown2;
					TagReference Unknown3;
					unsigned int Unknown4;
				};
				TAG_STRUCT_SIZE_ASSERT(MagPulseBlock, 0x34);

				struct HologramBlock
				{
					unsigned int Unknown;
					TagReference ActiveEffect;
					TagReference Unknown2;
					unsigned int Unknown3;
					unsigned int Unknown4;
					unsigned int Unknown5;
					TagReference DeathEffect;
					unsigned int Unknown6;
					unsigned int Unknown7;
					uint8_t Function[18];
					TagReference NavPointHud;
				};
				TAG_STRUCT_SIZE_ASSERT(HologramBlock, 0x6C);

				struct ReactiveArmorBlock
				{
					unsigned int Unknown;
					unsigned int Unknown2;
					unsigned int Unknown3;
					TagReference Unknown4;
					TagReference Unknown5;
					TagReference Unknown6;
					TagReference Unknown7;
				};
				TAG_STRUCT_SIZE_ASSERT(ReactiveArmorBlock, 0x4C);

				struct BombRunBlock
				{
					int Unknown;
					unsigned int Unknown2;
					unsigned int Unknown3;
					unsigned int Unknown4;
					unsigned int Unknown5;
					TagReference Projectile;
					TagReference ThrowSound;
				};
				TAG_STRUCT_SIZE_ASSERT(BombRunBlock, 0x34);

				struct ArmorLockBlock
				{
					TagReference Unknown;
					TagReference Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(ArmorLockBlock, 0x20);

				struct AdrenalineBlock
				{
					unsigned int Unknown;
					TagReference Unknown2;
					TagReference Unknown3;
				};
				TAG_STRUCT_SIZE_ASSERT(AdrenalineBlock, 0x24);

				struct LightningStrikeBlock
				{
					unsigned int Unknown;
					TagReference Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(LightningStrikeBlock, 0x14);

				struct ScramblerBlock
				{
					unsigned int Unknown;
					TagReference Unknown2;
					int Unknown3;
					int Unknown4;
					int Unknown5;
					int Unknown6;
				};
				TAG_STRUCT_SIZE_ASSERT(ScramblerBlock, 0x24);

				struct WeaponJammerBlock
				{
					unsigned int Unknown;
					TagReference Unknown2;
					int Unknown3;
					int Unknown4;
					int Unknown5;
					int Unknown6;
				};
				TAG_STRUCT_SIZE_ASSERT(WeaponJammerBlock, 0x24);

				struct WeaponBlock
				{
					StringID Name;
					TagReference WeaponObject;
					int Unknown;
				};
				TAG_STRUCT_SIZE_ASSERT(WeaponBlock, 0x18);

				struct AmmoPackBlock
				{
					unsigned int Unknown;
					int Unknown2;
					int Unknown3;
					unsigned int Unknown4;
					int Unknown5;
					int Unknown6;
					TagBlock<WeaponBlock> Weapons;
					TagReference Unknown7;
				};
				TAG_STRUCT_SIZE_ASSERT(AmmoPackBlock, 0x34);

				struct VisionBlock
				{
					TagReference ScreenEffect;
					TagReference Unknown;
				};
				TAG_STRUCT_SIZE_ASSERT(VisionBlock, 0x20);
			};
			TAG_STRUCT_SIZE_ASSERT(Equipment, 0x384);
		}
	}
}