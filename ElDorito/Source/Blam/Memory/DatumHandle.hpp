#pragma once
#include <cstdint>

namespace Blam
{
	struct DatumHandle
	{
		static const DatumHandle Null;

		union
		{
			struct { uint32_t Handle; };
			struct { uint16_t Index, Salt; };
		};

		DatumHandle();
		DatumHandle(const uint32_t handle);
		DatumHandle(const uint16_t index, const uint16_t salt);
		DatumHandle(const DatumHandle &other);

		bool operator==(const DatumHandle &other) const;
		bool operator!=(const DatumHandle &other) const;

		operator uint32_t() const;

		operator bool() const;
	};
	static_assert(sizeof(DatumHandle) == 4);
}