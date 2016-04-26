#pragma once
#include <cstdint>
#include <iostream>
#include <string>

namespace Blam
{
	namespace Cache
	{
		struct StringIdCacheHeader
		{
			int32_t StringCount;
			int32_t StringDataSize;
		};

		struct StringIdCache
		{
			StringIdCacheHeader Header;
			char **Strings;

			static StringIdCache Instance;

			bool Load(const std::string &path);
			char *GetString(const int32_t stringId);
		};
	}
}
