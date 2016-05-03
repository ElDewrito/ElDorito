#include <fstream>
#include "StringIDCache.hpp"

namespace Blam
{
	namespace Cache
	{
		StringIDCache StringIDCache::Instance;

		StringIDCache::StringIDCache()
			: Header(), Data(nullptr), Strings(nullptr)
		{
		}

		StringIDCache::~StringIDCache()
		{
			if (Strings != nullptr)
				delete Strings;

			if (Data != nullptr)
				delete Data;
		}

		bool StringIDCache::Load(const std::string &path)
		{
			std::ifstream stream;
			stream.open(path, std::ios::binary);

			if (!stream.good())
				return false;

			stream.read((char *)&Header, sizeof(StringIDCacheHeader));

			Data = new char[Header.StringDataSize + 1];
			Strings = new char *[Header.StringCount];

			auto *stringOffsets = new int32_t[Header.StringCount];
			stream.read((char *)stringOffsets, sizeof(int32_t) * Header.StringCount);

			auto dataOffset = stream.tellg().seekpos();
			stream.read(Data, Header.StringDataSize);
			Data[Header.StringDataSize] = '\0';

			for (auto i = 0; i < Header.StringCount; i++)
			{
				if (stringOffsets[i] < 0 || stringOffsets[i] >= Header.StringDataSize)
				{
					Strings[i] = nullptr;
					continue;
				}

				Strings[i] = Data + stringOffsets[i];
			}

			stream.close();
			delete stringOffsets;

			return true;
		}

		char *StringIDCache::GetString(const int32_t StringID)
		{
			int32_t setMin = 0x1;
			int32_t setMax = 0xF1E;
			int32_t setOffsets[] = { 0x90F, 0x1, 0x685, 0x720, 0x7C4, 0x778, 0x7D0, 0x8EA, 0x902 };

			int32_t set = (int32_t)((StringID >> 16) & 0xFF);
			int32_t index = (int32_t)(StringID & 0xFFFF);

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
