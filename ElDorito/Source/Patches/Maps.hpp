#pragma once

#include "../Blam/BlamTypes.hpp"

namespace Patches::Maps
{
	void ApplyAll();
	void InitializeMapVariant(Blam::MapVariant *mapv, int mapid);
}