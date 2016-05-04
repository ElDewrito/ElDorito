#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include "../Text/StringID.hpp"

namespace Blam
{
	namespace Cache
	{
		struct StringIDCacheHeader
		{
			int32_t StringCount;
			int32_t StringDataSize;
		};

		struct StringIDCache
		{
			StringIDCacheHeader Header;
			char *Data;
			char **Strings;

			StringIDCache();
			~StringIDCache();

			static StringIDCache Instance;

			bool Load(const std::string &path);
			char *GetString(const uint32_t stringID);
			char *GetString(const Blam::Text::StringID &stringID);
		};
	}
}
