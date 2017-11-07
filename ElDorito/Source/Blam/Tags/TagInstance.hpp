#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../../ElDorito.hpp"
#include "Tags.hpp"

namespace Blam::Tags
{
	const auto TagIndexTablePtr = (uint32_t**)0x022AAFFC;
	const auto TagTablePtr = (uint8_t***)0x022AAFF8;
	const auto MaxTagCountPtr = (uint32_t*)0x022AB008;

	struct TagInstance
	{
		uint16_t Index;

		static std::unordered_map<int32_t, std::string> TagNames;

		TagInstance(const uint16_t index);

		Tag GetGroupTag();

		inline static void LoadTagNames()
		{
			std::ifstream tagListFile(ElDorito::Instance().GetMapsFolder() + "\\tag_list.csv");

			while (tagListFile.is_open() && !tagListFile.eof())
			{
				std::string line;
				tagListFile >> line;

				auto tokens = Utils::String::SplitString(line, ',');

				if (tokens.size() != 2)
					continue;

				Blam::Tags::TagInstance::TagNames.emplace(std::make_pair(strtol(tokens.at(0).c_str(), nullptr, 0), tokens.at(1)));
			}

			tagListFile.close();
		}

		template <typename T>
		inline T *GetDefinition()
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
		inline static bool IsLoaded(Tag groupTag, uint32_t index)
		{
			TagInstance instance(index);
			return instance.GetDefinition<void>() && instance.GetGroupTag() == groupTag;
		}

		inline static TagInstance Find(const Tag groupTag, const std::string &tagName)
		{
			for (auto &entry : TagNames)
			{
				if (tagName != entry.second)
					continue;

				auto result = TagInstance(entry.first);
				
				if (result.GetGroupTag() == groupTag)
					return result;
			}

			return TagInstance(0xFFFF);
		}
	};
}
