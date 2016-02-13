#include "BlamData.hpp"

namespace Blam
{
	const DatumIndex DatumIndex::Null(0xFFFFFFFF);

	void* DataArray::Get(DatumIndex index) const
	{
		typedef void*(*DataArrayGetPtr)(const DataArray *data, DatumIndex index);
		auto DataArrayGet = reinterpret_cast<DataArrayGetPtr>(0x55B6D0);
		return DataArrayGet(this, index);
	}

	void* DataIterator::Next()
	{
		typedef void*(*DataIteratorNextPtr)(DataIterator *iterator);
		auto DataIteratorNext = reinterpret_cast<DataIteratorNextPtr>(0x55AE30);
		return DataIteratorNext(this);
	}
}