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

		// Contains information about a player.
		struct PlayerProperties
		{
			uint8_t Unknown0[0x30];
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