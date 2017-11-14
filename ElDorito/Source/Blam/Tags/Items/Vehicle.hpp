#pragma once
#include "../../Math/RealPoint3D.hpp"
#include "../../Text/StringID.hpp"
#include "../Tags.hpp"
#include "Item.hpp"

namespace Blam::Tags::Items
{
	struct Vehicle : TagGroup<'vehi'>
	{
		Item Item;

		struct EarlyMoverProperty;
		struct AiProperty;
		struct Function;
		struct Attachment;
		struct Widget;
		struct ChangeColor;
		struct NodeMap;
		struct MultiplayerObjectProperty;
		struct ModelObjectDatum;
		struct MetagameProperty;
		struct CameraTrack;
		struct Unknown10;
		struct CameraTrack2;
		struct Unknown16;
		struct Posture;
		struct HudInterface;
		struct DialogueVariant;
		struct PoweredSeat;
		struct Weapon;
		struct TargetTracking;
		struct Seat;
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

		int16_t ObjectType;
		uint16_t Flags;
		float BoundingRadius;
		float BoundingOffsetX;
		float BoundingOffsetY;
		float BoundingOffsetZ;
		float AccelerationScale;
		int16_t LightmapShadowModeSize;
		int8_t SweetenerSize;
		int8_t WaterDensity;
		int32_t Unknown;
		float DynamicLightSphereRadius;
		float DynamicLightSphereOffsetX;
		float DynamicLightSphereOffsetY;
		float DynamicLightSphereOffsetZ;
		Text::StringID DefaultModelVariant;
		TagReference Model;
		TagReference CrateObject;
		TagReference CollisionDamage;
		TagBlock<EarlyMoverProperty> EarlyMoverProperties;
		TagReference CreationEffect;
		TagReference MaterialEffects;
		TagReference ArmorSounds;
		TagReference MeleeImpact;
		TagBlock<AiProperty> AiProperties;
		TagBlock<Function> Functions;
		int16_t HudTextMessageIndex;
		int16_t Unknown2;
		TagBlock<Attachment> Attachments;
		TagBlock<Widget> Widgets;
		TagBlock<ChangeColor> ChangeColors;
		TagBlock<NodeMap> NodeMaps;
		TagBlock<MultiplayerObjectProperty> MultiplayerObjectProperties;
		uint32_t Unknown3;
		uint32_t Unknown4;
		uint32_t Unknown5;
		TagBlock<ModelObjectDatum> ModelObjectData;
		int32_t Flags2;
		int16_t DefaultTeam;
		int16_t ConstantSoundVolume;
		TagReference HologramUnit;
		TagBlock<MetagameProperty> MetagameProperties;
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
		TagBlock<CameraTrack> CameraTracks;
		float Unknown7;
		float Unknown8;
		float Unknown9;
		TagBlock<Unknown10> Unknown11;
		int16_t Flags4;
		int16_t Unknown12;
		Text::StringID CameraMarkerName2;
		Text::StringID CameraSubmergedMarkerName2;
		float PitchAutoLevel2;
		float PitchRangeMin2;
		float PitchRangeMax2;
		TagBlock<CameraTrack2> CameraTracks2;
		float Unknown13;
		float Unknown14;
		float Unknown15;
		TagBlock<Unknown16> Unknown17;
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
		TagBlock<Posture> Postures;
		TagBlock<HudInterface> HudInterfaces;
		TagBlock<DialogueVariant> DialogueVariants;
		float Unknown18;
		float Unknown19;
		float Unknown20;
		float Unknown21;
		float GrenadeVelocity;
		int16_t GrenadeType;
		int16_t GrenadeCount;
		TagBlock<PoweredSeat> PoweredSeats;
		TagBlock<Weapon> Weapons;
		TagBlock<TargetTracking> TargetTracking2;
		TagBlock<Seat> Seats;
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
		TagBlock<TankEngineMotionProperty> TankEngineMotionProperties;
		TagBlock<EngineMotionProperty> EngineMotionProperties;
		TagBlock<DropshipMotionProperty> DropshipMotionProperties;
		TagBlock<AntigravityMotionProperty> AntigravityMotionProperties;
		TagBlock<JetEngineMotionProperty> JetEngineMotionProperties;
		TagBlock<TurretProperty> TurretProperties;
		uint32_t Unknown22;
		uint32_t Unknown23;
		uint32_t Unknown24;
		TagBlock<HelicopterMotionProperty> HelicopterMotionProperties;
		TagBlock<AntigravityEngineMotionProperty> AntigravityEngineMotionProperties;
		TagBlock<AutoturretEquipment> AutoturretEquipment2;
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
		TagBlock<AntiGravityPoint> AntiGravityPoints;
		TagBlock<FrictionPoint> FrictionPoints;
		TagBlock<PhantomShape> PhantomShapes;
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

		struct EarlyMoverProperty
		{
			Text::StringID Name;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
		};
		TAG_STRUCT_SIZE_ASSERT(struct EarlyMoverProperty, 0x28);

		struct AiProperty
		{
			int32_t Flags;
			Text::StringID AiTypeName;
			int16_t Size;
			int16_t LeapJumpSpeed;
		};
		TAG_STRUCT_SIZE_ASSERT(struct AiProperty, 0xC);

		struct Function
		{
			int32_t Flags;
			Text::StringID ImportName;
			Text::StringID ExportName;
			Text::StringID TurnOffWith;
			float MinimumValue;
			TagData<uint8_t> DefaultFunction;
			Text::StringID ScaleBy;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Function, 0x2C);

		struct Attachment
		{
			int32_t AtlasFlags;
			TagReference Attachment_1;
			Text::StringID Marker;
			int16_t ChangeColor;
			int16_t Unknown;
			Text::StringID PrimaryScale;
			Text::StringID SecondaryScale;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Attachment, 0x24);

		struct Widget
		{
			TagReference Type;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Widget, 0x10);

		struct ChangeColor
		{
			struct InitialPermutation;
			struct Function;

			TagBlock<InitialPermutation> InitialPermutations;
			TagBlock<Function> Functions;

			struct InitialPermutation
			{
				uint32_t Weight;
				float ColorLowerBoundR;
				float ColorLowerBoundG;
				float ColorLowerBoundB;
				float ColorUpperBoundR;
				float ColorUpperBoundG;
				float ColorUpperBoundB;
				Text::StringID VariantName;
			};
			TAG_STRUCT_SIZE_ASSERT(struct InitialPermutation, 0x20);

			struct Function
			{
				int32_t ScaleFlags;
				float ColorLowerBoundR;
				float ColorLowerBoundG;
				float ColorLowerBoundB;
				float ColorUpperBoundR;
				float ColorUpperBoundG;
				float ColorUpperBoundB;
				Text::StringID DarkenBy;
				Text::StringID ScaleBy;
			};
			TAG_STRUCT_SIZE_ASSERT(struct Function, 0x24);
		};
		TAG_STRUCT_SIZE_ASSERT(struct ChangeColor, 0x18);

		struct NodeMap
		{
			int8_t TargetNode;
		};
		TAG_STRUCT_SIZE_ASSERT(struct NodeMap, 0x1);

		struct MultiplayerObjectProperty
		{
			uint16_t EngineFlags;
			int8_t ObjectType;
			uint8_t TeleporterFlags;
			uint16_t Flags;
			int8_t Shape;
			int8_t SpawnTimerMode;
			int16_t SpawnTime;
			int16_t AbandonTime;
			float RadiusWidth;
			float Length;
			float Top;
			float Bottom;
			float Unknown;
			float Unknown2;
			float Unknown3;
			int32_t Unknown4;
			int32_t Unknown5;
			TagReference ChildObject;
			int32_t Unknown6;
			TagReference ShapeShader;
			TagReference UnknownShader;
			TagReference Unknown7;
			TagReference Unknown8;
			TagReference Unknown9;
			TagReference Unknown10;
			TagReference Unknown11;
			TagReference Unknown12;
		};
		TAG_STRUCT_SIZE_ASSERT(struct MultiplayerObjectProperty, 0xC4);

		struct ModelObjectDatum
		{
			int16_t Type;
			int16_t Unknown;
			float OffsetX;
			float OffsetY;
			float OffsetZ;
			float Radius;
		};
		TAG_STRUCT_SIZE_ASSERT(struct ModelObjectDatum, 0x14);

		struct MetagameProperty
		{
			uint8_t Flags;
			int8_t Unit;
			int8_t Classification;
			int8_t Unknown;
			int16_t Points;
			int16_t Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(struct MetagameProperty, 0x8);

		struct CameraTrack
		{
			TagReference Track;
		};
		TAG_STRUCT_SIZE_ASSERT(struct CameraTrack, 0x10);

		struct Unknown10
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
		TAG_STRUCT_SIZE_ASSERT(struct Unknown10, 0x4C);

		struct CameraTrack2
		{
			TagReference Track;
		};
		TAG_STRUCT_SIZE_ASSERT(struct CameraTrack2, 0x10);

		struct Unknown16
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
		TAG_STRUCT_SIZE_ASSERT(struct Unknown16, 0x4C);

		struct Posture
		{
			Text::StringID Name;
			float PillOffsetI;
			float PillOffsetJ;
			float PillOffsetK;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Posture, 0x10);

		struct HudInterface
		{
			TagReference UnitHudInterface;
		};
		TAG_STRUCT_SIZE_ASSERT(struct HudInterface, 0x10);

		struct DialogueVariant
		{
			int16_t VariantNumber;
			int16_t Unknown;
			TagReference Dialogue;
		};
		TAG_STRUCT_SIZE_ASSERT(struct DialogueVariant, 0x14);

		struct PoweredSeat
		{
			float DriverPowerupTime;
			float DriverPowerdownTime;
		};
		TAG_STRUCT_SIZE_ASSERT(struct PoweredSeat, 0x8);

		struct Weapon
		{
			TagReference Weapon_1;
		};
		TAG_STRUCT_SIZE_ASSERT(struct Weapon, 0x10);

		struct TargetTracking
		{
			struct TrackingType;

			TagBlock<TrackingType> TrackingTypes;
			float AcquireTime;
			float GraceTime;
			float DecayTime;
			TagReference TrackingSound;
			TagReference LockedSound;

			struct TrackingType
			{
				Text::StringID TrackingType_1;
			};
			TAG_STRUCT_SIZE_ASSERT(struct TrackingType, 0x4);
		};
		TAG_STRUCT_SIZE_ASSERT(struct TargetTracking, 0x38);

		struct Seat
		{
			struct CameraTrack;
			struct Unknown7;
			struct UnitHudInterface;

			int32_t Flags;
			Text::StringID SeatAnimation;
			Text::StringID SeatMarkerName;
			Text::StringID EntryMarkerSName;
			Text::StringID BoardingGrenadeMarker;
			Text::StringID BoardingGrenadeString;
			Text::StringID BoardingMeleeString;
			Text::StringID DetachWeaponString;
			float PingScale;
			float TurnoverTime;
			float AccelerationRangeI;
			float AccelerationRangeJ;
			float AccelerationRangeK;
			float AccelerationActionScale;
			float AccelerationAttachScale;
			float AiScariness;
			int16_t AiSeatType;
			int16_t BoardingSeat;
			float ListenerInterpolationFactor;
			float YawRateBoundsMin;
			float YawRateBoundsMax;
			float PitchRateBoundsMin;
			float PitchRateBoundsMax;
			float Unknown;
			float MinimumSpeedReference;
			float MaximumSpeedReference;
			float SpeedExponent;
			int16_t Unknown2;
			int16_t Unknown3;
			Text::StringID CameraMarkerName;
			Text::StringID CameraSubmergedMarkerName;
			float PitchAutoLevel;
			float PitchRangeMin;
			float PitchRangeMax;
			TagBlock<CameraTrack> CameraTracks;
			float Unknown4;
			float Unknown5;
			float Unknown6;
			TagBlock<Unknown7> Unknown8;
			TagBlock<UnitHudInterface> UnitHudInterface2;
			Text::StringID EnterSeatString;
			float YawRangeMin;
			float YawRangeMax;
			TagReference BuiltInGunner;
			float EntryRadius;
			float EntryMarkerConeAngle;
			float EntryMarkerFacingAngle;
			float MaximumRelativeVelocity;
			Text::StringID InvisibleSeatRegion;
			int32_t RuntimeInvisibleSeatRegionIndex;

			struct CameraTrack
			{
				TagReference Track;
			};
			TAG_STRUCT_SIZE_ASSERT(struct CameraTrack, 0x10);

			struct Unknown7
			{
				float Unknown;
				float Unknown2;
				float Unknown3;
				float Unknown4;
				float Unknown5;
				float Unknown6;
				float Unknown7_1;
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
			};
			TAG_STRUCT_SIZE_ASSERT(struct Unknown7, 0x4C);

			struct UnitHudInterface
			{
				TagReference UnitHudInterface_1;
			};
			TAG_STRUCT_SIZE_ASSERT(struct UnitHudInterface, 0x10);
		};
		TAG_STRUCT_SIZE_ASSERT(struct Seat, 0xE4);

		struct TankEngineMotionProperty
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
			TagBlock<Gear> Gears;
			TagReference ChangeGearSound;
			float Unknown2;
			float Unknown3;

			struct Gear
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
			TAG_STRUCT_SIZE_ASSERT(struct Gear, 0x44);
		};
		TAG_STRUCT_SIZE_ASSERT(struct TankEngineMotionProperty, 0x58);

		struct EngineMotionProperty
		{
			struct Gear;

			float SteeringOverdampenCuspAngle;
			float SteeringOverdamenExponent;
			float MaximumLeftTurn;
			float MaximumRightTurnNegative;
			float TurnRate;
			float EngineMomentum;
			float EngineMaximumAngularVelocity;
			TagBlock<Gear> Gears;
			TagReference ChangeGearSound;
			uint32_t Unknown;
			uint32_t Unknown2;

			struct Gear
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
			TAG_STRUCT_SIZE_ASSERT(struct Gear, 0x44);
		};
		TAG_STRUCT_SIZE_ASSERT(struct EngineMotionProperty, 0x40);

		struct DropshipMotionProperty
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
		TAG_STRUCT_SIZE_ASSERT(struct DropshipMotionProperty, 0x4C);

		struct AntigravityMotionProperty
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
		TAG_STRUCT_SIZE_ASSERT(struct AntigravityMotionProperty, 0x70);

		struct JetEngineMotionProperty
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
		TAG_STRUCT_SIZE_ASSERT(struct JetEngineMotionProperty, 0x68);

		struct TurretProperty
		{
			uint32_t Unknown;
			uint32_t Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(struct TurretProperty, 0x8);

		struct HelicopterMotionProperty
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
		TAG_STRUCT_SIZE_ASSERT(struct HelicopterMotionProperty, 0x74);

		struct AntigravityEngineMotionProperty
		{
			struct Gear;

			float SteeringOverdampenCuspAngle;
			float SteeringOverdamenExponent;
			float MaximumLeftTurn;
			float MaximumRightTurnNegative;
			float TurnRate;
			float EngineMomentum;
			float EngineMaximumAngularVelocity;
			TagBlock<Gear> Gears;
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

			struct Gear
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
			TAG_STRUCT_SIZE_ASSERT(struct Gear, 0x44);
		};
		TAG_STRUCT_SIZE_ASSERT(struct AntigravityEngineMotionProperty, 0x70);

		struct AutoturretEquipment
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
		TAG_STRUCT_SIZE_ASSERT(struct AutoturretEquipment, 0x30);

		struct AntiGravityPoint
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
		TAG_STRUCT_SIZE_ASSERT(struct AntiGravityPoint, 0x4C);

		struct FrictionPoint
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
		TAG_STRUCT_SIZE_ASSERT(struct FrictionPoint, 0x4C);

		struct PhantomShape
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
		TAG_STRUCT_SIZE_ASSERT(struct PhantomShape, 0x3D0);
	};
	TAG_STRUCT_SIZE_ASSERT(struct Vehicle, 0x704);
}
