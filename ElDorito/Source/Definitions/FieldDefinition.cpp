#include "FieldDefinition.hpp"

namespace Definitions
{
	FieldDefinition::FieldDefinition(const FieldType &type) :
		FieldDefinition(type, "")
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const long length) :
		FieldDefinition(type, FieldType::Skip, "", length)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name) :
		FieldDefinition(type, FieldType::Skip, name, 1)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name, const EnumDefinition *enumDef) :
		FieldDefinition(type, FieldType::Skip, name, 1, enumDef)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const std::string &name, const StructDefinition *structDef) :
		FieldDefinition(type, FieldType::Skip, name, 1, structDef)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const FieldType &arrayType, const std::string &name, const long length) :
		Type(type), ArrayType(arrayType), Name(name), Length(length)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const FieldType &arrayType, const std::string &name, const long length, const EnumDefinition *enumDef) :
		Type(type), ArrayType(arrayType), Name(name), Length(length), Enum(enumDef)
	{
	}

	FieldDefinition::FieldDefinition(const FieldType &type, const FieldType &arrayType, const std::string &name, const long length, const StructDefinition *structDef) :
		Type(type), ArrayType(arrayType), Name(name), Length(length), Struct(structDef)
	{
	}
}