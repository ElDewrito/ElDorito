#pragma once

#include "../Blam/Math/RealVector3D.hpp"

namespace Forge::Magnets
{
	struct Magnet
	{
		Blam::Math::RealVector3D Position;
	};

	struct MagnetPair
	{
		bool IsValid;
		Magnet* Source;
		Magnet* Dest;
	};

	bool Initialize();
	void Shutdown();
	void Update();
	void Render();
	const MagnetPair& GetMagnetPair();
}
