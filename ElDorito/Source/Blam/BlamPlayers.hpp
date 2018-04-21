#pragma once

#include "BlamData.hpp"
#include "BlamTypes.hpp"
#include "Tags/Objects/Damage.hpp"
#include "Tags/Objects/Medal.hpp"
#include <cstdint>

namespace Blam::Players
{
	// Index values for the Colors array in the PlayerCustomization struct.
	struct ColorIndices
	{
		enum
		{
			Primary = 0,
			Secondary,
			Visor,
			Lights,

			Count
		};
	};

	// Index values for the Armor array in the PlayerCustomization struct.
	struct ArmorIndices
	{
		enum
		{
			Helmet = 0,
			Chest,
			RightShoulder,
			LeftShoulder,

			Count
		};
	};

	// Contains customization data for a player.
	struct PlayerCustomization
	{
		uint32_t Colors[ColorIndices::Count];
		uint32_t : 32;
		uint8_t Armor[ArmorIndices::Count];
		uint8_t : 8;
		uint8_t : 8;
		uint8_t : 8;
		uint32_t Unknown1C;
	};
	static_assert(sizeof(PlayerCustomization) == 0x20, "Invalid PlayerCustomization size");

	// Contains information about a player which can be set by the client.
	// This structure is included in the player-properties packet sent to the host.
	// See the comment in the PlayerProperties structure below for more information.
	struct ClientPlayerProperties
	{
		wchar_t DisplayName[16];
		int8_t TeamIndex;
		int16_t Unknown22;
		uint32_t Unknown24;
		uint32_t Unknown28;
		uint32_t Unknown2C;
	};
	static_assert(sizeof(ClientPlayerProperties) == 0x30, "Invalid ClientPlayerProperties size");

	// Contains information about a player.
	struct PlayerProperties
	{
		// The way that the ClientPlayerProperties struct works is a bit complicated.
		// Whenever the client requests a properties update, the struct gets copied out of the packet and into the struct here.
		// Then, when the membership data updates, the values are copied out into the ones below.
		//
		// So, if a value is in both PlayerProperties and ClientPlayerProperties, the following rules apply:
		// - If you need to read the value, then ALWAYS use the value directly in PlayerProperties.
		// - If you need to write the value, then ALWAYS write the value to the ClientPlayerProperties structure.
		// - Be sure to call Update() on the membership data after writing.
		//
		// For example, to toggle a player to the other team, you will need to do:
		//
		//   properties.ClientProperties.TeamIndex = !properties.TeamIndex;
		//   membership.Update();
		//
		ClientPlayerProperties ClientProperties;

		uint64_t Uid;
		wchar_t DisplayName[16];
		int TeamIndex;
		uint32_t Unknown5C;
		uint8_t Gender;
		uint8_t PlayerRepresentation;
		uint16_t Unknown62;
		uint8_t Unknown64[1616];
		wchar_t ServiceTag[6];
		uint16_t Unknown6C0;
		uint16_t Unknown6C2;
		uint16_t Unknown6C6;
		PlayerCustomization Customization;
		uint8_t Unknown6E4[0xF38];
	};
	static_assert(sizeof(PlayerProperties) == 0x1620, "Invalid PlayerProperties size");

	// A global player datum.
	struct PlayerDatum : DatumBase
	{
		uint8_t Unknown2[0x2E];
		DatumHandle SlaveUnit;
		DatumHandle DeadSlaveUnit;
		uint8_t Unknown34[0x38];
		PlayerProperties Properties;
		uint8_t Unknown1690[0x165a];
		int16_t DepletedStamina;
		uint8_t Unknown2CEA[0xc8];
		int16_t SpawnGrenadeSetting; //2db4
		uint8_t Unknown2DB6[0x152];
	};
	static_assert(sizeof(PlayerDatum) == 0x2F08, "Invalid PlayerDatum size");

	typedef struct _PLAYER_VS_STATS
	{
		uint16_t Kills;
		uint16_t KilledBy;
	} PLAYER_VS_STATS;

	typedef struct _PLAYER_KILLED_PLAYER_STATS
	{
		PLAYER_VS_STATS StatsAgainstEachPlayer[16];

	} PLAYER_KILLED_PLAYER_STATS;

	typedef struct _WEAPON_STATS
	{
		uint16_t Initialized;
		uint16_t Kills;
		uint16_t KilledBy;
		uint16_t BetrayalsWith;
		uint16_t SuicidesWith;
		uint16_t HeadshotsWith;
	} WEAPON_STATS;

	typedef struct _BLAM_PLAYER_STATS
	{
		uint8_t Unknown0[0xC];
		int16_t Score;
		uint16_t Unknown1;

		int16_t Kills; //0x04
		uint16_t Assists; //0x06
		uint16_t Deaths; //0x08
		uint16_t Betrayals; //0x0A
		uint16_t Suicides; //0x0C
		uint16_t BestStreak; //0x0E
		uint16_t TimeSpentAlive; //0x10
		uint8_t Unknown3[0x1A]; //0x12
		uint16_t KingsKilled; //0x2C
		uint16_t TimeInHill; //0x2E
		uint16_t TimeControllingHill; //0x30
		uint8_t Unknown4[0x22]; //0x32
		uint16_t ZombiesKilled; //0x54
		uint16_t HumansInfected; //0x56
		uint16_t Unknown5; //0x58

		uint16_t Medals[Blam::Tags::Objects::MedalType::MedalCount];
		uint8_t Unknown6[0x68];

		WEAPON_STATS WeaponStats[Blam::Tags::Objects::DamageReportingType::DamageCount];
	} PLAYER_STATS, *PPLAYER_STATS;
	static_assert(sizeof(PLAYER_STATS) == GameGlobals::GlobalStats::PlayerLength, "Invalid PlayerStats size");

	// Gets the global players data array.
	DataArray<PlayerDatum>& GetPlayers();

	// Gets the datum index for a local player (0-3).
	DatumHandle GetLocalPlayer(int index);

	// Gets a player's kill count.
	int GetKills(DatumHandle player);

	// Gets a player's death count.
	int GetDeaths(DatumHandle player);

	// Gets a player's assist count.
	int GetAssists(DatumHandle player);

	// Gets a player's stats.
	PLAYER_STATS GetStats(int playerIndex);

	PLAYER_KILLED_PLAYER_STATS GetPVPStats(int playerIndex);

	void FormatUid(char* out, int64_t Uid);
}
