#pragma once
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::Objects
{
	using Blam::Text::StringID;

	struct Projectile : TagGroup<'proj'>
	{
		struct EarlyMoverProperty;
		struct AiProperty;
		struct Function;
		struct Attachment;
		struct Widget;
		struct ChangeColor;
		struct NodeMap;
		struct MultiplayerObjectProperty;
		struct ModelObjectDatum;
		struct MaterialResponse;
		struct ImpactProperty;
		struct Unknown16;
		struct ShotgunProperty;

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
		StringID DefaultModelVariant;
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
		int16_t DetonationTimerStarts;
		int16_t ImpactNoise;
		float CollisionRadius;
		float ArmingTime;
		float DangerRadius;
		float TimerMin;
		float TimerMax;
		float MinimumVelocity;
		float MaximumRange;
		float DetonationChargeTime;
		int16_t DetonationNoise;
		int16_t SuperDetonationProjectileCount;
		float SuperDetonationDelay;
		TagReference DetonationStarted;
		TagReference AirborneDetonationEffect;
		TagReference GroundDetonationEffect;
		TagReference DetonationDamage;
		TagReference AttachedDetonationDamage;
		TagReference SuperDetonation;
		TagReference SuperDetonationDamage;
		TagReference DetonationSound;
		int8_t DamageReportingType;
		int8_t Unknown6;
		int8_t Unknown7;
		int8_t Unknown8;
		TagReference AttachedSuperDetonationDamage;
		float MaterialEffectRadius;
		TagReference FlybySound;
		TagReference FlybyResponse;
		TagReference ImpactEffect;
		TagReference ImpactDamage;
		float BoardingDetonationTime;
		TagReference BoardingDetonationDamage;
		TagReference BoardingAttachedDetonationDamage;
		float AirGravityScale;
		float AirDamageRangeMin;
		float AirDamageRangeMax;
		float WaterGravityScale;
		float WaterDamageScaleMin;
		float WaterDamageScaleMax;
		float InitialVelocity;
		float FinalVelocity;
		float Unknown9;
		float Unknown10;
		float GuidedAngularVelocityLower;
		float GuidedAngularVelocityUpper;
		float Unknown11;
		float AccelerationRangeMin;
		float AccelerationRangeMax;
		float Unknown12;
		uint32_t Unknown13;
		float TargetedLeadingFraction;
		uint32_t Unknown14;
		uint32_t Unknown15;
		TagBlock<MaterialResponse> MaterialResponses;
		TagBlock<ImpactProperty> ImpactProperties;
		TagBlock<Unknown16> Unknown17;
		TagBlock<ShotgunProperty> ShotgunProperties;

		struct EarlyMoverProperty
		{
			StringID Name;
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
		TAG_STRUCT_SIZE_ASSERT(EarlyMoverProperty, 0x28);

		struct AiProperty
		{
			int32_t Flags;
			StringID AiTypeName;
			int16_t Size;
			int16_t LeapJumpSpeed;
		};
		TAG_STRUCT_SIZE_ASSERT(AiProperty, 0xC);

		struct Function
		{
			int32_t Flags;
			StringID ImportName;
			StringID ExportName;
			StringID TurnOffWith;
			float MinimumValue;
			TagData<uint8_t> DefaultFunction;
			StringID ScaleBy;
		};
		TAG_STRUCT_SIZE_ASSERT(Function, 0x2C);

		struct Attachment
		{
			int32_t AtlasFlags;
			TagReference Tag;
			StringID Marker;
			int16_t ChangeColor;
			int16_t Unknown;
			StringID PrimaryScale;
			StringID SecondaryScale;
		};
		TAG_STRUCT_SIZE_ASSERT(Attachment, 0x24);

		struct Widget
		{
			TagReference Type;
		};
		TAG_STRUCT_SIZE_ASSERT(Widget, 0x10);

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
				StringID VariantName;
			};
			TAG_STRUCT_SIZE_ASSERT(InitialPermutation, 0x20);

			struct Function
			{
				int32_t ScaleFlags;
				float ColorLowerBoundR;
				float ColorLowerBoundG;
				float ColorLowerBoundB;
				float ColorUpperBoundR;
				float ColorUpperBoundG;
				float ColorUpperBoundB;
				StringID DarkenBy;
				StringID ScaleBy;
			};
			TAG_STRUCT_SIZE_ASSERT(Function, 0x24);
		};
		TAG_STRUCT_SIZE_ASSERT(ChangeColor, 0x18);

		struct NodeMap
		{
			int8_t TargetNode;
		};
		TAG_STRUCT_SIZE_ASSERT(NodeMap, 0x1);

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
		TAG_STRUCT_SIZE_ASSERT(MultiplayerObjectProperty, 0xC4);

		struct ModelObjectDatum
		{
			int16_t Type;
			int16_t Unknown;
			float OffsetX;
			float OffsetY;
			float OffsetZ;
			float Radius;
		};
		TAG_STRUCT_SIZE_ASSERT(ModelObjectDatum, 0x14);

		struct MaterialResponse
		{
			uint16_t Flags;
			int16_t Response;
			StringID MaterialName;
			int16_t GlobalMaterialIndex;
			int16_t Unknown;
			int16_t Response2;
			uint16_t Flags2;
			float ChanceFraction;
			float BetweenAngleMin;
			float BetweenAngleMax;
			float AndVelocityMin;
			float AndVelocityMax;
			int16_t ScaleEffectsBy;
			int16_t Unknown2;
			float AngularNoise;
			float VelocityNoise;
			float InitialFriction;
			float MaximumDistance;
			float ParallelFriction;
			float PerpendicularFriction;
		};
		TAG_STRUCT_SIZE_ASSERT(MaterialResponse, 0x40);

		struct ImpactProperty
		{
			float Unknown;
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
		};
		TAG_STRUCT_SIZE_ASSERT(ImpactProperty, 0x30);

		struct Unknown16
		{
			uint32_t Unknown;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown16, 0x4);

		struct ShotgunProperty
		{
			int16_t Amount;
			int16_t Distance;
			float Accuracy;
			float SpreadConeAngle;
		};
		TAG_STRUCT_SIZE_ASSERT(ShotgunProperty, 0xC);
	};
	TAG_STRUCT_SIZE_ASSERT(Projectile, 0x2CC);
}
