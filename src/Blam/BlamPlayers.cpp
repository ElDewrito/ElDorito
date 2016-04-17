#include "BlamPlayers.hpp"
#include "../ElDorito.hpp"

namespace
{
	const size_t PlayersTlsOffset = 0x40;
}

namespace Blam
{
	namespace Players
	{
		DataArray<PlayerDatum>& GetPlayers()
		{
			return *ElDorito::Instance().GetMainTls(PlayersTlsOffset).Read<DataArray<PlayerDatum>*>();
		}

		DatumIndex GetLocalPlayer(int index)
		{
			typedef uint32_t(*GetLocalPlayerPtr)(int index);
			auto GetLocalPlayer = reinterpret_cast<GetLocalPlayerPtr>(0x589C30);
			return GetLocalPlayer(index);
		}

		int GetKills(DatumIndex player)
		{
			typedef int(*GetPlayerKillsPtr)(DatumIndex player);
			auto GetPlayerKills = reinterpret_cast<GetPlayerKillsPtr>(0x550620);
			return GetPlayerKills(player);
		}

		int GetDeaths(DatumIndex player)
		{
			typedef int(*GetPlayerDeathsPtr)(DatumIndex player);
			auto GetPlayerDeaths = reinterpret_cast<GetPlayerDeathsPtr>(0x550580);
			return GetPlayerDeaths(player);
		}

		int GetAssists(DatumIndex player)
		{
			typedef int(*GetPlayerAssistsPtr)(DatumIndex player);
			auto GetPlayerAssists = reinterpret_cast<GetPlayerAssistsPtr>(0x5504E0);
			return GetPlayerAssists(player);
		}
	}
}