#pragma once

#include <inttypes.h>

namespace BlamGameTypes
{
	enum BlamGameTypes
	{
		Invalid = 0,
		CTF,
		Slayer,
		Oddball,
		KOTH,
		Forge,
		VIP,
		Juggernaut,
		Territories,
		Assault,
		Infection
	};
}

namespace BlamGameModes
{
	enum BlamGameModes
	{
		Invalid = 0,
		Campaign,
		Multiplayer,
		Mainmenu,
		Shared // not sure
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
}