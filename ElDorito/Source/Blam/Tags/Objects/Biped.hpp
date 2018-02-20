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
#include "Unit.hpp"

namespace Blam
{
	namespace Tags
	{
		namespace Objects
		{
			using Blam::Math::Angle;
			//using Blam::Math::Bounds;
			//using Blam::Math::RealEulerAngles2D;
			//using Blam::Math::RealPoint3D;
			//using Blam::Math::RealVector2D;
			//using Blam::Math::RealVector3D;
			//using Blam::Tags::Tag;
			//using Blam::Tags::TagBlock;
			//using Blam::Tags::TagData;
			//using Blam::Tags::TagGroup;
			using Blam::Tags::TagReference;
			//using Blam::Text::StringID;
			//using Blam::Tags::Objects::DamageReportingType;
			//using Blam::Tags::Sounds::NoiseLevel;

			struct Biped : TagGroup<'bipd'>
			{
				Unit Unit;

				enum LockOnFlagsValue : int32_t;

				struct WeaponCameraHeightBlock;
				struct UnknownBlock3;
				struct UnknownBlock4;
				struct DeadSphereShape;
				struct PillShape;
				struct SphereShape;
				struct ContactPoint;

				Angle MovingTurningSpeed;
				uint32_t Flags4;
				Angle StationaryTurningSpeed;
				uint32_t Unknown20;
				StringID Unknown21;
				float JumpVelocity;
				float MaximumSoftLandingTime;
				float MinimumHardLandingTime;
				float MinimumSoftLandingVelocity;
				float MinimumHardLandingVelocity;
				float MaximumHardLandingVelocity;
				float DeathHardLandingVelocity;
				float StunDuration;
				float StationaryStandingCameraHeight;
				float MovingStandingCameraHeight;
				float StationaryCrouchingCameraHeight;
				float MovingCrouchingCameraHeight;
				float CrouchTransitionTime;
				TagData<uint8_t> CrouchingCameraFunction;
				TagBlock<WeaponCameraHeightBlock> WeaponCameraHeight;
				Angle CameraInterpolationStart;
				Angle CameraInterpolationEnd;
				uint32_t Unknown22;
				uint32_t Unknown23;
				uint32_t Unknown24;
				uint32_t Unknown25;
				float AutoaimWidth;
				LockOnFlagsValue LockonFlags; //bitfield32
				uint32_t LockonDistance;
				short PhysicsControlNodeIndex;
				short Unknown29;
				uint32_t Unknown30;
				uint32_t Unknown31;
				uint32_t Unknown32;
				short PelvisNodeIndex;
				short HeadNodeIndex;
				uint32_t Unknown33;
				float HeadshotAccelerationScale;
				TagReference AreaDamageEffect;
				TagBlock<UnknownBlock3> Unknown34;
				TagBlock<UnknownBlock4> Unknown35;
				uint32_t Unknown36;
				uint32_t Unknown37;
				uint32_t Unknown38;
				uint32_t Unknown39;
				uint32_t Unknown40;
				uint32_t Unknown41;
				uint32_t Flags5;
				float HeightStanding;
				float HeightCrouching;
				float Radius;
				float Mass;
				StringID LivingMaterialName;
				StringID DeadMaterialName;
				short LivingMaterialGlobalIndex;
				short DeadMaterialGlobalIndex;
				TagBlock<DeadSphereShape> DeadSphereShapes;
				TagBlock<PillShape> PillShapes;
				TagBlock<SphereShape> SphereShapes;
				Angle MaximumSlopeAngle;
				Angle DownhillFalloffAngle;
				Angle DownhillCutoffAngle;
				Angle UphillFalloffAngle;
				Angle UphillCutoffAngle;
				float DownhillVelocityScale;
				float UphillVelocityScale;
				uint32_t Unknown42;
				uint32_t Unknown43;
				uint32_t Unknown44;
				uint32_t Unknown45;
				uint32_t Unknown46;
				uint32_t Unknown47;
				uint32_t Unknown48;
				uint32_t Unknown49;
				uint32_t Unknown50;
				uint32_t Unknown51;
				Angle BankAngle;
				float BankApplyTime;
				float BankDecayTime;
				float PitchRatio;
				float MaximumVelocity;
				float MaximumSidestepVelocity;
				float Acceleration;
				float Deceleration;
				Angle AngularVelocityMaximum;
				Angle AngularAccelerationMaximum;
				float CrouchVelocityModifier;
				TagBlock<ContactPoint> ContactPoints;
				TagReference ReanimationCharacter;
				TagReference TransformationMuffin;
				TagReference DeathSpawnCharacter;
				short DeathSpawnCount;
				short Unknown52;
				Angle Unknown53;
				Angle Unknown54;
				float Unknown55;
				float Unknown56;
				float Unknown57;
				float Unknown58;
				float Unknown59;
				float Unknown60;
				float Unknown61;
				float Unknown62;
				float Unknown63;
				float Unknown64;
				float Unknown65;
				float Unknown66;
				float Unknown67;
				float Unknown68;
				float Unknown69;
				float Unknown70;
				float Unknown71;
				float Unknown72;
				float Unknown73;
				float Unknown74;
				float Unknown75;
				float Unknown76;
				uint32_t Unknown77;

				enum LockOnFlagsValue : int32_t
				{
					None = 0,
					LockedByHumanTargeting = 1 << 0,
					LockedByPlasmaTargeting = 1 << 1,
					AlwaysLockedByHumanTargeting = 1 << 2,
					Bit3 = 1 << 3,
					Bit4 = 1 << 4,
					Bit5 = 1 << 5,
					Bit6 = 1 << 6,
					Bit7 = 1 << 7,
					Bit8 = 1 << 8,
					Bit9 = 1 << 9,
					Bit10 = 1 << 10,
					Bit11 = 1 << 11,
					Bit12 = 1 << 12,
					Bit13 = 1 << 13,
					Bit14 = 1 << 14,
					Bit15 = 1 << 15,
					Bit16 = 1 << 16,
					Bit17 = 1 << 17,
					Bit18 = 1 << 18,
					Bit19 = 1 << 19,
					Bit20 = 1 << 20,
					Bit21 = 1 << 21,
					Bit22 = 1 << 22,
					Bit23 = 1 << 23,
					Bit24 = 1 << 24,
					Bit25 = 1 << 25,
					Bit26 = 1 << 26,
					Bit27 = 1 << 27,
					Bit28 = 1 << 28,
					Bit29 = 1 << 29,
					Bit30 = 1 << 30,
					Bit31 = 1 << 31
				};

				struct WeaponCameraHeightBlock
				{
					StringID Class;
					float StandingHeightFraction;
					float CrouchingHeightFraction;
					uint32_t Unknown;
					uint32_t Unknown2;
					uint32_t Unknown3;
				};

				struct UnknownBlock3
				{
					uint32_t Unknown;
					uint32_t Unknown2;
					uint32_t Unknown3;
					uint32_t Unknown4;
					TagData<uint8_t> Function;
				};

				struct UnknownBlock4
				{
					uint32_t Unknown;
					uint32_t Unknown2;
					uint32_t Unknown3;
					uint32_t Unknown4;
					TagData<uint8_t> Function;
				};

				struct DeadSphereShape
				{
					StringID Name;
					int8_t MaterialIndex;
					int8_t Unknown;
					short GlobalMaterialIndex;
					float RelativeMassScale;
					float Friction;
					float Restitution;
					float Volume;
					float Mass;
					short OverallShapeIndex;
					int8_t PhantomIndex;
					int8_t InteractionUnknown;
					int32_t Unknown2;
					short Size;
					short Count;
					int32_t Offset;
					int32_t Unknown3;
					float Radius;
					uint32_t Unknown4;
					uint32_t Unknown5;
					uint32_t Unknown6;
					int32_t Unknown7;
					short Size2;
					short Count2;
					int32_t Offset2;
					int32_t Unknown8;
					float Radius2;
					uint32_t Unknown9;
					uint32_t Unknown10;
					uint32_t Unknown11;
					Blam::Math::RealVector3D Translation;
					float TranslationRadius;
				};

				struct PillShape
				{
					StringID Name;
					int8_t MaterialIndex;
					int8_t Unknown;
					short GlobalMaterialIndex;
					float RelativeMassScale;
					float Friction;
					float Restitution;
					float Volume;
					float Mass;
					short Index;
					int8_t PhantomIndex;
					int8_t InteractionUnknown;
					int32_t Unknown2;
					short Size;
					short Count;
					int32_t Offset;
					int32_t Unknown3;
					float Radius;
					uint32_t Unknown4;
					uint32_t Unknown5;
					uint32_t Unknown6;
					Blam::Math::RealVector3D Bottom;
					float BottomRadius;
					Blam::Math::RealVector3D Top;
					float TopRadius;
				};

				struct SphereShape
				{
					StringID Name;
					int8_t MaterialIndex;
					int8_t Unknown;
					short GlobalMaterialIndex;
					float RelativeMassScale;
					float Friction;
					float Restitution;
					float Volume;
					float Mass;
					short OverallShapeIndex;
					int8_t PhantomIndex;
					int8_t InteractionUnknown;
					int32_t Unknown2;
					short Size;
					short Count;
					int32_t Offset;
					int32_t Unknown3;
					float Radius;
					uint32_t Unknown4;
					uint32_t Unknown5;
					uint32_t Unknown6;
					int32_t Unknown7;
					short Size2;
					short Count2;
					int32_t Offset2;
					int32_t Unknown8;
					float Radius2;
					uint32_t Unknown9;
					uint32_t Unknown10;
					uint32_t Unknown11;
					Blam::Math::RealVector3D Translation;
					float TranslationRadius;
				};

				struct ContactPoint
				{
					StringID MarkerName;
				};

			};
			TAG_STRUCT_SIZE_ASSERT(Biped, 0x628);
		}
	}
}