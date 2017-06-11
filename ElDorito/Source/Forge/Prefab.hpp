#pragma once

#include <string>

namespace Forge
{
	class ObjectSet;

	namespace Prefabs
	{
		bool Save(const ObjectSet& objectSet, const std::string& name, const std::string& path);
		bool Load(ObjectSet& objectSet, const std::string& path);
	}
}
