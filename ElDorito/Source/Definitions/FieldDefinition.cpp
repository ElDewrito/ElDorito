#include "FieldDefinition.hpp"

namespace Definitions
{
	FieldDefinition::FieldDefinition(const FieldType &type) :
		FieldDefinition(type, "")
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const long length) :
		FieldDefinition(type, "", length)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name) :
		FieldDefinition(type, name, 1)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name, const long length) :
		Type(type), Name(name), Length(length)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name, const EnumDefinition *enumDef) :
		Type(type), Name(name), Length(1), Enum(enumDef)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name, const StructDefinition *structDef) :
		FieldDefinition(type, name, 1, structDef)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name, const long length, const StructDefinition *structDef) :
		Type(type), Name(name), Length(length), Struct(structDef)
	{
	}
}