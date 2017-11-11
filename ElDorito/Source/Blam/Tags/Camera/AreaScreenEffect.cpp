#include "Blam\Tags\Camera\AreaScreenEffect.hpp"
#include "Definitions\EnumDefinition.hpp"

using namespace Blam::Tags::Camera;
using namespace Definitions;

namespace Blam::Tags
{
	const EnumDefinition FlagsValuesEnum =
	{
		"FlagsValue", EnumType::Short,
		{
			{ "DebugDisable", (short)AreaScreenEffect::FlagsValue::DebugDisable },
			{ "AllowEffectOutsideRadius", (short)AreaScreenEffect::FlagsValue::AllowEffectOutsideRadius },
			{ "Unattached", (short)AreaScreenEffect::FlagsValue::Unattached },
			{ "FirstPersonOnly", (short)AreaScreenEffect::FlagsValue::FirstPersonOnly },
			{ "ThirdPersonOnly", (short)AreaScreenEffect::FlagsValue::ThirdPersonOnly },
			{ "DisableCinematicCameraFalloffs", (short)AreaScreenEffect::FlagsValue::DisableCinematicCameraFalloffs },
			{ "OnlyAffectsAttachedObject", (short)AreaScreenEffect::FlagsValue::OnlyAffectsAttachedObject },
			{ "DrawPreciselyOne", (short)AreaScreenEffect::FlagsValue::DrawPreciselyOne },
			{ "UsePlayerTravelDirection", (short)AreaScreenEffect::FlagsValue::UsePlayerTravelDirection }
		}
	};

	const EnumDefinition HiddenFlagsValuesEnum =
	{
		"HiddenFlagsValue", EnumType::Short,
		{
			{ "NoneHidden", (short)AreaScreenEffect::HiddenFlagsValue::NoneHidden },
			{ "UpdateThread", (short)AreaScreenEffect::HiddenFlagsValue::UpdateThread },
			{ "RenderThread", (short)AreaScreenEffect::HiddenFlagsValue::RenderThread }
		}
	};

	const StructDefinition ScreenEffectsStruct =
	{
		"ScreenEffect", sizeof(AreaScreenEffect::ScreenEffect),
		{
			{ FieldType::StringID, "Name" },
			{ FieldType::ShortEnum, "Flags", &FlagsValuesEnum },
			{ FieldType::ShortEnum, "HiddenFlags", &HiddenFlagsValuesEnum },
			{ FieldType::Real, "MaximumDistance" },
			{ FieldType::Data, "DistanceFalloffFunction" },
			{ FieldType::Real, "Duration" },
			{ FieldType::Data, "TimeEvolutionFunction" },
			{ FieldType::Data, "AngleFalloffFunction" },
			{ FieldType::Real, "LightIntensity" },
			{ FieldType::Block, "PrimaryHue" },
			{ FieldType::Block, "SecondaryHue" },
			{ FieldType::Real, "Saturation" },
			{ FieldType::Real, "Desaturation" },
			{ FieldType::Real, "GammaIncrease" },
			{ FieldType::Real, "GammaDecrease" },
			{ FieldType::Real, "ShadowBrightness" },
			{ FieldType::RealRgbColor, "ColorFilter" },
			{ FieldType::RealRgbColor, "ColorFloor" },
			{ FieldType::Real, "Tracing" },
			{ FieldType::Real, "Turbulance" },
			{ FieldType::TagReference, "ScreenShader" }
		}
	};

	const StructDefinition TagGroup<AreaScreenEffect::GroupTag>::Definition =
	{
		"AreaScreenEffect", sizeof(AreaScreenEffect),
		{
			{ FieldType::Block, "ScreenEffects", &ScreenEffectsStruct }
		}
	};
}