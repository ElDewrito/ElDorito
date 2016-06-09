#include "BlamData.hpp"

namespace Blam
{
	const DatumIndex DatumIndex::Null;

	DatumBase* DataArrayBase::Get(DatumIndex index) const
	{
		typedef DatumBase*(*DataArrayGetPtr)(const DataArrayBase *data, DatumIndex index);
		auto DataArrayGet = reinterpret_cast<DataArrayGetPtr>(0x55B6D0);
		return DataArrayGet(this, index);
	}

	DatumBase* DataIteratorBase::Next()
	{
		typedef DatumBase*(*DataIteratorNextPtr)(DataIteratorBase *iterator);
		auto DataIteratorNext = reinterpret_cast<DataIteratorNextPtr>(0x55AE30);
		return DataIteratorNext(this);
	}

	int CalculateDatumArraySize(int datumCount, int datumSize, int alignmentBits)
	{
		return reinterpret_cast<int(__cdecl*)(int, int, int)>(0x55AAB0)(datumCount, datumSize, alignmentBits);
	}
}