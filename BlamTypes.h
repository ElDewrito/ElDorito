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

	// todo: safely make this into a C++ struct without any padding/packing etc
	typedef struct _BLAM_GAME_VARIANT
	{
		/* 0xB2C */ uint32_t GameType; // see Blam::GameType
		float Unknown1; // gravity maybe?
		uint8_t Unknown2[0x2C];
		/* 0xB60 */ uint8_t Unknown3[0xF0]; // start of variant data
		/* 0xC50 */ uint8_t TeamGame; // default 0, 1 for eg. Team Slayer, 0 for Slayer
		uint8_t RoundTimeLimit; // default 8
		uint8_t Unknown5; // default 1
		uint8_t NumberOfRounds; // default 2
		uint8_t Unknown6; // default 0x10
		uint8_t Unknown7; // 0
		uint8_t Unknown8; // 0
		uint8_t Unknown9; // default 5
		/* 0xC58 */ uint8_t Unknown10; // default 0xA
		uint8_t RespawnTime; // default 5 (is doubled maybe.. or when i was testing i was having time added on for suicide perhaps)
		// also this is the actual time till player gets respawned without pressing buttons, the respawn time shown is sometimes a few seconds sooner for some reason

		uint8_t Unknown11; // default 5
		uint8_t Unknown12; // 0
		uint8_t Unknown13; // default 5 
		uint8_t Unknown14; // 0
		uint8_t Unknown15; // 0
		uint8_t Unknown16; // 0
		/* 0xC60 */ uint8_t Unknown17; // 0
		uint8_t Unknown18; // 0
		uint8_t Unknown19; // 0
		uint8_t Unknown20; // 0
		uint8_t Unknown21; // 0
		uint8_t Unknown22; // 0
		uint8_t Unknown23; // 0
		uint8_t Unknown24; // 0
		/* 0xC68 */ uint8_t Unknown25; // 0
		uint8_t Unknown26; // 0
		uint8_t Unknown27; // 0
		uint8_t Unknown28; // default 0xFE
		uint8_t Unknown29; // default 0xFE
		uint8_t Unknown30; // 0
		uint8_t Unknown31; // 0
		uint8_t Unknown32; // 0
		/* 0xC70 */ uint8_t Unknown33; // 0
		uint8_t Unknown34; // 0
		uint8_t Unknown35; // 0
		uint8_t Unknown36; // 0
		uint8_t Unknown37; // 0
		uint8_t Unknown38; // 0
		uint8_t Unknown39; // 0
		uint8_t Unknown40; // 0
		/* 0xC78 */ uint8_t Unknown41; // 0
		uint8_t Unknown42; // 0
		uint8_t Unknown43; // 0
		uint8_t Unknown44; // 0
		uint8_t Unknown45; // 0
		uint8_t Unknown46; // default 7
		uint8_t Unknown47; // 0
		uint8_t Unknown48; // default 1
		/* 0xC80 */ uint8_t Unknown49; // 0
		uint8_t Unknown50; // default 1
		uint8_t Unknown51; // 0
		uint8_t Unknown52; // 0
		uint8_t Unknown53; // 0
		uint8_t Unknown54; // 0
		uint8_t Unknown55; // 0
		uint8_t Unknown56; // 0
		/* 0xC88 */ uint8_t Unknown57; // 0
		uint8_t Unknown58; // 0
		uint8_t Unknown59; // 0
		uint8_t Unknown60; // 0
		uint8_t Unknown61; // 0
		uint8_t Unknown62; // 0
		uint8_t Unknown63; // 0
		uint8_t Unknown64; // default 0xFE
		uint8_t Unknown65; // default 0xFE
		// theres a lot more bytes too
	} BLAM_GAME_VARIANT, *PBLAM_GAME_VARIANT;
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
		const unsigned WaterDensityIndex = 4;
		const unsigned AirDensityIndex = 8;
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