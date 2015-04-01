#pragma once

#include <inttypes.h>
#include <string>

namespace Blam
{
	enum GameType : uint32_t
	{
		None = 0,
		CTF,
		Slayer,
		Oddball,
		KOTH,
		Forge,
		VIP,
		Juggernaut,
		Territories,
		Assault,
		Infection,
		GameTypeCount
	};
	const std::string GameTypeNames[GameTypeCount] =
	{
		"none",
		"ctf",
		"slayer",
		"koth",
		"forge",
		"vip",
		"juggernaut",
		"territories",
		"assault",
		"infection"
	};

	enum GameMode : uint32_t
	{
		InvalidMode = 0,
		Campaign,
		Multiplayer,
		Mainmenu,
		Shared, // not sure
		GameModesCount
	};

	const std::string GameModeNames[GameModesCount] =
	{
		"none",
		"campaign",
		"multiplayer",
		"shared",
	};
}

namespace GameGlobals
{
	namespace Physics
	{
		const size_t TLSOffset = 0x32C;
		const unsigned GravityIndex = 0;
		const float DefualtGravity = 4.17126f;
	}

	namespace Graphics
	{
		const size_t TLSOffset = 0x3C0;
		const unsigned GraphicsOverrideIndex = 0;
		const unsigned SaturationIndex = 4;
		const unsigned ColorIndex = 8;
	}

	namespace Time
	{
		const size_t TLSOffset = 0x50;
		const size_t DTInverseIndex = 8;
		const size_t GameSpeedIndex = 16;
	}

  namespace Cinematic
  {
    const size_t TLSOffset = 0x324;
    const unsigned LetterboxIndex = 4;
  }
}