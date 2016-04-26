#include <fstream>
#include "StringIdCache.hpp"
#include <Windows.h>

namespace Blam
{
	namespace Cache
	{
		StringIdCache StringIdCache::Instance;

		bool StringIdCache::Load(const std::string &path)
		{
			char buffer[256];

			std::ifstream stream;
			stream.open(path, std::ios::binary);

			if (!stream.good())
				return false;

			stream.read((char *)&Header, sizeof(StringIdCacheHeader));

			Strings = new char *[Header.StringCount];
			auto *stringOffsets = new int32_t[Header.StringCount];

			stream.read((char *)stringOffsets, sizeof(int32_t) * Header.StringCount);

			auto dataOffset = stream.tellg().seekpos();

			for (auto i = 0; i < Header.StringCount; i++)
			{
				if (stringOffsets[i] < 0 || stringOffsets[i] >= Header.StringDataSize)
					continue;

				stream.seekg(dataOffset + stringOffsets[i], std::ios::beg);

				auto stringLength = 0;
				while (stream.get(buffer[stringLength]) && buffer[stringLength++] != '\0');

				Strings[i] = new char[stringLength];
				std::copy(buffer, buffer + stringLength, Strings[i]);
			}

			stream.close();
			delete stringOffsets;

			return true;
		}

		char *StringIdCache::GetString(const int32_t stringId)
		{
			int32_t setMin = 0x1;
			int32_t setMax = 0xF1E;
			int32_t setOffsets[] = { 0x90F, 0x1, 0x685, 0x720, 0x7C4, 0x778, 0x7D0, 0x8EA, 0x902 };

			int32_t set = (int32_t)((stringId >> 16) & 0xFF);
			int32_t index = (int32_t)(stringId & 0xFFFF);

			if (set == 0 && (index < setMin || index > setMax))
				return Strings[index];

			if (set < 0 || set >= 9)
				return nullptr;

			if (set == 0)
				index -= setMin;

			return Strings[index + setOffsets[set]];
		}
	}
}
