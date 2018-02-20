#include "BlamPlayers.hpp"
#include "../ElDorito.hpp"
#include <iomanip>

namespace
{
	const size_t PlayersTlsOffset = 0x40;
}

namespace Blam::Players
{
	DataArray<PlayerDatum>& GetPlayers()
	{
		return *ElDorito::Instance().GetMainTls(PlayersTlsOffset).Read<DataArray<PlayerDatum>*>();
	}

	DatumHandle GetLocalPlayer(int index)
	{
		typedef uint32_t(*GetLocalPlayerPtr)(int index);
		auto GetLocalPlayer = reinterpret_cast<GetLocalPlayerPtr>(0x589C30);
		return GetLocalPlayer(index);
	}

	int GetKills(DatumHandle player)
	{
		typedef int(*GetPlayerKillsPtr)(DatumHandle player);
		auto GetPlayerKills = reinterpret_cast<GetPlayerKillsPtr>(0x550620);
		return GetPlayerKills(player);
	}

	int GetDeaths(DatumHandle player)
	{
		typedef int(*GetPlayerDeathsPtr)(DatumHandle player);
		auto GetPlayerDeaths = reinterpret_cast<GetPlayerDeathsPtr>(0x550580);
		return GetPlayerDeaths(player);
	}

	int GetAssists(DatumHandle player)
	{
		typedef int(*GetPlayerAssistsPtr)(DatumHandle player);
		auto GetPlayerAssists = reinterpret_cast<GetPlayerAssistsPtr>(0x5504E0);
		return GetPlayerAssists(player);
	}

	PLAYER_STATS GetStats(int playerIndex)
	{
		return Pointer(0x023F1718 + (playerIndex * GameGlobals::GlobalStats::PlayerLength)).Read<PLAYER_STATS>();
	}

	PLAYER_KILLED_PLAYER_STATS GetPVPStats(int playerIndex)
	{
		return Pointer(0x23F5A98 + (playerIndex * 0x40)).Read<PLAYER_KILLED_PLAYER_STATS>();
	}

	void FormatUid(char* out, int64_t Uid)
	{
		std::stringstream ss;
		ss << std::setw(16) << std::setfill('0') << std::hex << Uid << std::dec << std::setw(0);

		strncpy(out, ss.str().c_str(), 16);
		out[16] = '\0';
	}
}
