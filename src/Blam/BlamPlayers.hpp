#pragma once

#include "BlamData.hpp"
#include "Tags/Objects/Damage.hpp"
#include <cstdint>

namespace Blam
{
	namespace Players
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
				Holo,

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
				Shoulders,
				Arms,
				Legs,
				Acc,
				Pelvis,

				Count
			};
		};

		// Contains customization data for a player.
		struct PlayerCustomization
		{
			uint32_t Colors[ColorIndices::Count];
			uint8_t Armor[ArmorIndices::Count];
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
			uint8_t Unknown5C[0x66C];
			PlayerCustomization Customization;
			uint8_t Unknown6E4[0xF38];
		};
		static_assert(sizeof(PlayerProperties) == 0x1620, "Invalid PlayerProperties size");

		// A global player datum.
		struct PlayerDatum : DatumBase
		{
			uint8_t Unknown2[0x2E];
			DatumIndex SlaveUnit;
			uint8_t Unknown34[0x3C];
			PlayerProperties Properties;
			uint8_t Unknown1690[0x1724];
			int16_t SpawnGrenadeSetting;
			uint8_t Unknown2DB6[0x152];
		};
		static_assert(sizeof(PlayerDatum) == 0x2F08, "Invalid PlayerDatum size");

		typedef struct _PLAYER_MEDALS
		{
			uint16_t Exterminations; //0x5A
			uint16_t Perfections; //0x5C
			uint16_t DoubleKills; //0x5E
			uint16_t TripleKills; //0x60
			uint16_t OverKills; //0x62
			uint16_t Killtaculars; //0x64
			uint16_t Killtrocities; //0x66
			uint16_t Killimanjaros; //0x68
			uint16_t Killtastrophes; //0x6A
			uint16_t Killpocalypses; //0x6C
			uint16_t Killionaires; //0x6E
			uint16_t KillingSprees; //0x70
			uint16_t KillingFrenzies; //0x72
			uint16_t RunningRiots; //0x74
			uint16_t Rampages; //0x76
			uint16_t Untouchables; //0x78
			uint16_t Invincibles; //0x7A
			uint16_t SniperSprees; //0x7C
			uint16_t Sharpshooters; //0x7E
			uint16_t ShotgunSprees; //0x80
			uint16_t OpenSeasons; //0x82
			uint16_t SplatterSprees; //0x84
			uint16_t VehicularManslaughters; //0x86
			uint16_t SwordSprees; //0x88
			uint16_t SliceNDices; //0x8A
			uint16_t JuggernautSprees; //0x8C
			uint16_t Unstoppables; //0x8E
			uint16_t InfectionSprees; //0x90
			uint16_t MMMBrains; //0x92
			uint16_t ZombieKillingSprees; //0x94
			uint16_t HellsJanitors; //0x96
			uint16_t Unknown6; //0x98

			uint16_t HailToTheKings; //0x9A
			uint16_t Bulltrue; //0x9C
			uint16_t Splatters; //0x9E
			uint16_t HiJacks; //0xA0
			uint16_t Skyjacks; //0xA2
			uint16_t Unknown9; //0xA4

			uint16_t Killjoys; //0xA6
			uint16_t Unknown7; //0xA8
			uint16_t Sticks; //0xAA
			uint16_t Headshots; //0xAC
			uint16_t Assasinations; //0xAE
			uint16_t Beatdowns; //0xB0
			uint16_t Incinerations; //0xB2
			uint16_t Wheelmans; //0xB4
			uint16_t BombPlanteds; //0xB6
			uint16_t KilledBombCarriers; //0xB8
			uint16_t KilledVIPs; //0xBA
			uint16_t KilledJuggernauts; //0xBC
			uint16_t Unknown10; //0xBE

			uint16_t FlagScores; //0xC0
			uint16_t KilledFlagCarriers; //0xBC
		} PLAYER_MEDALS;

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

			PLAYER_MEDALS Medals;
			uint8_t Unknown6[0x6C];

			WEAPON_STATS WeaponStats[Blam::Tags::Objects::DamageReportingType::Count];
		} PLAYER_STATS, *PPLAYER_STATS;

		// Gets the global players data array.
		DataArray<PlayerDatum>& GetPlayers();

		// Gets the datum index for a local player (0-3).
		DatumIndex GetLocalPlayer(int index);

		// Gets a player's kill count.
		int GetKills(DatumIndex player);

		// Gets a player's death count.
		int GetDeaths(DatumIndex player);

		// Gets a player's assist count.
		int GetAssists(DatumIndex player);

		// Gets a player's stats.
		PLAYER_STATS GetStats(int playerIndex);
	}
}