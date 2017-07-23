#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::Game
{
	using Blam::Tags::TagBlock;
	using Blam::Text::StringID;

	struct GameVariant
	{
		struct GeneralSettings;
		struct RespawnSettings;
		struct SocialSettings;
		struct MapOverrides;

		char Name[32];
		StringID NameID;
		StringID Description;
		TagBlock<GameVariant::GeneralSettings> GeneralSettings;
		TagBlock<GameVariant::RespawnSettings> RespawnSettings;
		TagBlock<GameVariant::SocialSettings> SocialSettings;
		TagBlock<GameVariant::MapOverrides> MapOverrides;

		struct GameVariant::GeneralSettings
		{
			enum class Flags : int32_t;
			enum class RoundResets : int8_t;

			GameVariant::GeneralSettings::Flags Flags : 32;
			int8_t TimeLimit;
			int8_t NumberOfRounds;
			int8_t EarlyVictoryWinCount;
			GameVariant::GeneralSettings::RoundResets RoundResets : 8;

			enum class GameVariant::GeneralSettings::Flags : int32_t
			{
				None,
				TeamsEnabled = 1 << 0,
				Unknown = 1 << 1
			};

			enum class GameVariant::GeneralSettings::RoundResets : int8_t
			{
				Nothing,
				PlayersOnly,
				Everything
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct GameVariant::GeneralSettings, 0x8);

		struct GameVariant::RespawnSettings
		{
			enum class Flags : uint16_t;

			GameVariant::RespawnSettings::Flags Flags : 16;
			int8_t LivesPerRound;
			int8_t SharedTeamLives;
			uint8_t RespawnTime;
			uint8_t SuicidePenalty;
			uint8_t BetrayalPenalty;
			uint8_t UnknownPenalty;
			uint8_t RespawnTimeGrowth;
			uint8_t Unknown1;
			uint8_t Unknown2;
			uint8_t Unknown3;
			StringID RespawnTraitProfile;
			uint8_t RespawnTraitDuration;
			uint8_t Unknown4;
			uint8_t Unknown5;
			uint8_t Unknown6;

			enum class GameVariant::RespawnSettings::Flags : uint16_t
			{
				None,
				InheritRespawnTime = 1 << 0,
				RespawnWithTeam = 1 << 1,
				RespawnAtLocation = 1 << 2,
				RespawnOnKills = 1 << 3,
				Unknown = 1 << 4
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct GameVariant::RespawnSettings, 0x14);

		struct GameVariant::SocialSettings
		{
			enum class Flags : int32_t;

			GameVariant::SocialSettings::Flags Flags : 32;

			enum class GameVariant::SocialSettings::Flags : int32_t
			{
				None,
				ObserversEnabled = 1 << 0,
				TeamChangingEnabled = 1 << 1,
				BalancedTeamChanging = 1 << 2,
				FriendlyFireEnabled = 1 << 3,
				BetrayalBootingEnabled = 1 << 4,
				EnemyVoiceEnabled = 1 << 5,
				OpenChannelVoiceEnabled = 1 << 6,
				DeadPlayerVoiceEnabled = 1 << 7,
				CrossRaceVoiceEnabled = 1 << 8
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct GameVariant::SocialSettings, 0x4);

		struct GameVariant::MapOverrides
		{
			enum class Flags : int32_t;

			GameVariant::MapOverrides::Flags Flags : 32;
			StringID BasePlayerTraitProfile;
			StringID WeaponSet;
			StringID VehicleSet;
			StringID OvershieldTraitProfile;
			StringID ActiveCamoTraitProfile;
			StringID CustomPowerupTraitProfile;
			int8_t OvershieldTraitDuration;
			int8_t ActiveCamoTraitDuration;
			int8_t CustomPowerupTraitDuration;
			int8_t Unknown;

			enum class GameVariant::MapOverrides::Flags : int32_t
			{
				None,
				GrenadesOnMap = 1 << 0,
				IndestructibleVehicles = 1 << 1
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct GameVariant::MapOverrides, 0x20);
	};
	TAG_STRUCT_SIZE_ASSERT(struct GameVariant, 0x58);
}

