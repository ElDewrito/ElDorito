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
				return GetDefinition<T>(GetGroupTag());
			}

			// For use when GetGroupTag may fail due to a tag not being loaded yet
			template <typename T>
			inline T *GetDefinition(int groupTag)
			{
				if (Index != 0xFFFF)
				{
					typedef void *(*GetTagAddressPtr)(int groupTag, uint32_t index);
					auto GetTagAddressImpl = reinterpret_cast<GetTagAddressPtr>(0x503370);

					return reinterpret_cast<T *>(GetTagAddressImpl(groupTag, Index));
				}

				return nullptr;
			}
		};
	}
}
