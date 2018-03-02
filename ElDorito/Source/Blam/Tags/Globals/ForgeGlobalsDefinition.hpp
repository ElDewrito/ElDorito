#pragma once

#include "../Tags.hpp"
#include "../../Math/RealVector3D.hpp"

namespace Blam::Tags::Globals
{
	using Blam::Math::RealVector3D;

	struct ForgeGlobalsDefinition : Blam::Tags::TagGroup<'forg'>
	{
		struct ReForgeMaterial;
		enum class PaletteItemCategory : short;
		struct PaletteItem;
		struct WeatherEffect;
		struct Sky;

		TagReference InvisibleRenderMethod;
		TagReference DefaultRenderMethod;
		TagBlock<ReForgeMaterial> ReForgeMaterials;
		TagReference PrematchCameraObject;
		TagReference ModifierObject;
		TagReference KillVolumeObject;
		TagReference GarbageVolumeObject;
		TagBlock<PaletteItem> Palette;
		TagBlock<WeatherEffect> WeatherEffects;
		TagBlock<Sky> Skies;

		struct ReForgeMaterial
		{
			char Name[32];
			TagReference RenderMethod;
		};
		static_assert(sizeof(ForgeGlobalsDefinition::ReForgeMaterial) == 0x30);

		enum class PaletteItemCategory : short
		{
			Tool,
			Prop,
			Light,
			Effects,
			Structure,
			Equipment,
			Weapon,
			Vehicle,
			Teleporter,
			Game,
			Assault,
			CaptureTheFlag,
			Infection,
			Juggernaut,
			KingOfTheHill,
			Territories,
			Slayer,
			VIP
		};
		static_assert(sizeof(ForgeGlobalsDefinition::PaletteItemCategory) == 0x2);

		struct PaletteItem
		{
			enum class SetterType : short;
			enum class SetterFlags : unsigned char;
			struct Setter;

			char Name[32];
			PaletteItemCategory Category : 16;
			unsigned short MaxAllowed;
			TagReference Object;
			TagBlock<Setter> Setters;

			enum class SetterType : short
			{
				Boolean,
				Integer,
				Real
			};
			static_assert(sizeof(ForgeGlobalsDefinition::PaletteItem::SetterType) == 0x2);

			enum class SetterFlags : unsigned char
			{
				None,
				Hidden = 1 << 0
			};
			static_assert(sizeof(ForgeGlobalsDefinition::PaletteItem::SetterFlags) == 0x1);

			struct Setter
			{
				char Target[32];
				SetterType Type : 16;
				SetterFlags Flags : 8;
				bool BooleanValue;
				long IntegerValue;
				float RealValue;
			};
			static_assert(sizeof(ForgeGlobalsDefinition::PaletteItem::Setter) == 0x2C);
		};
		static_assert(sizeof(ForgeGlobalsDefinition::PaletteItem) == 0x40);

		struct WeatherEffect
		{
			char Name[32];
			TagReference Effect;
		};
		static_assert(sizeof(ForgeGlobalsDefinition::WeatherEffect) == 0x30);

		struct Sky
		{
			char Name[32];
			uint32_t Flags;
			RealVector3D Translation;
			RealVector3D Orientation;
			TagReference Object;
			TagReference Parameters;
			TagReference Wind;
			TagReference CameraFX;
			TagReference ScreenFX;
			TagReference GlobalLighting;
			TagReference BackgroundSound;
		};
		static_assert(sizeof(ForgeGlobalsDefinition::Sky) == 0xAC);
	};
	static_assert(sizeof(ForgeGlobalsDefinition) == 0x90);
}
