#pragma once
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::Models
{
	struct Model : TagGroup<'hlmt'>
	{
		struct Variant;
		struct Unknown;
		struct InstanceGroup;
		struct Material;
		struct NewDamageInfoBlock;
		struct Target;
		struct CollisionRegion;
		struct Node;
		struct ModelObjectDatum;
		struct Unknown6;
		struct Unknown8;
		struct Unknown10;

		TagReference ModelTag;
		TagReference CollisionModel;
		TagReference Animation;
		TagReference PhysicsModel;
		float ReduceToL1SuperLow;
		float ReduceToL2Low;
		float ReduceToL3Medium;
		float ReduceToL4High;
		float ReduceToL5SuperHigh;
		TagReference LodModel;
		TagBlock<Variant> Variants;
		TagBlock<Unknown> Unknown2;
		TagBlock<InstanceGroup> InstanceGroups;
		TagBlock<Material> Materials;
		TagBlock<NewDamageInfoBlock> NewDamageInfo;
		TagBlock<Target> Targets;
		TagBlock<CollisionRegion> CollisionRegions;
		TagBlock<Node> Nodes;
		uint32_t Unknown3;
		TagBlock<ModelObjectDatum> ModelObjectData;
		TagReference PrimaryDialogue;
		TagReference SecondaryDialogue;
		int32_t Flags;
		Text::StringID DefaultDialogueEffect;
		int32_t RenderOnlyNodeFlags1;
		int32_t RenderOnlyNodeFlags2;
		int32_t RenderOnlyNodeFlags3;
		int32_t RenderOnlyNodeFlags4;
		int32_t RenderOnlyNodeFlags5;
		int32_t RenderOnlyNodeFlags6;
		int32_t RenderOnlyNodeFlags7;
		int32_t RenderOnlyNodeFlags8;
		int32_t RenderOnlySectionFlags1;
		int32_t RenderOnlySectionFlags2;
		int32_t RenderOnlySectionFlags3;
		int32_t RenderOnlySectionFlags4;
		int32_t RenderOnlySectionFlags5;
		int32_t RenderOnlySectionFlags6;
		int32_t RenderOnlySectionFlags7;
		int32_t RenderOnlySectionFlags8;
		int32_t RuntimeFlags;
		uint32_t ScenarioLoadParametersBlock;
		uint32_t ScenarioLoadParametersBlock2;
		uint32_t ScenarioLoadParametersBlock3;
		int16_t Unknown4;
		int16_t Unknown5;
		TagBlock<Unknown6> Unknown7;
		TagBlock<Unknown8> Unknown9;
		TagBlock<Unknown10> Unknown11;
		TagReference ShieldImpactThirdPerson;
		TagReference ShieldImpactFirstPerson;
		TagReference OvershieldThirdPerson;
		TagReference OvershieldFirstPerson;

		struct Variant
		{
			struct Region;
			struct Object;

			Text::StringID Name;
			TagReference VariantDialogue;
			Text::StringID DefaultDialogEffect;
			int8_t CharacterFilter;
			int8_t Unknown;
			int8_t Unknown2;
			int8_t Unknown3;
			int8_t ModelRegion0Index;
			int8_t ModelRegion1Index;
			int8_t ModelRegion2Index;
			int8_t ModelRegion3Index;
			int8_t ModelRegion4Index;
			int8_t ModelRegion5Index;
			int8_t ModelRegion6Index;
			int8_t ModelRegion7Index;
			int8_t ModelRegion8Index;
			int8_t ModelRegion9Index;
			int8_t ModelRegion10Index;
			int8_t ModelRegion11Index;
			int8_t ModelRegion12Index;
			int8_t ModelRegion13Index;
			int8_t ModelRegion14Index;
			int8_t ModelRegion15Index;
			TagBlock<Region> Regions;
			TagBlock<Object> Objects;
			int32_t InstanceGroupIndex;
			uint32_t Unknown4;
			uint32_t Unknown5;

			struct Region
			{
				struct Permutation;

				Text::StringID Name;
				int8_t ModelRegionIndex;
				int8_t Unknown;
				int16_t ParentVariantIndex;
				TagBlock<Permutation> Permutations;
				int32_t SortOrder;

				struct Permutation
				{
					struct StateBlock;

					Text::StringID Name;
					int8_t ModelPermutationIndex;
					uint8_t Flags;
					int8_t Unknown;
					int8_t Unknown2;
					float Probability;
					TagBlock<StateBlock> States;
					uint32_t Unknown3;
					uint32_t Unknown4;
					uint32_t Unknown5;

					struct StateBlock
					{
						Text::StringID Name;
						int8_t ModelPermutationIndex;
						uint8_t PropertyFlags;
						int16_t State;
						TagReference LoopingEffect;
						Text::StringID LoopingEffectMarkerName;
						float InitialProbability;
					};
					TAG_STRUCT_SIZE_ASSERT(StateBlock, 0x20);
				};
				TAG_STRUCT_SIZE_ASSERT(Permutation, 0x24);
			};
			TAG_STRUCT_SIZE_ASSERT(Region, 0x18);

			struct Object
			{
				Text::StringID ParentMarker;
				Text::StringID ChildMarker;
				Text::StringID ChildVariant;
				TagReference ChildObject;
			};
			TAG_STRUCT_SIZE_ASSERT(Object, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(Variant, 0x50);

		struct Unknown
		{
			Text::StringID Unknown1;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown, 0x4);

		struct InstanceGroup
		{
			struct InstanceMember;

			Text::StringID Name;
			int32_t Unknown;
			TagBlock<InstanceMember> InstanceMembers;
			float Probability;

			struct InstanceMember
			{
				int32_t Unknown;
				Text::StringID InstanceName;
				float Probability;
				int32_t InstanceFlags1;
				int32_t InstanceFlags2;
				int32_t InstanceFlags3;
				int32_t InstanceFlags4;
			};
			TAG_STRUCT_SIZE_ASSERT(InstanceMember, 0x1C);
		};
		TAG_STRUCT_SIZE_ASSERT(InstanceGroup, 0x18);

		struct Material
		{
			Text::StringID Name;
			int16_t Unknown;
			int16_t DamageSectionIndex;
			int16_t Unknown2;
			int16_t Unknown3;
			Text::StringID MaterialName;
			int16_t GlobalMaterialIndex;
			int16_t Unknown4;
		};
		TAG_STRUCT_SIZE_ASSERT(Material, 0x14);

		struct NewDamageInfoBlock
		{
			struct DamageSection;
			struct Node;
			struct DamageSeat;
			struct DamageConstraint;

			int32_t Flags;
			Text::StringID GlobalIndirectMaterialName;
			int16_t IndirectDamageSection;
			int16_t Unknown;
			uint32_t Unknown2;
			int8_t CollisionDamageReportingType;
			int8_t ResponseDamageReportingType;
			int16_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			float MaxVitality;
			float MinStunDamage;
			float StunTime;
			float RechargeTime;
			float RechargeFraction;
			uint32_t Unknown9;
			uint32_t Unknown10;
			uint32_t Unknown11;
			uint32_t Unknown12;
			uint32_t Unknown13;
			uint32_t Unknown14;
			uint32_t Unknown15;
			uint32_t Unknown16;
			uint32_t Unknown17;
			uint32_t Unknown18;
			uint32_t Unknown19;
			uint32_t Unknown20;
			uint32_t Unknown21;
			uint32_t Unknown22;
			uint32_t Unknown23;
			uint32_t Unknown24;
			float MaxShieldVitality;
			Text::StringID GlobalShieldMaterialName;
			float MinStunDamage2;
			float StunTime2;
			float ShieldRechargeTime;
			float ShieldDamagedThreshold;
			TagReference ShieldDamagedEffect;
			TagReference ShieldDepletedEffect;
			TagReference ShieldRechargingEffect;
			TagBlock<DamageSection> DamageSections;
			TagBlock<Node> Nodes;
			int16_t GlobalShieldMaterialIndex;
			int16_t GlobalIndirectMaterialIndex;
			uint32_t Unknown25;
			uint32_t Unknown26;
			TagBlock<DamageSeat> DamageSeats;
			TagBlock<DamageConstraint> DamageConstraints;

			struct DamageSection
			{
				struct InstantRespons;

				Text::StringID Name;
				int32_t Flags;
				float VitalityPercentage;
				TagBlock<InstantRespons> InstantResponses;
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
				uint32_t Unknown6;
				float StunTime;
				float RechargeTime;
				float Unknown7;
				Text::StringID ResurrectionRegionName;
				int16_t RessurectionRegionRuntimeIndex;
				int16_t Unknown8;

				struct InstantRespons
				{
					int16_t ResponseType;
					int16_t ConstraintDamageType;
					Text::StringID Trigger;
					int32_t Flags;
					float DamageThreshold;
					TagReference PrimaryTransitionEffect;
					TagReference SecondaryTransitionEffect;
					TagReference TransitionDamageEffect;
					Text::StringID Region;
					int16_t NewState;
					int16_t RuntimeRegionIndex;
					Text::StringID SecondaryRegion;
					int16_t SecondaryNewState;
					int16_t SecondaryRuntimeRegionIndex;
					int16_t Unknown;
					int16_t UnknownSpecialDamage;
					Text::StringID SpecialDamageCase;
					Text::StringID EffectMarkerName;
					Text::StringID DamageEffectMarkerName;
					float ResponseDelay;
					TagReference DelayEffect;
					Text::StringID DelayEffectMarkerName;
					Text::StringID EjectingSeatLabel;
					float SkipFraction;
					Text::StringID DestroyedChildObjectMarkerName;
					float TotalDamageThreshold;
				};
				TAG_STRUCT_SIZE_ASSERT(InstantRespons, 0x88);
			};
			TAG_STRUCT_SIZE_ASSERT(DamageSection, 0x44);

			struct Node
			{
				int16_t Unknown;
				int16_t Unknown2;
				uint32_t Unknown3;
				uint32_t Unknown4;
				uint32_t Unknown5;
			};
			TAG_STRUCT_SIZE_ASSERT(Node, 0x10);

			struct DamageSeat
			{
				struct Unknown;

				Text::StringID SeatLabel;
				float DirectDamageScale;
				float DamageTransferFallOffRadius;
				float MaximumTransferDamageScale;
				float MinimumTransferDamageScale;
				TagBlock<Unknown> Unknown2;

				struct Unknown
				{
					Text::StringID Node;
					uint32_t Unknown1;
					uint32_t Unknown2;
					uint32_t Unknown3;
					uint32_t Unknown4;
					uint32_t Unknown5;
					uint32_t Unknown6;
					uint32_t Unknown7;
					uint32_t Unknown8;
					uint32_t Unknown9;
					uint32_t Unknown10;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown, 0x2C);
			};
			TAG_STRUCT_SIZE_ASSERT(DamageSeat, 0x20);

			struct DamageConstraint
			{
				Text::StringID PhysicsModelConstraintName;
				Text::StringID DamageConstraintName;
				Text::StringID DamageConstraintGroupName;
				float GroupProbabilityScale;
				int16_t Type;
				int16_t Index;
			};
			TAG_STRUCT_SIZE_ASSERT(DamageConstraint, 0x14);
		};
		TAG_STRUCT_SIZE_ASSERT(NewDamageInfoBlock, 0x100);

		struct Target
		{
			uint32_t Unknown;
			Text::StringID MarkerName;
			float Size;
			float ConeAngle;
			int16_t DamageSection;
			int16_t Variant;
			float TargetingRelevance;
			uint32_t Unknown2;
			int32_t Flags;
			float LockOnDistance;
			Text::StringID TargetFilter;
		};
		TAG_STRUCT_SIZE_ASSERT(Target, 0x28);

		struct CollisionRegion
		{
			struct Permutation;

			Text::StringID Name;
			int8_t CollisionRegionIndex;
			int8_t PhysicsRegionIndex;
			int8_t Unknown;
			int8_t Unknown2;
			TagBlock<Permutation> Permutations;

			struct Permutation
			{
				Text::StringID Name;
				uint8_t Flags;
				int8_t CollisionPermutationIndex;
				int8_t PhysicsPermutationIndex;
				int8_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(Permutation, 0x8);
		};
		TAG_STRUCT_SIZE_ASSERT(CollisionRegion, 0x14);

		struct Node
		{
			Text::StringID Name;
			int16_t ParentNode;
			int16_t FirstChildNode;
			int16_t NextSiblingNode;
			int16_t ImportNodeIndex;
			float DefaultTranslationX;
			float DefaultTranslationY;
			float DefaultTranslationZ;
			float DefaultRotationI;
			float DefaultRotationJ;
			float DefaultRotationK;
			float DefaultRotationW;
			float DefaultScale;
			float InverseForwardI;
			float InverseForwardJ;
			float InverseForwardK;
			float InverseLeftI;
			float InverseLeftJ;
			float InverseLeftK;
			float InverseUpI;
			float InverseUpJ;
			float InverseUpK;
			float InversePositionX;
			float InversePositionY;
			float InversePositionZ;
		};
		TAG_STRUCT_SIZE_ASSERT(Node, 0x5C);

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

		struct Unknown6
		{
			Text::StringID Region;
			Text::StringID Permutation;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown6, 0x8);

		struct Unknown8
		{
			Text::StringID Unknown;
			uint32_t Unknown2;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown8, 0x8);

		struct Unknown10
		{
			Text::StringID Marker;
			uint32_t Unknown;
			Text::StringID Marker2;
			uint32_t Unknown2;
			uint32_t Unknown3;
		};
		TAG_STRUCT_SIZE_ASSERT(Unknown10, 0x14);
	};
	TAG_STRUCT_SIZE_ASSERT(Model, 0x1B4);
}