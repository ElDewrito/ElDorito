#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"
#include "GameVariant.hpp"

namespace Blam::Tags::Game
{
	using Blam::Text::StringID;

	struct CTFVariant : GameVariant
	{
		enum class Flags : int32_t;
		enum class HomeFlagWaypoint : int16_t;
		enum class GameMode : int16_t;
		enum class RespawnOnCapture : int16_t;

		CTFVariant::Flags Flags : 32;
		CTFVariant::HomeFlagWaypoint HomeFlagWaypoint : 16;
		CTFVariant::GameMode GameMode : 16;
		CTFVariant::RespawnOnCapture RespawnOnCapture : 16;
		int16_t FlagReturnTime;
		int16_t SuddenDeathTime;
		int16_t ScoreToWin;
		int16_t Unknown;
		int16_t FlagResetTime;
		StringID FlagCarrierTraitProfile;

		enum class CTFVariant::Flags : int32_t
		{
			None,
			FlagAtHomeToScore = 1 << 0
		};

		enum class CTFVariant::HomeFlagWaypoint : int16_t
		{
			Unknown1,
			Unknown2,
			Unknown3,
			NotInSingle
		};

		enum class CTFVariant::GameMode : int16_t
		{
			Multiple,
			Single,
			Neutral
		};

		enum class CTFVariant::RespawnOnCapture : int16_t
		{
			Disabled,
			OnAllyCapture,
			OnEnemyCapture,
			OnAnyCapture
		};
	};
	TAG_STRUCT_SIZE_ASSERT(struct CTFVariant, 0x70);
}
