#pragma once
#include <cstdint>
#include "Tags.hpp"

namespace Blam
{
	namespace Tags
	{
		struct TagInstance
		{
			uint16_t Index;

			TagInstance(const uint16_t index);

			uint32_t GetGroupTag();

			template <typename T>
			inline T *GetDefinition()
			{
				if (Index != 0xFFFF)
				{
					typedef void *(*GetTagAddressPtr)(int groupTag, uint32_t index);
					auto GetTagAddressImpl = reinterpret_cast<GetTagAddressPtr>(0x503370);

					return reinterpret_cast<T *>(GetTagAddressImpl(GetGroupTag(), Index));
				}

				return nullptr;
			}
		};
	}
}
