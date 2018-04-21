#pragma once

#include <string>

namespace Forge
{
	namespace Prefabs
	{
		struct PrefabHeader
		{
			uint32_t Magic;
			uint16_t Flags;
			uint16_t Version;
			uint64_t DateCreated;
			char Name[16];
			char Author[16];
			uint16_t ObjectCount;
			uint16_t _Padding;
		};

		bool Save(const std::string &author, std::string& name, const std::string& path);
		bool Load(const std::string& path);
		bool ReadHeader(const std::string &path, PrefabHeader &header);
	}
}
