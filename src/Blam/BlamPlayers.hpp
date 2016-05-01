#pragma once

#include "BlamData.hpp"
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
	}
}