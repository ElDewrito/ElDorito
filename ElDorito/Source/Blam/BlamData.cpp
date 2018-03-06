#include "BlamData.hpp"

namespace Blam
{
	DatumBase* DataArrayBase::Get(DatumHandle index) const
	{
		if (index.Index == 0xffff || index.Index >= this->FirstUnallocated)
			return nullptr;

		auto datum = this->GetAddress(index);
		auto salt = datum->GetSalt();
		if (!salt || salt != index.Salt)
			return nullptr;

		return datum;
	}

	void DataArrayBase::Delete(DatumHandle index)
	{
		uint8_t *datum;
		datum = ((uint8_t*)this->Data + index.Index * this->DatumSize);
		if (((this->Flags & 0xff) >> 3) & 1)
			memset(datum, 0xBA, this->DatumSize);
		this->ActiveIndices[index.Index >> 5] &= ~(1 << (index.Index & 0x1F));
		*datum = 0;
		if (index.Index < this->NextIndex)
			this->NextIndex = index;
		if ((index.Index + 1) == this->FirstUnallocated)
		{
			do
			{
				--this->FirstUnallocated;
				datum -= this->DatumSize;
			} while (this->FirstUnallocated > 0 && !*datum);
		}
		--this->ActualCount;
	}

	uint32_t DataArray_GetNextIndex(const DataArrayBase *data, int index)
	{
		auto result = index;
		if (index < 0 || index >= data->FirstUnallocated)
			return -1;

		while (!((1 << (result & 0x1F)) & data->ActiveIndices[result >> 5]))
		{
			if (++result >= data->FirstUnallocated)
				return -1;
		}

		return result;
	}

	DatumBase* DataIteratorBase::Next()
	{
		auto index = DataArray_GetNextIndex(this->Array, this->CurrentIndex + 1);

		if (index == -1)
		{
			this->CurrentIndex = this->Array->MaxCount;
			this->CurrentDatumIndex = -1;
			return nullptr;
		}
		else
		{
			auto datum = (Blam::DatumBase*)((uint8_t*)this->Array->Data + index * this->Array->DatumSize);
			this->CurrentIndex = index;
			this->CurrentDatumIndex = Blam::DatumHandle(index, datum->GetSalt());
			return datum;
		}		
	}

	int CalculateDatumArraySize(int datumCount, int datumSize, int alignmentBits)
	{
		return reinterpret_cast<int(__cdecl*)(int, int, int)>(0x55AAB0)(datumCount, datumSize, alignmentBits);
	}
}