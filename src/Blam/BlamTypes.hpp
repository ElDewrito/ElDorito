#pragma once

#include <cstdint>
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
		GameModeCount
	};

	const std::string GameModeNames[GameModeCount] =
	{
		"none",
		"campaign",
		"multiplayer",
		"mainmenu",
		"shared"
	};

	typedef struct _BLAM_CONTENT_HEADER
	{
		uint64_t ID; // not sure about this
		wchar_t Name[0x10];
		char Description[0x80];
		char Author[0x10];
		uint32_t Type;
		uint32_t Unknown1; // 0
		uint32_t Unknown2; // 1
		uint32_t Unknown3; // 0
		uint32_t Size; // 0xE1F0 for maps, 0x3BC for variants
		uint32_t Unknown4; // 0
		uint32_t UnknownUnique1; // changes for each item
		uint32_t Unknown5; // 0
		uint32_t Unknown6; // 0
		uint32_t Unknown7; // -1
		uint32_t Unknown8; // 0x140 for maps, -1 for variants, 
		uint64_t Unknown9; // 0 for maps, 4 for variants
		uint32_t Unknown10; // -1;
		uint32_t Unknown11; // 0;
		uint32_t Unknown12; // 0;
		uint32_t Unknown13; // 0;
	} BLAM_CONTENT_HEADER, *PBLAM_CONTENT_HEADER;

	// struct of entries in the content items global
	typedef struct _BLAM_CONTENT_ITEM
	{
		uint32_t Index;
		uint32_t ItemFlags;
		uint32_t ItemType;
		uint32_t BaseOffset;
		BLAM_CONTENT_HEADER ContentHeader;
		wchar_t FilePath[0x99];
	} BLAM_CONTENT_ITEM, *PBLAM_CONTENT_ITEM;

	// todo: safely make this into a C++ struct without any padding/packing etc
	typedef struct _BLAM_GAME_VARIANT
	{
		/* 0xB2C */ uint32_t GameType; // see Blam::GameType
		float Unknown1; // gravity maybe?
		uint8_t Unknown2[0x2C];
		/* 0xB60 */ uint8_t Unknown3[0xF0]; // start of variant data
		/* 0xC50 */ uint8_t TeamGame; // default 0, 1 for eg. Team Slayer, 0 for Slayer (note: the game ANDs this with 1, is that actually necessary?)
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
		const float DefaultGravity = 4.17126f;
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
		const unsigned FpsIndex = 4;
		const unsigned DTInverseIndex = 8;
		const unsigned GameSpeedIndex = 16;
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

	// atmosphere_fog globals
	namespace Bloom
	{
		const size_t TLSOffset = 0x104;
		const unsigned EnableIndex = 0;
		const unsigned IntensityIndex = 8;
	}

	namespace Players
	{
		const size_t TLSOffset = 0x40;
		const size_t ArrayHeaderLength = 0x54;
		const size_t PlayerEntryLength = 0x2F08;
		const size_t ScoresEntryLength = 0x34; // size of scores entry per player

		const size_t DisplayNameOffset = 0xA8;
		const size_t TeamOffset = 0xC8; // ( (TLSOffset) + ArrayHeaderLength + (i*PlayerEntryLength) + TeamOffset )

		const size_t ScoreBase = 0x4046C; // ( (TLSOffset) + ArrayHeaderLength + (i*ScoresEntryLength) + ScoreBase )
		const size_t KillsBase = 0x40470;
		const size_t DeathsBase = 0x40472;
		const size_t AssistsBase = 0x40474; // not sure about this
	}
	
	namespace Input
	{
		const size_t TLSOffset = 0xC4;
		const size_t DisablePlayerInputIndex = 6;
		const size_t HorizontalViewAngle = 0x30C;
		const size_t VerticalViewAngle = 0x310;
	}

	namespace GameInfo
	{
		const size_t TLSOffset = 0x3C;
		const size_t GameMode = 0x10;
	}

	namespace GameSettings
	{
		const size_t TLSOffset = 0x18;
		const size_t MouseAcceleration = 0x41C88;
		const size_t YAxisInverted = 0x41C8C;

		// these don't seem to be used, keeping them here because afaik this is what they actually should be though
		/*const size_t YAxisSensitivity = 0x43C78;
		const size_t XAxisSensitivity = 0x43C7C;
		const size_t VehicleYAxisSensitivity = 0x43C80;
		const size_t VehicleXAxisSensitivity = 0x43C84;*/


		const size_t MouseAccelerationAlt = 0x81C88;
		const size_t YAxisInvertedAlt = 0x81C8C;

		const size_t YAxisSensitivity = 0x83C78;
		const size_t XAxisSensitivity = 0x83C7C;
		const size_t VehicleYAxisSensitivity = 0x83C80;
		const size_t VehicleXAxisSensitivity = 0x83C84;
	}

	namespace PlayerAlt // todo: find out what this global is actually for
	{
		const size_t TLSOffset = 0xe8;
		const size_t VehicleData = 0x8; // pointer
		const size_t VehicleDataIsInVehicle = 0xA0;

		const size_t PlayerObjectSize = 0x3C8;
	}

	namespace LocalPlayers
	{
		const size_t TLSOffset = 0x5C;

		const size_t Player0DatumIdx = 0x4;
		const size_t Player1DatumIdx = 0x4;
		const size_t Player2DatumIdx = 0x4;
		const size_t Player3DatumIdx = 0x4;
	}

	namespace PersistentUserDataChud
	{
		const size_t TLSOffset = 0x424;
		const size_t SprintMeterOffset = 0x157;
	}
}