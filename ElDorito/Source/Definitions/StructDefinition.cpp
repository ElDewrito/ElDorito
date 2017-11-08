#include "StructDefinition.hpp"

namespace Definitions
{
	StructDefinition::StructDefinition(const std::string &name, const size_t size, const std::initializer_list<FieldDefinition> &fields) :
		Name(name), Size(size), Fields(fields)
	{
	}
}