#pragma once
#include <cstdint>
#include <vector>
#include "Tags.hpp"

namespace Blam::Tags
{
	const auto TagIndexTablePtr = (uint32_t**)0x022AAFFC;
	const auto TagTablePtr = (uint8_t***)0x022AAFF8;
	const auto MaxTagCountPtr = (uint32_t*)0x022AB008;

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
			auto maxTagCount = *MaxTagCountPtr;

			if (Index == 0xFFFF || Index >= maxTagCount * 4)
				return nullptr;
			auto tagTableIndex = (*TagIndexTablePtr)[Index];
			if (tagTableIndex == -1 && tagTableIndex >= maxTagCount * 4)
				return nullptr;
			auto tagHeader = (*TagTablePtr)[tagTableIndex];
			if (!tagHeader)
				return nullptr;

			return reinterpret_cast<T *>(tagHeader + *(uint32_t*)(tagHeader + 0x10));
		}

		// Gets all valid tag instances
		inline static std::vector<TagInstance> GetInstances()
		{
			auto tagCount = *reinterpret_cast<uint32_t *>(0x22AB008);
			std::vector<TagInstance> result;

			for (auto i = 0U; i < tagCount; i++)
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

			for (auto i = 0U; i < tagCount; i++)
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
			TagInstance instance(index);
			return instance.GetDefinition<void>(groupTag) && instance.GetGroupTag() == groupTag;
		}
	};
}
