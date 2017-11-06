#pragma once

#include <string>

namespace Forge
{
	class ObjectSet;

	namespace Prefabs
	{
		bool Save(const std::string& name, const std::string& path);
		bool Load(const std::string& path);
	}
}
