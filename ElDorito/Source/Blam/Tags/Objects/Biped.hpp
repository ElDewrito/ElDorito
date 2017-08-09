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

			struct Biped : Unit, TagGroup<'bipd'>
			{
				enum LockOnFlagsValue : int32_t
				{
					None,
					LockedByHumanTargeting,
					LockedByPlasmaTargeting,
					AlwaysLockedByHumanTargeting = 4,
					Bit3 = 8,
					Bit4 = 16,
					Bit5 = 32,
					Bit6 = 64,
					Bit7 = 128,
					Bit8 = 256,
					Bit9 = 512,
					Bit10 = 1024,
					Bit11 = 2048,
					Bit12 = 4096,
					Bit13 = 8192,
					Bit14 = 16384,
					Bit15 = 32768,
					Bit16 = 65536,
					Bit17 = 131072,
					Bit18 = 262144,
					Bit19 = 524288,
					Bit20 = 1048576,
					Bit21 = 2097152,
					Bit22 = 4194304,
					Bit23 = 8388608,
					Bit24 = 16777216,
					Bit25 = 33554432,
					Bit26 = 67108864,
					Bit27 = 134217728,
					Bit28 = 268435456,
					Bit29 = 536870912,
					Bit30 = 1073741824,
					Bit31 = -2147483648,
				};

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
					float TranslationI;
					float TranslationJ;
					float TranslationK;
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
					float BottomI;
					float BottomJ;
					float BottomK;
					float BottomRadius;
					float TopI;
					float TopJ;
					float TopK;
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
					float TranslationI;
					float TranslationJ;
					float TranslationK;
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