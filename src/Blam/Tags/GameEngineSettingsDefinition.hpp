#pragma once
#include "Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		struct TraitsProfileDefinition;
		struct SlayerVariantDefinition;
		struct OddballVariantDefinition;
		struct CtfVariantDefinition;
		struct AssaultVariantDefinition;
		struct InfectionVariantDefinition;
		struct KothVariantDefinition;
		struct TerritoriesVariantDefinition;
		struct JuggernautVariantDefinition;
		struct VipVariantDefinition;
		struct SandboxVariantDefinition;

		struct GameEngineSettingsDefinition : Tag<'wezr'>
		{
			int Unknown0;
			TagBlock<TraitsProfileDefinition> TraitsProfiles;
			TagBlock<SlayerVariantDefinition> SlayerVariants;
			TagBlock<OddballVariantDefinition> OddballVariants;
			TagBlock<CtfVariantDefinition> CtfVariants;
			TagBlock<AssaultVariantDefinition> AssaultVariants;
			TagBlock<InfectionVariantDefinition> InfectionVariants;
			TagBlock<KothVariantDefinition> KothVariants;
			TagBlock<TerritoriesVariantDefinition> TerritoriesVariants;
			TagBlock<JuggernautVariantDefinition> JuggernautVariants;
			TagBlock<VipVariantDefinition> VipVariants;
			TagBlock<SandboxVariantDefinition> SandboxVariants;
			int Unknown88;
		};
		TAG_STRUCT_SIZE_ASSERT(GameEngineSettingsDefinition, 0x8C);

		struct TraitsProfileDefinition
		{
			struct TagBlock1;
			struct TagBlock2;
			struct TagBlock3;
			struct TagBlock4;
			struct TagBlock5;

			int Unknown0;
			TagBlock<TagBlock1> Unknown4;
			TagBlock<TagBlock2> Unknown10;
			TagBlock<TagBlock3> Unknown1C;
			TagBlock<TagBlock4> Unknown28;
			TagBlock<TagBlock5> Unknown34;

			struct TagBlock1
			{
				int Unknown0;
				int Unknown4;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock1, 0x8);

			struct TagBlock2
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock2, 0x10);

			struct TagBlock3
			{
				int Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock3, 0x4);

			struct TagBlock4
			{
				int Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock4, 0x4);

			struct TagBlock5
			{
				int Unknown0;
				int Unknown4;
			};
			TAG_STRUCT_SIZE_ASSERT(TagBlock5, 0x8);
		};
		TAG_STRUCT_SIZE_ASSERT(TraitsProfileDefinition, 0x40);

		struct GameVariantDefinition
		{
			struct GeneralSettingsDefinition;
			struct RespawnSettingsDefinition;
			struct SocialSettingsDefinition;
			struct MapOverridesDefinition;

			char Name[0x20];
			int NameStringId;
			int DescriptionStringId;
			TagBlock<GeneralSettingsDefinition> GeneralSettings;
			TagBlock<RespawnSettingsDefinition> RespawnSettings;
			TagBlock<SocialSettingsDefinition> SocialSettings;
			TagBlock<MapOverridesDefinition> MapOverrides;

			struct GeneralSettingsDefinition
			{
				int Unknown0;
				int Unknown4;
			};
			TAG_STRUCT_SIZE_ASSERT(GeneralSettingsDefinition, 0x8);

			struct RespawnSettingsDefinition
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
			};
			TAG_STRUCT_SIZE_ASSERT(RespawnSettingsDefinition, 0x14);

			struct SocialSettingsDefinition
			{
				int Unknown0;
			};
			TAG_STRUCT_SIZE_ASSERT(SocialSettingsDefinition, 0x4);

			struct MapOverridesDefinition
			{
				int Unknown0;
				int Unknown4;
				int Unknown8;
				int UnknownC;
				int Unknown10;
				int Unknown14;
				int Unknown18;
				int Unknown1C;
			};
			TAG_STRUCT_SIZE_ASSERT(MapOverridesDefinition, 0x20);
		};
		TAG_STRUCT_SIZE_ASSERT(GameVariantDefinition, 0x58);

		struct SlayerVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
		};
		TAG_STRUCT_SIZE_ASSERT(SlayerVariantDefinition, 0x70);

		struct OddballVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
		};
		TAG_STRUCT_SIZE_ASSERT(OddballVariantDefinition, 0x70);

		struct CtfVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
		};
		TAG_STRUCT_SIZE_ASSERT(CtfVariantDefinition, 0x70);

		struct AssaultVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
			int Unknown70;
			int Unknown74;
			int Unknown78;
			int Unknown7C;
		};
		TAG_STRUCT_SIZE_ASSERT(AssaultVariantDefinition, 0x80);

		struct InfectionVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
			int Unknown70;
			int Unknown74;
			int Unknown78;
		};
		TAG_STRUCT_SIZE_ASSERT(InfectionVariantDefinition, 0x7C);

		struct KothVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
		};
		TAG_STRUCT_SIZE_ASSERT(KothVariantDefinition, 0x70);

		struct TerritoriesVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
		};
		TAG_STRUCT_SIZE_ASSERT(TerritoriesVariantDefinition, 0x6C);

		struct JuggernautVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
			int Unknown70;
		};
		TAG_STRUCT_SIZE_ASSERT(JuggernautVariantDefinition, 0x74);

		struct VipVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
			int Unknown64;
			int Unknown68;
			int Unknown6C;
			int Unknown70;
			int Unknown74;
			int Unknown78;
		};
		TAG_STRUCT_SIZE_ASSERT(VipVariantDefinition, 0x7C);

		struct SandboxVariantDefinition : GameVariantDefinition
		{
			int Unknown58;
			int Unknown5C;
			int Unknown60;
		};
		TAG_STRUCT_SIZE_ASSERT(SandboxVariantDefinition, 0x64);
	}
}
