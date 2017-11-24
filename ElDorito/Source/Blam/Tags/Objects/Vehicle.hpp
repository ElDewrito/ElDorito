#pragma once
#include "../../Math/RealPoint3D.hpp"
#include "../../Text/StringID.hpp"
#include "../Tags.hpp"
#include "Unit.hpp"

namespace Blam::Tags::Objects
{
	struct Vehicle : TagGroup<'vehi'>
	{
		Unit Unit;

		struct Unknown10;
		struct Unknown16;
		struct TankEngineMotionProperty;
		struct EngineMotionProperty;
		struct DropshipMotionProperty;
		struct AntigravityMotionProperty;
		struct JetEngineMotionProperty;
		struct TurretProperty;
		struct HelicopterMotionProperty;
		struct AntigravityEngineMotionProperty;
		struct AutoturretEquipment;
		struct AntiGravityPoint;
		struct FrictionPoint;
		struct PhantomShape;

		Object::TypeValue ObjectType : 16;
		Object::FlagsValue ObjectFlags : 16;
		float BoundingRadius;
		RealPoint3D BoundingOffset;
		float AccelerationScale;
		Object::LightmapShadowModeValue LightmapShadowMode : 16;
		Object::SweetenerSizeValue SweetenerSize : 8;
		Object::WaterDensityValue WaterDensity : 8;
		int32_t Unknown;
		StringID DynamicLightSphereRadius;
		RealPoint3D DynamicLightSphereOffset;
		Text::StringID DefaultModelVariant;
		TagReference Model;
		TagReference CrateObject;
		TagReference CollisionDamage;
		TagBlock<Object::EarlyMoverProperty> EarlyMoverProperties;
		TagReference CreationEffect;
		TagReference MaterialEffects;
		TagReference ArmorSounds;
		TagReference MeleeImpact;
		TagBlock<Object::AIProperty> AiProperties;
		TagBlock<Object::Function> Functions;
		int16_t HudTextMessageIndex;
		int16_t Unknown2;
		TagBlock<Object::Attachment> Attachments;
		TagBlock<Object::Widget> Widgets;
		TagBlock<Object::ChangeColor> ChangeColors;
		TagBlock<Object::NodeMap> NodeMaps;
		TagBlock<Object::MultiplayerProperty> MultiplayerProperties;
		uint32_t Unknown3;
		uint32_t Unknown4;
		uint32_t Unknown5;
		TagBlock<Object::ModelObjectDatum> ModelObjectData;
		int32_t Flags2;
		int16_t DefaultTeam;
		int16_t ConstantSoundVolume;
		TagReference HologramUnit;
		TagBlock<Unit::MetagameProperty> MetagameProperties;
		TagReference IntegratedLightToggle;
		float CameraFieldOfView;
		float CameraStiffness;
		int16_t Flags3;
		int16_t Unknown6;
		Text::StringID CameraMarkerName;
		Text::StringID CameraSubmergedMarkerName;
		float PitchAutoLevel;
		float PitchRangeMin;
		float PitchRangeMax;
		TagBlock<Unit::CameraTrackBlock> CameraTracks;
		float Unknown7;
		float Unknown8;
		float Unknown9;
		TagBlock<Vehicle::Unknown10> Unknown11;
		int16_t Flags4;
		int16_t Unknown12;
		Text::StringID CameraMarkerName2;
		Text::StringID CameraSubmergedMarkerName2;
		float PitchAutoLevel2;
		float PitchRangeMin2;
		float PitchRangeMax2;
		TagBlock<Unit::CameraTrackBlock> CameraTracks2;
		float Unknown13;
		float Unknown14;
		float Unknown15;
		TagBlock<Vehicle::Unknown16> Unknown17;
		TagReference AssassinationResponse;
		TagReference AssassinationWeapon;
		Text::StringID AssasinationToolStowAnchor;
		Text::StringID AssasinationToolHandMarker;
		Text::StringID AssasinationToolMarker;
		float AccelerationRangeI;
		float AccelerationRangeJ;
		float AccelerationRangeK;
		float AccelerationActionScale;
		float AccelerationAttachScale;
		float SoftPingThreshold;
		float SoftPingInterruptTime;
		float HardPingThreshold;
		float HardPingInterruptTime;
		float FeignDeathThreshold;
		float FeignDeathTime;
		float DistanceOfEvadeAnimation;
		float DistanceOfDiveAnimation;
		float StunnedMovementThreshold;
		float FeignDeathChance;
		float FeignRepeatChance;
		TagReference SpawnedTurretCharacter;
		int16_t SpawnedActorCountMin;
		int16_t SpawnedActorCountMax;
		float SpawnedVelocity;
		float AimingVelocityMaximum;
		float AimingAccelerationMaximum;
		float CasualAimingModifier;
		float LookingVelocityMaximum;
		float LookingAccelerationMaximum;
		Text::StringID RightHandNode;
		Text::StringID LeftHandNode;
		Text::StringID PreferredGunNode;
		TagReference MeleeDamage;
		TagReference BoardingMeleeDamage;
		TagReference BoardingMeleeResponse;
		TagReference EjectionMeleeDamage;
		TagReference EjectionMeleeResponse;
		TagReference LandingMeleeDamage;
		TagReference FlurryMeleeDamage;
		TagReference ObstacleSmashMeleeDamage;
		TagReference ShieldPopDamage;
		TagReference AssassinationDamage;
		int16_t MotionSensorBlipSize;
		int16_t ItemScale;
		TagBlock<Unit::Posture> Postures;
		TagBlock<Unit::HudInterface> HudInterfaces;
		TagBlock<Unit::DialogueVariant> DialogueVariants;
		float Unknown18;
		float Unknown19;
		float Unknown20;
		float Unknown21;
		float GrenadeVelocity;
		int16_t GrenadeType;
		int16_t GrenadeCount;
		TagBlock<Unit::PoweredSeat> PoweredSeats;
		TagBlock<Unit::Weapon> Weapons;
		TagBlock<Unit::TargetTrackingBlock> TargetTrackings;
		TagBlock<Unit::Seat> Seats;
		float EmpRadius;
		TagReference EmpEffect;
		TagReference BoostCollisionDamage;
		float BoostPeakPower;
		float BoostRisePower;
		float BoostPeakTime;
		float BoostFallPower;
		float BoostDeadTime;
		float LipsyncAttackWeight;
		float LipsyncDecayWeight;
		TagReference DetachDamage;
		TagReference DetachedWeapon;
		int32_t Flags5;
		TagBlock<Vehicle::TankEngineMotionProperty> TankEngineMotionProperties;
		TagBlock<Vehicle::EngineMotionProperty> EngineMotionProperties;
		TagBlock<Vehicle::DropshipMotionProperty> DropshipMotionProperties;
		TagBlock<Vehicle::AntigravityMotionProperty> AntigravityMotionProperties;
		TagBlock<Vehicle::JetEngineMotionProperty> JetEngineMotionProperties;
		TagBlock<Vehicle::TurretProperty> TurretProperties;
		uint32_t Unknown22;
		uint32_t Unknown23;
		uint32_t Unknown24;
		TagBlock<Vehicle::HelicopterMotionProperty> HelicopterMotionProperties;
		TagBlock<Vehicle::AntigravityEngineMotionProperty> AntigravityEngineMotionProperties;
		TagBlock<Vehicle::AutoturretEquipment> AutoturretEquipments;
		int32_t Flags6;
		float GroundFriction;
		float GroundDepth;
		float GroundDampFactor;
		float GroundMovingFriction;
		float GroundMaximumSlope0;
		float GroundMaximumSlope1LessThanSlope0;
		float Unknown25;
		float AntiGravityBankLift;
		float SteeringBankReactionScale;
		float GravityScale;
		float Radius;
		float Unknown26;
		float Unknown27;
		float Unknown28;
		TagBlock<Vehicle::AntiGravityPoint> AntiGravityPoints;
		TagBlock<Vehicle::FrictionPoint> FrictionPoints;
		TagBlock<Vehicle::PhantomShape> PhantomShapes;
		int8_t PlayerTrainingVehicleType;
		int8_t VehicleSize;
		int8_t Unknown29;
		int8_t Unknown30;
		float MinimumFlippingAngularVelocity;
		float MaximumFlippingAngularVelocity;
		uint32_t Unknown31;
		uint32_t Unknown32;
		float SeatEntranceAccelerationScale;
		float SeatExitAccelerationScale;
		float FlipTime;
		Text::StringID FlipOverMessage;
		TagReference SuspensionSound;
		TagReference RunningEffect;
		TagReference UnknownResponse;
		TagReference UnknownResponse2;
		uint32_t Unknown33;
		uint32_t Unknown34;

		struct Vehicle::Unknown10
		{
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10_1;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::Unknown10, 0x4C);

		struct Vehicle::Unknown16
		{
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16_1;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::Unknown16, 0x4C);

		struct Vehicle::TankEngineMotionProperty
		{
			struct Gear;

			float SteeringOverdampenCuspAngle;
			float SteeringOverdamenExponent;
			float Unknown;
			float SpeedLeft;
			float SpeedRight;
			float TurningSpeedLeft;
			float TurningSpeedRight;
			float SpeedLeft2;
			float SpeedRight2;
			float TurningSpeedLeft2;
			float TurningSpeedRight2;
			float EngineMomentum;
			float EngineMaximumAngularVelocity;
			TagBlock<Vehicle::TankEngineMotionProperty::Gear> Gears;
			TagReference ChangeGearSound;
			float Unknown2;
			float Unknown3;

			struct Vehicle::TankEngineMotionProperty::Gear
			{
				float MinTorque;
				float MaxTorque;
				float PeakTorqueScale;
				float PastPeakTorqueExponent;
				float TorqueAtMaxAngularVelovity;
				float TorqueAt2xMaxAngularVelocity;
				float MinTorque2;
				float MaxTorque2;
				float PeakTorqueScale2;
				float PastPeakTorqueExponent2;
				float TorqueAtMaxAngularVelovity2;
				float TorqueAt2xMaxAngularVelocity2;
				float MinTimeToUpshift;
				float EngineUpshiftScale;
				float GearRatio;
				float MinTimeToDownshift;
				float EngineDownshiftScale;
			};
			TAG_STRUCT_SIZE_ASSERT(Vehicle::TankEngineMotionProperty::Gear, 0x44);
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::TankEngineMotionProperty, 0x58);

		struct Vehicle::EngineMotionProperty
		{
			struct Gear;

			float SteeringOverdampenCuspAngle;
			float SteeringOverdamenExponent;
			float MaximumLeftTurn;
			float MaximumRightTurnNegative;
			float TurnRate;
			float EngineMomentum;
			float EngineMaximumAngularVelocity;
			TagBlock<Vehicle::EngineMotionProperty::Gear> Gears;
			TagReference ChangeGearSound;
			uint32_t Unknown;
			uint32_t Unknown2;

			struct Vehicle::EngineMotionProperty::Gear
			{
				float MinTorque;
				float MaxTorque;
				float PeakTorqueScale;
				float PastPeakTorqueExponent;
				float TorqueAtMaxAngularVelovity;
				float TorqueAt2xMaxAngularVelocity;
				float MinTorque2;
				float MaxTorque2;
				float PeakTorqueScale2;
				float PastPeakTorqueExponent2;
				float TorqueAtMaxAngularVelovity2;
				float TorqueAt2xMaxAngularVelocity2;
				float MinTimeToUpshift;
				float EngineUpshiftScale;
				float GearRatio;
				float MinTimeToDownshift;
				float EngineDownshiftScale;
			};
			TAG_STRUCT_SIZE_ASSERT(Vehicle::EngineMotionProperty::Gear, 0x44);
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::EngineMotionProperty, 0x40);

		struct Vehicle::DropshipMotionProperty
		{
			float ForwardAcceleration;
			float BackwardAcceleration;
			float Unknown;
			float Unknown2;
			float LeftStrafeAcceleration;
			float RightStrafeAcceleration;
			float Unknown3;
			float Unknown4;
			float LiftAcceleration;
			float DropAcceleration;
			float Unknown5;
			float Unknown6;
			float Unknown7;
			float Unknown8;
			float Unknown9;
			float Unknown10;
			float Unknown11;
			float Unknown12;
			float Unknown13;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::DropshipMotionProperty, 0x4C);

		struct Vehicle::AntigravityMotionProperty
		{
			float SteeringOverdampenCuspAngle;
			float SteeringOverdamenExponent;
			float MaximumForwardSpeed;
			float MaximumReverseSpeed;
			float SpeedAcceleration;
			float SpeedDeceleration;
			float MaximumLeftSlide;
			float MaximumRightSlide;
			float SlideAcceleration;
			float SlideDeceleration;
			int8_t Unknown;
			int8_t Unknown2;
			int8_t Unknown3;
			int8_t Unknown4;
			float Traction;
			uint32_t Unknown5;
			float TurningRate;
			Text::StringID Unknown6;
			float Unknown7;
			float Unknown8;
			float Unknown9;
			float Unknown10;
			Text::StringID Unknown11;
			float Unknown12;
			float Unknown13;
			float Unknown14;
			float Unknown15;
			float Unknown16;
			float Unknown17;
			float Unknown18;
			float Unknown19;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::AntigravityMotionProperty, 0x70);

		struct Vehicle::JetEngineMotionProperty
		{
			float SteeringOverdampenCuspAngle;
			float SteeringOverdamenExponent;
			float MaximumLeftTurn;
			float MaximumRightTurnNegative;
			float TurnRate;
			float FlyingSpeed;
			float Acceleration;
			float SpeedAcceleration;
			float SpeedDeceleration;
			float PitchLeftSpeed;
			float PitchRightSpeed;
			float PitchRate;
			float UnpitchRate;
			float FlightStability;
			uint32_t Unknown;
			float NoseAngle;
			float Unknown2;
			float Unknown3;
			float Unknown4;
			float FallingSpeed;
			float FallingSpeed2;
			float Unknown5;
			float Unknown6;
			float IdleRise;
			float IdleForward;
			uint32_t Unknown7;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::JetEngineMotionProperty, 0x68);

		struct Vehicle::TurretProperty
		{
			uint32_t Unknown;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::TurretProperty, 0x4);

		struct Vehicle::HelicopterMotionProperty
		{
			float MaximumLeftTurn;
			float MaximumRightTurnNegative;
			float Unknown;
			Text::StringID ThrustFrontLeft;
			Text::StringID ThrustFrontRight;
			Text::StringID Thrust;
			float Unknown2;
			float Unknown3;
			float Unknown4;
			float Unknown5;
			float Unknown6;
			float Unknown7;
			float Unknown8;
			float Unknown9;
			float Unknown10;
			float Unknown11;
			float Unknown12;
			float Unknown13;
			float Unknown14;
			float Unknown15;
			float Unknown16;
			float Unknown17;
			float Unknown18;
			float Unknown19;
			float Unknown20;
			float Unknown21;
			float Unknown22;
			float Unknown23;
			float Unknown24;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::HelicopterMotionProperty, 0x74);

		struct Vehicle::AntigravityEngineMotionProperty
		{
			struct Gear;

			float SteeringOverdampenCuspAngle;
			float SteeringOverdamenExponent;
			float MaximumLeftTurn;
			float MaximumRightTurnNegative;
			float TurnRate;
			float EngineMomentum;
			float EngineMaximumAngularVelocity;
			TagBlock<Vehicle::AntigravityEngineMotionProperty::Gear> Gears;
			TagReference ChangeGearSound;
			float Unknown;
			Text::StringID Unknown2;
			float Unknown3;
			float Unknown4;
			float Unknown5;
			float Unknown6;
			float Unknown7;
			float Unknown8;
			float Unknown9;
			float Unknown10;
			float Unknown11;
			float Unknown12;
			float Unknown13;
			float Unknown14;

			struct Vehicle::AntigravityEngineMotionProperty::Gear
			{
				float MinTorque;
				float MaxTorque;
				float PeakTorqueScale;
				float PastPeakTorqueExponent;
				float TorqueAtMaxAngularVelovity;
				float TorqueAt2xMaxAngularVelocity;
				float MinTorque2;
				float MaxTorque2;
				float PeakTorqueScale2;
				float PastPeakTorqueExponent2;
				float TorqueAtMaxAngularVelovity2;
				float TorqueAt2xMaxAngularVelocity2;
				float MinTimeToUpshift;
				float EngineUpshiftScale;
				float GearRatio;
				float MinTimeToDownshift;
				float EngineDownshiftScale;
			};
			TAG_STRUCT_SIZE_ASSERT(Vehicle::AntigravityEngineMotionProperty::Gear, 0x44);
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::AntigravityEngineMotionProperty, 0x70);

		struct Vehicle::AutoturretEquipment
		{
			float Unknown;
			float Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			float Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			float Unknown10;
			float Unknown11;
			float Unknown12;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::AutoturretEquipment, 0x30);

		struct Vehicle::AntiGravityPoint
		{
			Text::StringID MarkerName;
			int32_t Flags;
			float AntigravStrength;
			float AntigravOffset;
			float AntigravHeight;
			float AntigravDumpFactor;
			float AntigravNormalK1;
			float AntigravNormalK0;
			float Radius;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			int16_t Unknown4;
			int16_t DamageSourceRegionIndex;
			Text::StringID DamageSourceRegionName;
			float DefaultStateError;
			float MinorDamageError;
			float MediumDamageError;
			float MajorDamageError;
			float DestroyedStateError;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::AntiGravityPoint, 0x4C);

		struct Vehicle::FrictionPoint
		{
			Text::StringID MarkerName;
			int32_t Flags;
			float FractionOfTotalMass;
			float Radius;
			float DamagedRadius;
			int16_t FrictionType;
			int16_t Unknown;
			float MovingFrictionVelocityDiff;
			float EBrakeMovingFriction;
			float EBrakeFriction;
			float EBrakeMovingFrictionVelocityDiff;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			Text::StringID CollisionMaterialName;
			int16_t CollisionGlobalMaterialIndex;
			int16_t ModelStateDestroyed;
			Text::StringID RegionName;
			int32_t RegionIndex;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::FrictionPoint, 0x4C);

		struct Vehicle::PhantomShape
		{
			struct PhantomShapePart
			{
				struct PhantomShapeSphere
				{
					Blam::Math::RealPoint3D Position;
					float Radius;
				};

				long Unknown1;
				short Size;
				short Count;
				long OverallShapeIndex;
				long Offset;
				long NumberOfSpheres;
				unsigned long Unknown2;
				unsigned long Unknown3;
				unsigned long Unknown4;
				PhantomShapeSphere Spheres[8];
			};

			int32_t Unknown;
			int16_t Size;
			int16_t Count;
			int32_t OverallShapeIndex;
			int32_t Offset;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			int32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
			float Unknown10;
			float Unknown11;
			float Unknown12;
			float Unknown13;
			float Unknown14;
			float Unknown15;
			float Unknown16;
			uint32_t Unknown17;
			int32_t MultisphereCount;
			int32_t Flags;
			float X0;
			float X1;
			float Y0;
			float Y1;
			float Z0;
			float Z1;
			PhantomShapePart Parts[5];
			uint32_t Unknown34;
			uint32_t Unknown35;
			uint32_t Unknown36;
			uint32_t Unknown37;
			uint32_t Unknown38;
			uint32_t Unknown39;
			uint32_t Unknown40;
			uint32_t Unknown41;
			uint32_t Unknown42;
			uint32_t Unknown43;
			uint32_t Unknown44;
			uint32_t Unknown45;
			uint32_t Unknown46;
			uint32_t Unknown47;
			uint32_t Unknown48;
			uint32_t Unknown49;
		};
		TAG_STRUCT_SIZE_ASSERT(Vehicle::PhantomShape, 0x3D0);
	};
	TAG_STRUCT_SIZE_ASSERT(Vehicle, 0x918);
}
