#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include "GameVariant.hpp"

namespace Blam::Tags::Game
{
	using Blam::Text::StringID;

	struct OddballVariant : GameVariant
	{
		enum class Flags : int32_t;
		enum class TeamScoring : int16_t;

		OddballVariant::Flags Flags : 32;
		OddballVariant::TeamScoring TeamScoring : 16;
		int16_t PointsToWin;
		int16_t Unknown1;
		int8_t CarryingPoints;
		int8_t KillPoints;
		int8_t BallKillPoints;
		int8_t BallCarrierKillPoints;
		int8_t BallCount;
		int8_t Unknown2;
		int16_t InitialBallDelay;
		int16_t BallRespawnDelay;
		StringID BallCarrierTraitProfile;

		enum class OddballVariant::Flags : int32_t
		{
			None,
			AutopickupEnabled = 1 << 0,
			BallEffectEnabled = 1 << 1
		};

		enum class OddballVariant::TeamScoring : int16_t
		{
			SumOfTeam,
			MinimumScore,
			MaximumScore,
			Default
		};
	};
	TAG_STRUCT_SIZE_ASSERT(struct OddballVariant, 0x70);
}
