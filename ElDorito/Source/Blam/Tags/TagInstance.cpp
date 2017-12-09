#include "TagInstance.hpp"

namespace Blam::Tags
{
	std::unordered_map<int32_t, std::string> TagInstance::TagNames = std::unordered_map<int32_t, std::string>();

	TagInstance::TagInstance(const uint16_t index)
		: Index(index)
	{
	}

	Tag TagInstance::GetGroupTag()
	{
		auto tagTableIndex = (*TagIndexTablePtr)[Index];
		auto *tagTableEntry = (*TagTablePtr)[tagTableIndex];

		if (tagTableEntry == nullptr)
			return -1;

		return *(Tag *)(tagTableEntry + 0x14);
	}
}
