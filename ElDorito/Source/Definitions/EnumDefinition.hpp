#pragma once
#include <string>
#include <vector>
#include <initializer_list>

namespace Definitions
{
	enum class EnumType
	{
		Char,
		Short,
		Long,
		Byte,
		Word,
		DWord
	};

	struct EnumOption
	{
		const std::string Name;
		const long Value;

		EnumOption(const std::string &name, const long value);
	};

	struct EnumDefinition
	{
		const std::string Name;
		const EnumType Type;
		const std::vector<EnumOption> Options;

		EnumDefinition(const std::string &name, const EnumType &type, const std::initializer_list<EnumOption> &options);
	};
}