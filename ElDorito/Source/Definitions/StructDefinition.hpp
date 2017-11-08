#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <initializer_list>
#include "FieldDefinition.hpp"

namespace Definitions
{
	struct StructDefinition
	{
		const std::string Name;
		const size_t Size;
		const std::vector<FieldDefinition> Fields;

		StructDefinition(const std::string &name, const size_t size, const std::initializer_list<FieldDefinition> &fields);
	};
}