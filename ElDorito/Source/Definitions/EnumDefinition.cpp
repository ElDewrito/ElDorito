#include "EnumDefinition.hpp"

namespace Definitions
{
	EnumOption::EnumOption(const std::string &name, const long value) :
		Name(name), Value(value)
	{
	}

	EnumDefinition::EnumDefinition(const std::string &name, const EnumType &type, const std::initializer_list<EnumOption> &options) :
		Name(name), Type(type), Options(options)
	{
	}
}