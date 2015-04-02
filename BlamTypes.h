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
		"oddball",
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

enum class CameraType : uint8_t
{
	Following = 0,
	Orbiting = 1,
	Flying = 2,
	FirstPerson = 3,
	Dead = 4,
	Static = 5,
	Scripted = 6,

	Authored = 7
};

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

	namespace Director
	{
		const size_t TLSOffset = 0x60;
		const unsigned CameraFunctionIndex = 4;
	}

	namespace DepthOfField
	{
		const size_t TLSOffset = 0x3DC;
		const unsigned EnableIndex = 0;
		const unsigned IntensityIndex = 16;
	}

	namespace Bloom
	{
    const size_t TLSOffset = 0x104;
		const unsigned EnableIndex = 0;
		const unsigned IntensityIndex = 8;
	}
}