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
		return *(Tag *)((*TagTablePtr)[(*TagIndexTablePtr)[Index]] + 0x14);
	}
}