#pragma once
#include "../../Padding.hpp"
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include "TraitsProfile.hpp"
#include "GameVariant.hpp"
#include "SlayerVariant.hpp"
#include "OddballVariant.hpp"
#include "CTFVariant.hpp"
#include "AssaultVariant.hpp"
#include "InfectionVariant.hpp"
#include "KOTHVariant.hpp"
#include "TerritoriesVariant.hpp"
#include "JuggernautVariant.hpp"
#include "VIPVariant.hpp"
#include "SandboxVariant.hpp"

namespace Blam::Tags::Game
{
	using Blam::Tags::Tag;
	using Blam::Tags::TagBlock;
	using Blam::Tags::TagData;
	using Blam::Tags::TagGroup;
	using Blam::Tags::TagReference;
	using Blam::Text::StringID;

	struct GameEngineSettingsDefinition : TagGroup<'wezr'>
	{
		PAD32;
		TagBlock<TraitsProfile> TraitsProfiles;
		TagBlock<SlayerVariant> SlayerVariants;
		TagBlock<OddballVariant> OddballVariants;
		TagBlock<CTFVariant> CTFVariants;
		TagBlock<AssaultVariant> AssaultVariants;
		TagBlock<InfectionVariant> InfectionVariants;
		TagBlock<KOTHVariant> KOTHVariants;
		TagBlock<TerritoriesVariant> TerritoriesVariants;
		TagBlock<JuggernautVariant> JuggernautVariants;
		TagBlock<VIPVariant> VIPVariants;
		TagBlock<SandboxVariant> SandboxVariants;
		PAD32;
	};
	TAG_STRUCT_SIZE_ASSERT(struct GameEngineSettingsDefinition, 0x8C);
}

