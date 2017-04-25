#include "TagInstance.hpp"

namespace Blam
{
	namespace Tags
	{
		TagInstance::TagInstance(const uint16_t index)
			: Index(index)
		{
		}

		uint32_t TagInstance::GetGroupTag()
		{
			typedef int(*GetGroupTagPtr)(uint16_t);
			auto GetGroupTagImpl = reinterpret_cast<GetGroupTagPtr>(0x5033A0);

			return GetGroupTagImpl(Index);
		}
	}
}
