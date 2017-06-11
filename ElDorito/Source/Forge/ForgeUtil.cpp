#include "ForgeUtil.hpp"

namespace Forge
{
	Blam::MapVariant* GetMapVariant()
	{
		static auto GetMapVariant = (Blam::MapVariant* (__cdecl*)())(0x00583230);
		return GetMapVariant();
	}
}
