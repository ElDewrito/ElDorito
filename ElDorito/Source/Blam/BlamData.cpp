#include "BlamData.hpp"

namespace Blam
{

	DatumBase* DataArrayBase::Get(DatumHandle index) const
	{
		typedef DatumBase*(*DataArrayGetPtr)(const DataArrayBase *data, DatumHandle index);
		auto DataArrayGet = reinterpret_cast<DataArrayGetPtr>(0x55B6D0);
		return DataArrayGet(this, index);
	}

	void DataArrayBase::Delete(DatumHandle index)
	{
		const auto datum_array_free_datum = (void(*)(void *a1, unsigned __int16 datumIndex))(0x0055B2E0);
		datum_array_free_datum(this, index.Index);
		return;

		if (index.Index == 0xffff)
			return;

		auto datum = (Blam::DatumBase*)((uint8_t*)this->Data + index.Index * this->DatumSize);
		if ((this->Flags >> 3) & 1)
			memset(datum, 0xBA, this->DatumSize);
		this->ActiveIndices[index.Index >> 5] &= ~(1 << index.Index & 0x1F);
		*(uint16_t *)datum = 0;
		if (index.Index < this->NextIndex)
			this->NextIndex = index.Index;
		auto nextIndex = index.Index + 1;
		if (nextIndex == this->FirstUnallocated)
		{
			auto p = (uint8_t*)datum;
			do
			{
				--this->FirstUnallocated;
				p -= this->DatumSize;	
			} 
			while (this->FirstUnallocated > 0 && !*(uint16_t *)datum);
		}
		--this->ActualCount;
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