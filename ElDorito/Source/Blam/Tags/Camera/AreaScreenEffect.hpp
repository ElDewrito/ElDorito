#pragma once
#include "..\Tags.hpp"
#include "../../Text/StringID.hpp"
#include "../../Math/RealColorRGB.hpp"

namespace Blam::Tags::Camera
{
	struct AreaScreenEffect : TagGroup<'sefc'>
	{
		struct ScreenEffect;

		TagBlock<ScreenEffect> ScreenEffects;

		enum FlagsValue : int16_t;
		enum HiddenFlagsValue : int16_t;

		struct ScreenEffect
		{
			Text::StringID Name;
			FlagsValue Flags;
			HiddenFlagsValue HiddenFlags;
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

		enum FlagsValue : int16_t
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

		enum HiddenFlagsValue : int16_t
		{
			NoneHidden = 0,
			UpdateThread = 1 << 0,
			RenderThread = 1 << 1
		};
	};
	TAG_STRUCT_SIZE_ASSERT(AreaScreenEffect, 0xC);
}
