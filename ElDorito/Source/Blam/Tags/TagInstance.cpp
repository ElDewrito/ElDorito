#include "TagInstance.hpp"

namespace Blam::Tags
{
	TagInstance::TagInstance(const uint16_t index)
		: Index(index)
	{
	}

	uint32_t TagInstance::GetGroupTag()
	{
		return *(uint32_t*)((*TagTablePtr)[(*TagIndexTablePtr)[Index]] + 0x14);
	}
}
