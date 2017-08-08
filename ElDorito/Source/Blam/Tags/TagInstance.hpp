#pragma once
#include <cstdint>
#include <vector>
#include "Tags.hpp"

// Disable warnings about signed/unsigned mismatch
#pragma warning(disable:4018)


namespace Blam::Tags
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

		// Gets all valid tag instances
		inline static std::vector<TagInstance> GetInstances()
		{
			auto tagCount = *reinterpret_cast<uint32_t *>(0x22AB008);
			std::vector<TagInstance> result;

			for (auto i = 0; i < tagCount; i++)
			{
				auto instance = TagInstance(i);

				if (instance.GetDefinition<void>() == nullptr)
					continue;

				result.push_back(instance);
			}

			return result;
		}

		// Gets all valid tag instances within the specified tag group
		inline static std::vector<TagInstance> GetInstancesInGroup(const Tag groupTag)
		{
			auto tagCount = *reinterpret_cast<uint32_t *>(0x22AB008);
			std::vector<TagInstance> result;

			for (auto i = 0; i < tagCount; i++)
			{
				auto instance = TagInstance(i);

				if (!IsLoaded(groupTag, instance.Index))
					continue;

				if (instance.GetDefinition<void>() == nullptr)
					continue;

				if (instance.GetGroupTag() != groupTag)
					continue;

				result.push_back(instance);
			}

			return result;
		}

		// Returns true if the tag of the provided group and index is loaded
		inline static bool IsLoaded(int groupTag, uint32_t index)
		{
			typedef void *(*GetTagAddressPtr)(int groupTag, uint32_t index);
			auto GetTagAddressImpl = reinterpret_cast<GetTagAddressPtr>(0x503370);

			if (GetTagAddressImpl('unic', 0x12c2) == nullptr)
				return false;

			return true;
		}
	};
}
