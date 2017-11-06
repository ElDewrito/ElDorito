#pragma once
#include "..\Tags.hpp"
#include "../../Text/StringID.hpp"
#include "../../Math/RealColorRGB.hpp"

namespace Blam
{
	namespace Tags
	{
		struct AreaScreenEffect : TagGroup<'sefc'>
		{
			enum ScreenEffectFlags : int16_t
			{
				None = 0,
				DebugDisable = 1 << 0,
				AllowEffectOutsideRadius = 1 << 1,
				Unattached = 1 << 2,
				FirstPersonOnly = 1 << 3,
				ThirdPersonOnly = 1 << 4,
				DisableCinematicCameraFalloffs = 1 << 5,
				OnlyAffectsAttachedObject = 1 << 6,
				DrawPreciselyOne = 1 << 7,
				UsePlayerTravelDirection = 1 << 8
			};

			enum ScreenEffectHiddenFlags : int16_t
			{
				NoneHidden = 0,
				UpdateThread = 1 << 0,
				RenderThread = 1 << 1
			};

			struct ScreenEffect;

			TagBlock<ScreenEffect> ScreenEffect2;

			struct ScreenEffect
			{
				Text::StringID Name;
				ScreenEffectFlags Flags;
				ScreenEffectHiddenFlags HiddenFlags;
				float MaximumDistance;
				TagData<uint8_t> DistanceFalloffFunction;
				float Duration;
				TagData<uint8_t> TimeEvolutionFunction;
				TagData<uint8_t> AngleFalloffFunction;
				float LightIntensity;
				uint32_t PrimaryHue;
				uint32_t SecondaryHue;
				float Saturation;
				float Desaturation;
				float GammaIncrease;
				float GammaDecrease;
				float ShadowBrightness;
				Math::RealColorRGB ColorFilter;
				Math::RealColorRGB ColorFloor;
				float Tracing;
				float Turbulance;
				TagReference ScreenShader;
			};
			TAG_STRUCT_SIZE_ASSERT(ScreenEffect, 0x9C);
		};
		TAG_STRUCT_SIZE_ASSERT(AreaScreenEffect, 0xC);
	}
}
