#pragma once
#include <string>

namespace Definitions
{
	enum class FieldType
	{
		Tag,
		String,
		LongString,
		StringID,
		CharInteger,
		ShortInteger,
		LongInteger,
		Int64Integer,
		ByteInteger,
		WordInteger,
		DwordInteger,
		QwordInteger,
		CharEnum,
		ShortEnum,
		LongEnum,
		ByteFlags,
		WordFlags,
		LongFlags,
		Angle,
		Point2D,
		Rectangle2D,
		RgbColor,
		ArgbColor,
		Real,
		RealFraction,
		RealPoint2D,
		RealPoint3D,
		RealVector2D,
		RealVector3D,
		RealQuaternion,
		RealEulerAngles2D,
		RealEulerAngles3D,
		RealPlane2D,
		RealPlane3D,
		RealRgbColor,
		RealArgbColor,
		RealHsvColor,
		RealAhsvColor,
		ShortIntegerBounds,
		AngleBounds,
		RealBounds,
		FractionBounds,
		TagReference,
		Block,
		CharBlockIndex,
		ShortBlockIndex,
		LongBlockIndex,
		Data,
		PageableResource,
		Pad,
		Skip,
		Explanation,
		Custom,
		Struct,
		Array
	};

	struct EnumDefinition;
	struct StructDefinition;

	struct FieldDefinition
	{
		const FieldType Type;
		const std::string Name;
		const long Length;
		union
		{
			const EnumDefinition *Enum;
			const StructDefinition *Struct;
		};

		FieldDefinition(const FieldType &type);
		FieldDefinition(const FieldType &type, const long length);
		FieldDefinition(const FieldType &type, const std::string &name);
		FieldDefinition(const FieldType &type, const std::string &name, const long length);
		FieldDefinition(const FieldType &type, const std::string &name, const EnumDefinition *enumDef);
		FieldDefinition(const FieldType &type, const std::string &name, const StructDefinition *structDef);
		FieldDefinition(const FieldType &type, const std::string &name, const long length, const StructDefinition *structDef);
	};
}