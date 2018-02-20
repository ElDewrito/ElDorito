#include "DatumHandle.hpp"

namespace Blam
{
	const DatumHandle DatumHandle::Null = -1;

	DatumHandle::DatumHandle() :
		DatumHandle(Null)
	{
	}

	DatumHandle::DatumHandle(const uint32_t handle) :
		Handle(handle)
	{
	}

	DatumHandle::DatumHandle(const uint16_t index, const uint16_t salt) :
		Index(index), Salt(salt)
	{
	}

	DatumHandle::DatumHandle(const DatumHandle &other) :
		DatumHandle(other.Handle)
	{
	}

	bool DatumHandle::operator==(const DatumHandle &other) const
	{
		return Handle == other.Handle;
	}

	bool DatumHandle::operator!=(const DatumHandle &other) const
	{
		return !(*this == other);
	}

	DatumHandle::operator uint32_t() const
	{
		return Handle;
	}

	DatumHandle::operator bool() const
	{
		return *this != Null;
	}
}