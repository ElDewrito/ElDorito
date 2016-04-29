#pragma once
#include "Tags.hpp"
#include "../Math/RealColorRGB.hpp"
#include "../Math/RealPoint3D.hpp"

namespace Blam
{
	namespace Tags
	{
		using Blam::Math::RealColorRGB;
		using Blam::Math::RealPoint3D;

		struct Object : Tag<'obje'>
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

			int16_t ObjectType;
			uint16_t Flags;
			float BoundingRadius;
			RealPoint3D BoundingOffset;
			float AccelerationScale;
			int16_t LightmapShadowModeSize;
			int8_t SweetenerSize;
			int8_t WaterDensity;
			int32_t Unknown;
			float DynamicLightSphereRadius;
			RealPoint3D DynamicLightSphereOffset;
			int32_t DefaultModelVariant;
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

			struct EarlyMoverProperty
			{
				int32_t Name;
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
				uint32_t Flags;
				int32_t AiTypeName;
				int16_t Size;
				int16_t LeapJumpSpeed;
			};
			TAG_STRUCT_SIZE_ASSERT(AiProperty, 0xC);

			struct Function
			{
				uint32_t Flags;
				int32_t ImportName;
				int32_t ExportName;
				int32_t TurnOffWith;
				float MinimumValue;
				DataReference<uint8_t> DefaultFunction;
				int32_t ScaleBy;
			};
			TAG_STRUCT_SIZE_ASSERT(Function, 0x2C);

			struct Attachment
			{
				uint32_t AtlasFlags;
				TagReference Attached;
				int32_t Marker;
				int16_t ChangeColor;
				int16_t Unknown;
				int32_t PrimaryScale;
				int32_t SecondaryScale;
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
					RealColorRGB LowerBoundColor;
					RealColorRGB UpperBoundColor;
					int32_t VariantName;
				};
				TAG_STRUCT_SIZE_ASSERT(InitialPermutation, 0x20);

				struct Function
				{
					uint32_t ScaleFlags;
					RealColorRGB LowerBoundColor;
					RealColorRGB UpperBoundColor;
					int32_t DarkenBy;
					int32_t ScaleBy;
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
				RealPoint3D Offset;
				float Radius;
			};
			TAG_STRUCT_SIZE_ASSERT(ModelObjectDatum, 0x14);
		};
		TAG_STRUCT_SIZE_ASSERT(Object, 0x120);
	}
}
