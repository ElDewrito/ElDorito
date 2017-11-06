#pragma once
#include <cstdint>
#include "../../Text/StringID.hpp"
#include "GameVariant.hpp"

namespace Blam::Tags::Game
{
	using Blam::Text::StringID;

	struct SlayerVariant : GameVariant
	{
		enum class TeamScoring : int16_t;

		SlayerVariant::TeamScoring TeamScoring : 16;
		int16_t PointsToWin;
		int16_t Unknown1;
		int8_t KillPoints;
		int8_t AssistPoints;
		int8_t DeathPoints;
		int8_t SuicidePoints;
		int8_t BetrayalPoints;
		int8_t LeaderKillBonus;
		int8_t EliminationBonus;
		int8_t AssassinationBonus;
		int8_t HeadshotBonus;
		int8_t BeatdownBonus;
		int8_t StickyBonus;
		int8_t SplatterBonus;
		int8_t SpreeBonus;
		int8_t Unknown_2;
		StringID LeaderTraitProfile;

		enum class SlayerVariant::TeamScoring : int16_t
		{
			SumOfTeam,
			MinimumScore,
			MaximumScore,
			Default
		};
	};
	TAG_STRUCT_SIZE_ASSERT(struct SlayerVariant, 0x70);
}

