#pragma once
#include "../../Math/Bounds.hpp"
#include "../../Math/RealColorRGB.hpp"
#include "../../Math/RealPoint3D.hpp"
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::Objects
{
	using Blam::Math::Bounds;
	using Blam::Math::RealColorRGB;
	using Blam::Math::RealPoint3D;
	using Blam::Tags::Tag;
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;

	struct Object : TagGroup<'obje'>
	{
		enum class Type : int16_t;
		enum class Flags : uint16_t;
		enum class LightmapShadowModeSize : int16_t;
		enum class SweetenerSize : int8_t;
		enum class WaterDensity : int8_t;
		struct EarlyMoverProperty;
		struct AIProperty;
		struct Function;
		struct Attachment;
		struct Widget;
		struct ChangeColor;
		struct NodeMap;
		struct MultiplayerProperty;
		struct ModelObjectDatum;

		Object::Type ObjectType : 16;
		Object::Flags ObjectFlags : 16;
		float BoundingRadius;
		RealPoint3D BoundingOffset;
		float AccelerationScale;
		Object::LightmapShadowModeSize LightmapShadowModeSize : 16;
		Object::SweetenerSize SweetenerSize : 8;
		Object::WaterDensity WaterDensity : 8;
		int32_t Unknown1;
		float DynamicLightSphereRadius;
		RealPoint3D DynamicLightSphereOffset;
		int32_t DefaultModelVariant;
		TagReference Model;
		TagReference CrateObject;
		TagReference CollisionDamage;
		TagBlock<Object::EarlyMoverProperty> EarlyMoverProperties;
		TagReference CreationEffect;
		TagReference MaterialEffects;
		TagReference ArmorSounds;
		TagReference MeleeImpact;
		TagBlock<Object::AIProperty> AIProperties;
		TagBlock<Object::Function> Functions;
		int16_t HUDTextMessageIndex;
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

		enum class Object::Type : int16_t
		{
			Biped,
			Vehicle,
			Weapon,
			Equipment,
			ARGDevice,
			Terminal,
			Projectile,
			Scenery,
			Control,
			SoundScenery,
			Crate,
			Creature,
			Giant,
			EffectScenery
		};

		enum class Object::Flags : uint16_t
		{
			None,
			DoesNotCastShadow = 1 << 0,
			SearchCardinalDirectionLightmaps = 1 << 1,
			NotAPathfindingObstacle = 1 << 3,
			ExtensionOfParent = 1 << 4,
			DoesNotCauseCollisionDamage = 1 << 5,
			EarlyMover = 1 << 6,
			EarlyMoverLocalizedPhysics = 1 << 7,
			UseStaticMassiveLightmapSample = 1 << 8,
			ObjectScalesAttachments = 1 << 9,
			InheritsPlayersAppearance = 1 << 10,
			DeadBipedsCannotLocalize = 1 << 11,
			AttachToClustersByDynamicSphere = 1 << 12,
			EffectDoNotSpawnObjectsInMP = 1 << 13
		};

		struct Object::EarlyMoverProperty
		{
			int32_t Name;
			uint32_t Unknown1;
			uint32_t Unknown2;
			uint32_t Unknown3;
			uint32_t Unknown4;
			uint32_t Unknown5;
			uint32_t Unknown6;
			uint32_t Unknown7;
			uint32_t Unknown8;
			uint32_t Unknown9;
		};
		TAG_STRUCT_SIZE_ASSERT(Object::EarlyMoverProperty, 0x28);

		struct Object::AIProperty
		{
			enum class Flags : int32_t;
			enum class Size : int16_t;
			enum class LeapJumpSpeed : int16_t;

			Object::AIProperty::Flags Flags : 32;
			StringID AITypeName;
			Object::AIProperty::Size Size : 16;
			Object::AIProperty::LeapJumpSpeed LeapJumpSpeed : 16;

			enum class Object::AIProperty::Flags : int32_t
			{
				None,
				DestroyableCover = 1 << 0,
				PathfindingIgnoreWhenDead = 1 << 1,
				DynamicCover = 1 << 2
			};

			enum class Object::AIProperty::Size : int16_t
			{
				Default,
				Tiny,
				Small,
				Medium,
				Large,
				Huge,
				Immobile
			};

			enum class Object::AIProperty::LeapJumpSpeed : int16_t
			{
				None,
				Down,
				Step,
				Crouch,
				Stand,
				Storey,
				Tower,
				Infinite
			};
		};
		TAG_STRUCT_SIZE_ASSERT(Object::AIProperty, 0xC);

		struct Object::Function
		{
			enum class Flags : int32_t;

			Object::Function::Flags Flags : 32;
			StringID ImportName;
			StringID ExportName;
			StringID TurnOffWith;
			float MinimumValue;
			TagData<uint8_t> DefaultFunction;
			StringID ScaleBy;

			enum class Object::Function::Flags : int32_t
			{
				None,
				Invert = 1 << 0,
				MappingDoesNotControlsActive = 1 << 1,
				AlwaysActive = 1 << 2,
				RandomTimeOffset = 1 << 3
			};
		};
		TAG_STRUCT_SIZE_ASSERT(Object::Function, 0x2C);

		struct Attachment
		{
			enum class Flags : int32_t;
			enum class ChangeColor : int16_t;

			Object::Attachment::Flags Flags : 32;
			TagReference Attached;
			StringID Marker;
			Object::Attachment::ChangeColor ChangeColor : 16;
			PAD16;
			StringID PrimaryScale;
			StringID SecondaryScale;

			enum class Object::Attachment::Flags : int32_t
			{
				None,
				GameplayVisionMode = 1 << 0,
				TheaterVisionMode = 1 << 1
			};

			enum class Object::Attachment::ChangeColor : int16_t
			{
				None,
				Primary,
				Secondary,
				Tertiary,
				Quaternary
			};
		};
		TAG_STRUCT_SIZE_ASSERT(Object::Attachment, 0x24);

		struct Object::Widget
		{
			TagReference Type;
		};
		TAG_STRUCT_SIZE_ASSERT(Object::Widget, 0x10);

		struct Object::ChangeColor
		{
			struct InitialPermutation;
			struct Function;

			TagBlock<Object::ChangeColor::InitialPermutation> InitialPermutations;
			TagBlock<Object::ChangeColor::Function> Functions;

			struct Object::ChangeColor::InitialPermutation
			{
				uint32_t Weight;
				Bounds<RealColorRGB> ColorBounds;
				StringID VariantName;
			};
			TAG_STRUCT_SIZE_ASSERT(Object::ChangeColor::InitialPermutation, 0x20);

			struct Object::ChangeColor::Function
			{
				enum class ScaleFlags : int32_t;

				Object::ChangeColor::Function::ScaleFlags ScaleFlags : 32;
				Bounds<RealColorRGB> ColorBounds;
				StringID DarkenBy;
				StringID ScaleBy;

				enum class Object::ChangeColor::Function::ScaleFlags : int32_t
				{
					None,
					BlendInHSV = 1 << 0,
					MoreColors = 1 << 1
				};
			};
			TAG_STRUCT_SIZE_ASSERT(Object::ChangeColor::Function, 0x24);
		};
		TAG_STRUCT_SIZE_ASSERT(Object::ChangeColor, 0x18);

		struct Object::NodeMap
		{
			int8_t TargetNode;
		};
		TAG_STRUCT_SIZE_ASSERT(Object::NodeMap, 0x1);

		struct Object::MultiplayerProperty
		{
			enum class EngineFlags : uint16_t;
			enum class ObjectType : int8_t;
			enum class TeleporterFlags : uint8_t;
			enum class Flags : uint16_t;
			enum class Shape : int8_t;
			enum class SpawnTimerMode : int8_t;

			Object::MultiplayerProperty::EngineFlags EngineFlags : 16;
			Object::MultiplayerProperty::ObjectType ObjectType : 8;
			Object::MultiplayerProperty::TeleporterFlags TeleporterFlags : 8;
			Object::MultiplayerProperty::Flags Flags : 16;
			Object::MultiplayerProperty::Shape Shape : 8;
			Object::MultiplayerProperty::SpawnTimerMode SpawnTimerMode;
			int16_t SpawnTime;
			int16_t AbandonTime;
			float RadiusWidth;
			float Length;
			float Top;
			float Bottom;
			float Unknown1;
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

			enum class Object::MultiplayerProperty::EngineFlags : uint16_t
			{
				None,
				CTF = 1 << 0,
				Slayer = 1 << 1,
				Oddball = 1 << 2,
				KOTH = 1 << 3,
				Juggernaut = 1 << 4,
				Territories = 1 << 5,
				Assault = 1 << 6,
				VIP = 1 << 7,
				Infection = 1 << 8
			};

			enum class Object::MultiplayerProperty::ObjectType : int8_t
			{
				Ordinary,
				Weapon,
				Grenade,
				Projectile,
				Powerup,
				Equipment,
				LightLandVehicle,
				HeavyLandVehicle,
				FlyingVehicle,
				TeleporterTwoWay,
				TeleporterSender,
				TeleporterReceiver,
				PlayerSpawnLocation,
				PlayerRespawnZone,
				HoldSpawnObjective,
				CaptureSpawnObjective,
				HoldDestinationObjective,
				CaptureDestinationObjective,
				HillObjective,
				InfectionHavenObjective,
				TerritoryObjective,
				VIPBoundaryObjective,
				VIPDestinationObjective,
				JuggernautDestinationObjective
			};

			enum class Object::MultiplayerProperty::TeleporterFlags : uint8_t
			{
				None,
				DisallowsPlayers = 1 << 0,
				AllowsLandVehicles = 1 << 1,
				AllowsHeavyVehicles = 1 << 2,
				AllowsFlyingVehicles = 1 << 3,
				AllowsProjectiles = 1 << 4
			};

			enum class Object::MultiplayerProperty::Flags : uint16_t
			{
				None,
				EditorOnly = 1 << 0
			};

			enum class Object::MultiplayerProperty::Shape : int8_t
			{
				None,
				Sphere,
				Cylinder,
				Box
			};

			enum class Object::MultiplayerProperty::SpawnTimerMode : int8_t
			{
				OnDeath,
				OnDisturbance
			};
		};
		TAG_STRUCT_SIZE_ASSERT(Object::MultiplayerProperty, 0xC4);

		struct Object::ModelObjectDatum
		{
			enum class Type : int16_t;

			Object::ModelObjectDatum::Type Type;
			PAD16;
			RealPoint3D Offset;
			float Radius;

			enum class Object::ModelObjectDatum::Type : int16_t
			{
				NotSet,
				UserDefined,
				AutoGenerated
			};
		};
		TAG_STRUCT_SIZE_ASSERT(Object::ModelObjectDatum, 0x14);
	};
	TAG_STRUCT_SIZE_ASSERT(Object, 0x120);
}
