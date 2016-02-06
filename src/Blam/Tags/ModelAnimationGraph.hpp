#pragma once
#include "Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		struct ModelAnimationGraph : Tag<'jmad'>
		{
			struct SkeletonNode;
			struct SoundReference;
			struct EffectReference;
			struct BlendScreen;
			struct Leg;
			struct Animation;
			struct Mode;
			struct VehicleSuspension;
			struct ObjectOverlay;
			struct InheritanceList;
			struct WeaponList;
			struct ResourceGroup;

			TagReference ParentAnimationGraph;
			uint8_t InheritanceFlags;
			uint8_t PrivateFlags;
			int16_t AnimationCodecPack;
			TagBlock<SkeletonNode> SkeletonNodes;
			TagBlock<SoundReference> SoundReferences;
			TagBlock<EffectReference> EffectReferences;
			TagBlock<BlendScreen> BlendScreens;
			TagBlock<Leg> Legs;
			TagBlock<Animation> Animations;
			TagBlock<Mode> Modes;
			TagBlock<VehicleSuspension> VehicleSuspension2;
			TagBlock<ObjectOverlay> ObjectOverlays;
			TagBlock<InheritanceList> InheritanceList2;
			TagBlock<WeaponList> WeaponList2;
			uint32_t UnknownArmNodes1;
			uint32_t UnknownArmNodes2;
			uint32_t UnknownArmNodes3;
			uint32_t UnknownArmNodes4;
			uint32_t UnknownArmNodes5;
			uint32_t UnknownArmNodes6;
			uint32_t UnknownArmNodes7;
			uint32_t UnknownArmNodes8;
			uint32_t UnknownNodes1;
			uint32_t UnknownNodes2;
			uint32_t UnknownNodes3;
			uint32_t UnknownNodes4;
			uint32_t UnknownNodes5;
			uint32_t UnknownNodes6;
			uint32_t UnknownNodes7;
			uint32_t UnknownNodes8;
			DataReference<uint8_t> LastImportResults;
			uint32_t Unknown;
			uint32_t Unknown2;
			uint32_t Unknown3;
			TagBlock<ResourceGroup> ResourceGroups;

			struct SkeletonNode
			{
				int32_t Name;
				int16_t NextSiblingNodeIndex;
				int16_t FirstChildNodeIndex;
				int16_t ParentNodeIndex;
				uint8_t ModelFlags;
				uint8_t NodeJointFlags;
				float BaseVectorI;
				float BaseVectorJ;
				float BaseVectorK;
				float VectorRange;
				float ZPosition;
			};
			TAG_STRUCT_SIZE_ASSERT(SkeletonNode, 0x20);

			struct SoundReference
			{
				TagReference Sound;
				uint16_t Flags;
				int16_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(SoundReference, 0x14);

			struct EffectReference
			{
				TagReference Effect;
				uint16_t Flags;
				int16_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(EffectReference, 0x14);

			struct BlendScreen
			{
				int32_t Label;
				float RightYawPerFrame;
				float LeftYawPerFrame;
				int16_t RightFrameCount;
				int16_t LeftFrameCount;
				float DownPitchPerFrame;
				float UpPitchPerFrame;
				int16_t DownPitchFrameCount;
				int16_t UpPitchFrameCount;
			};
			TAG_STRUCT_SIZE_ASSERT(BlendScreen, 0x1C);

			struct Leg
			{
				int32_t FootMarker;
				float FootMin;
				float FootMax;
				int32_t AnkleMarker;
				float AnkleMin;
				float AnkleMax;
				int16_t Anchors;
				int16_t Unknown;
			};
			TAG_STRUCT_SIZE_ASSERT(Leg, 0x1C);

			struct Animation
			{
				struct FrameEvent;
				struct SoundEvent;
				struct EffectEvent;
				struct Unknown3;
				struct ObjectSpaceParentNode;
				struct LegAnchoring;

				int32_t Name;
				float Weight;
				int16_t LoopFrameIndex;
				uint16_t PlaybackFlags;
				int8_t BlendScreen;
				int8_t DesiredCompression;
				int8_t CurrentCompression;
				int8_t NodeCount;
				int16_t FrameCount;
				int8_t Type;
				int8_t FrameInfoType;
				uint16_t ProductionFlags;
				uint16_t InternalFlags;
				int32_t NodeListChecksum;
				int32_t ProductionChecksum;
				int16_t Unknown;
				int16_t Unknown2;
				int16_t PreviousVariantSibling;
				int16_t NextVariantSibling;
				int16_t RawInformationGroupIndex;
				int16_t RawInformationMemberIndex;
				TagBlock<FrameEvent> FrameEvents;
				TagBlock<SoundEvent> SoundEvents;
				TagBlock<EffectEvent> EffectEvents;
				TagBlock<Unknown3> Unknown4;
				TagBlock<ObjectSpaceParentNode> ObjectSpaceParentNodes;
				TagBlock<LegAnchoring> LegAnchoring2;
				float Unknown5;
				float Unknown6;
				float Unknown7;
				float Unknown8;
				float Unknown9;

				struct FrameEvent
				{
					int16_t Type;
					int16_t Frame;
				};
				TAG_STRUCT_SIZE_ASSERT(FrameEvent, 0x4);

				struct SoundEvent
				{
					int16_t Sound;
					int16_t Frame;
					int32_t MarkerName;
				};
				TAG_STRUCT_SIZE_ASSERT(SoundEvent, 0x8);

				struct EffectEvent
				{
					int16_t Effect;
					int16_t Frame;
					int32_t MarkerName;
				};
				TAG_STRUCT_SIZE_ASSERT(EffectEvent, 0x8);

				struct Unknown3
				{
					int16_t Unknown;
					int16_t Unknown2;
				};
				TAG_STRUCT_SIZE_ASSERT(Unknown3, 0x4);

				struct ObjectSpaceParentNode
				{
					int16_t NodeIndex;
					uint16_t ComponentFlags;
					int16_t RotationX;
					int16_t RotationY;
					int16_t RotationZ;
					int16_t RotationW;
					float DefaultTranslationX;
					float DefaultTranslationY;
					float DefaultTranslationZ;
					float DefaultScale;
				};
				TAG_STRUCT_SIZE_ASSERT(ObjectSpaceParentNode, 0x1C);

				struct LegAnchoring
				{
					struct TagBlock2;

					int16_t LegIndex;
					int16_t Unknown;
					TagBlock<TagBlock2> Unknown3;

					struct TagBlock2
					{
						int16_t Frame1a;
						int16_t Frame2a;
						int16_t Frame1b;
						int16_t Frame2b;
						uint32_t Unknown;
						uint32_t Unknown2;
						uint32_t Unknown3;
					};
					TAG_STRUCT_SIZE_ASSERT(TagBlock2, 0x14);
				};
				TAG_STRUCT_SIZE_ASSERT(LegAnchoring, 0x10);
			};
			TAG_STRUCT_SIZE_ASSERT(Animation, 0x88);

			struct Mode
			{
				struct WeaponClass;
				struct ModeIk;

				int32_t Label;
				TagBlock<WeaponClass> WeaponClass2;
				TagBlock<ModeIk> ModeIk2;
				uint32_t Unknown;
				uint32_t Unknown2;
				uint32_t Unknown3;

				struct WeaponClass
				{
					struct WeaponType;
					struct WeaponIk;
					struct SyncAction;

					int32_t Label;
					TagBlock<WeaponType> WeaponType2;
					TagBlock<WeaponIk> WeaponIk2;
					TagBlock<SyncAction> SyncActions;

					struct WeaponType
					{
						struct Action;
						struct Overlay;
						struct DeathAndDamage;
						struct Transition;

						int32_t Label;
						TagBlock<Action> Actions;
						TagBlock<Overlay> Overlays;
						TagBlock<DeathAndDamage> DeathAndDamage2;
						TagBlock<Transition> Transitions;

						struct Action
						{
							int32_t Label;
							int16_t GraphIndex;
							int16_t Animation;
						};
						TAG_STRUCT_SIZE_ASSERT(Action, 0x8);

						struct Overlay
						{
							int32_t Label;
							int16_t GraphIndex;
							int16_t Animation;
						};
						TAG_STRUCT_SIZE_ASSERT(Overlay, 0x8);

						struct DeathAndDamage
						{
							struct Direction;

							int32_t Label;
							TagBlock<Direction> Directions;

							struct Direction
							{
								struct Region;

								TagBlock<Region> Regions;

								struct Region
								{
									int16_t GraphIndex;
									int16_t Animation;
								};
								TAG_STRUCT_SIZE_ASSERT(Region, 0x4);
							};
							TAG_STRUCT_SIZE_ASSERT(Direction, 0xC);
						};
						TAG_STRUCT_SIZE_ASSERT(DeathAndDamage, 0x10);

						struct Transition
						{
							struct Destination;

							int32_t FullName;
							int32_t StateName;
							int16_t Unknown;
							int8_t IndexA;
							int8_t IndexB;
							TagBlock<Destination> Destinations;

							struct Destination
							{
								int32_t FullName;
								int32_t ModeName;
								int32_t StateName;
								int8_t FrameEventLink;
								int8_t Unknown;
								int8_t IndexA;
								int8_t IndexB;
								int16_t GraphIndex;
								int16_t Animation;
							};
							TAG_STRUCT_SIZE_ASSERT(Destination, 0x14);
						};
						TAG_STRUCT_SIZE_ASSERT(Transition, 0x18);
					};
					TAG_STRUCT_SIZE_ASSERT(WeaponType, 0x34);

					struct WeaponIk
					{
						int32_t Marker;
						int32_t AttachToMarker;
					};
					TAG_STRUCT_SIZE_ASSERT(WeaponIk, 0x8);

					struct SyncAction
					{
						struct Class;

						int32_t Label;
						TagBlock<Class> Class2;

						struct Class
						{
							struct TagBlock3;
							struct SyncBiped;

							int32_t Label;
							TagBlock<TagBlock3> Unknown2;
							TagBlock<SyncBiped> SyncBiped2;

							struct TagBlock3
							{
								int32_t Unknown;
								int16_t GraphIndex;
								int16_t Animation;
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
							};
							TAG_STRUCT_SIZE_ASSERT(TagBlock3, 0x30);

							struct SyncBiped
							{
								int32_t Unknown;
								TagReference Biped;
							};
							TAG_STRUCT_SIZE_ASSERT(SyncBiped, 0x14);
						};
						TAG_STRUCT_SIZE_ASSERT(Class, 0x1C);
					};
					TAG_STRUCT_SIZE_ASSERT(SyncAction, 0x10);
				};
				TAG_STRUCT_SIZE_ASSERT(WeaponClass, 0x28);

				struct ModeIk
				{
					int32_t Marker;
					int32_t AttachToMarker;
				};
				TAG_STRUCT_SIZE_ASSERT(ModeIk, 0x8);
			};
			TAG_STRUCT_SIZE_ASSERT(Mode, 0x28);

			struct VehicleSuspension
			{
				int32_t Label;
				int16_t GraphIndex;
				int16_t Animation;
				int32_t MarkerName;
				float MassPointOffset;
				float FullExtensionGroundDepth;
				float FullCompressionGroundDepth;
				int32_t RegionName;
				float MassPointOffset2;
				float ExpressionGroundDepth;
				float CompressionGroundDepth;
			};
			TAG_STRUCT_SIZE_ASSERT(VehicleSuspension, 0x28);

			struct ObjectOverlay
			{
				int32_t Label;
				int16_t GraphIndex;
				int16_t Animation;
				int16_t Unknown;
				int16_t FunctionControls;
				int32_t Function;
				uint32_t Unknown2;
			};
			TAG_STRUCT_SIZE_ASSERT(ObjectOverlay, 0x14);

			struct InheritanceList
			{
				struct NodeMap;
				struct NodeMapFlag;

				TagReference InheritedGraph;
				TagBlock<NodeMap> NodeMap2;
				TagBlock<NodeMapFlag> NodeMapFlags;
				float RootZOffset;
				uint32_t InheritanceFlags;

				struct NodeMap
				{
					int16_t LocalNode;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeMap, 0x2);

				struct NodeMapFlag
				{
					uint32_t LocalNodeFlags;
				};
				TAG_STRUCT_SIZE_ASSERT(NodeMapFlag, 0x4);
			};
			TAG_STRUCT_SIZE_ASSERT(InheritanceList, 0x30);

			struct WeaponList
			{
				int32_t WeaponName;
				int32_t WeaponClass;
			};
			TAG_STRUCT_SIZE_ASSERT(WeaponList, 0x8);

			struct ResourceGroup
			{
				int32_t MemberCount;
				void* Resource;
				int32_t UselessPadding;
			};
			TAG_STRUCT_SIZE_ASSERT(ResourceGroup, 0xC);
		};
		TAG_STRUCT_SIZE_ASSERT(ModelAnimationGraph, 0x104);
	}
}
