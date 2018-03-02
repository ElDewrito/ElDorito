#pragma once
#include <cstdint>
#include <iterator>
#include <type_traits>
#include "Tags\Tag.hpp"

#include "Memory\DatumHandle.hpp"

namespace Blam
{
	int CalculateDatumArraySize(int datumCount, int datumSize, int alignmentBits);

	// Base for structures in a data array.
	struct DatumBase
	{
	private:
		uint16_t salt;

	public:
		explicit DatumBase(uint16_t salt) : salt(salt) { }

		// Gets the datum's salt value.
		uint16_t GetSalt() const { return salt; }

		// Returns true if the datum is null.
		bool IsNull() const { return salt == 0; }
	};
	static_assert(sizeof(DatumBase) == 2, "Invalid DatumBase size");

	// Base struct for data arrays.
	// Consider using DataArray instead of this for type safety.
	struct DataArrayBase
	{
		char Name[0x20];               // Name given to the array when it was allocated (e.g. "players")
		int MaxCount;                  // The total number of data slots available
		int DatumSize;                 // Size of each datum in bytes
		uint8_t Alignment;             // Bit to align datum addresses to (0 = none)
		bool IsValid;                  // true if the array can be used
		uint16_t Flags;                // TODO: Map these out
		Blam::Tags::Tag Signature;                 // 'd@t@'
		void *Allocator;               // Object used to allocate the array
		int NextIndex;                 // Index to start searching at to allocate a new datum
		int FirstUnallocated;          // Data starting at this index is guaranteed to be unallocated
		int ActualCount;               // Number of indices that are actually used
		uint16_t NextSalt;    // Next salt value to use
		uint16_t AltNextSalt; // Alternate next salt value to use (apparently used mainly by effects)
		void *Data;                    // The data objects
		uint32_t *ActiveIndices;       // Bitarray with 1 bit per index, where 1 = used and 0 = unused
		int HeaderSize;                // Size of this object, including padding for alignment
		int TotalSize;                 // Total size allocated for the data array

		// Gets a pointer to the datum corresponding to a datum index.
		// Returns null if the datum index does not match a valid datum.
		DatumBase* Get(DatumHandle index) const;

		// Gets a pointer to the datum corresponding to a datum index.
		// The datum index is NOT checked for validity and this will always succeed.
		// Use Get() if you need validity checking.
		DatumBase* GetAddress(DatumHandle index) const
		{
			auto address = static_cast<uint8_t*>(Data) + index.Index * DatumSize;
			return reinterpret_cast<DatumBase*>(address);
		}

		void Delete(DatumHandle index);
	};
	static_assert(sizeof(DataArrayBase) == 0x54, "Invalid DataArrayBase size");

	struct DataPoolBase
	{
		Blam::Tags::Tag Signature; // 'pool'
		char Name[0x20];           // Name given to the pool when it was allocated
		void** Allocator;
		int Size;
		int FreeSize;
		int Padding;
		int Unknown52;
		int Unknown56;
		uint16_t Unknown60;
		uint8_t Unknown62;
		uint8_t Unknown63;			   // likely IsValid
		int Unk64;
	};
	static_assert(sizeof(DataPoolBase) == 0x44, "Invalid DataPoolBase size");

	struct LruvCacheBase
	{
		char Name[0x20];		// Name given to the cache when it was allocated
		void* Unk32;
		void* Unk36;
		void* Unk40;
		void* Unk44;
		int Unk48;
		int Unk52;
		int Unk56;
		int Unk60;
		int Unk64;
		int Unk68;
		int Unk72;
		int Unk76;
		int Unk80;
		int Unk84;
		int Unk88;
		int Unk92;
		int Unk96;
		int Unk100;
		int Unk104;
		int Unk108;
		int Unk112;
		int Signature;			// 'weee'
		void** Allocator;
		int Unk124;
		int Unk128;
	};
	static_assert(sizeof(LruvCacheBase) == 0x84, "Invalid LruvCacheBase size");

	// Base struct for an iterator which iterates over the values in a data array.
	// Consider using DataIterator instead of this for type safety.
	struct DataIteratorBase
	{
		// Creates a data iterator for an array.
		explicit DataIteratorBase(const DataArrayBase *data) : Array(data), CurrentDatumIndex(DatumHandle::Null), CurrentIndex(-1) { }

		// Moves to the next datum and returns a pointer to it.
		// Returns null if at the end of the array.
		DatumBase* Next();

		const DataArrayBase *Array;   // The data array that the iterator operates on
		DatumHandle CurrentDatumIndex; // The datum index of the current datum
		int CurrentIndex;             // The index of the current datum
	};
	static_assert(sizeof(DataIteratorBase) == 0xC, "Invalid DataIteratorBase size");

	template<class TDatum> struct DataIterator;
	template<class TDatum> struct ConstDataIterator;

	// Type-safe data array struct.
	// TDatum MUST inherit from the DatumBase struct.
	template<class TDatum>
	struct DataArray : DataArrayBase
	{
		static_assert(std::is_base_of<DatumBase, TDatum>::value, "TDatum must inherit from DatumBase");

		// Gets a pointer to the datum corresponding to a datum index.
		// Returns null if the datum index does not match a valid datum.
		TDatum* Get(DatumHandle index) const { return static_cast<TDatum*>(DataArrayBase::Get(index)); }

		// Gets a reference to the datum corresponding to a datum index.
		// The datum index is NOT checked for validity and this will always succeed.
		// Use Get() if you need validity checking.
		TDatum& operator[](DatumHandle index) const { return *static_cast<TDatum*>(GetAddress(index)); }

		// Gets an iterator pointing to the beginning of this data array.
		DataIterator<TDatum> begin()
		{
			DataIterator<TDatum> result(this);
			result.Next();
			return result;
		}

		// Gets a const iterator pointing to the beginning of this data array.
		ConstDataIterator<TDatum> begin() const
		{
			ConstDataIterator<TDatum> result(this);
			result.Next();
			return result;
		}

		// Gets a const iterator pointing to the beginning of this data array.
		ConstDataIterator<TDatum> cbegin() const { return begin(); }

		// Gets an iterator pointing to the end of this data array.
		DataIterator<TDatum> end()
		{
			DataIterator<TDatum> result(this);
			result.CurrentIndex = MaxCount;
			return result;
		}

		// Gets a const iterator pointing to the end of this data array.
		ConstDataIterator<TDatum> end() const
		{
			ConstDataIterator<TDatum> result(this);
			result.CurrentIndex = MaxCount;
			return result;
		}

		// Gets a const iterator pointing to the end of this data array.
		ConstDataIterator<TDatum> cend() const { return end(); }
	};
	static_assert(sizeof(DataArray<DatumBase>) == sizeof(DataArrayBase), "Invalid DataArray size");

	// Type-safe struct for a forward iterator which iterates over the values in a DataArray.
	template<class TDatum>
	struct DataIterator : DataIteratorBase, std::iterator<std::forward_iterator_tag, TDatum>
	{
		static_assert(std::is_base_of<DatumBase, TDatum>::value, "TDatum must inherit from DatumBase");

		// Creates a data iterator for an array.
		explicit DataIterator(DataArray<TDatum> *data) : DataIteratorBase(data) { }

		// Moves to the next datum and returns a pointer to it.
		// Returns null if at the end of the array.
		TDatum* Next() { return static_cast<TDatum*>(DataIteratorBase::Next()); }

		DataIterator() : DataIteratorBase(nullptr) { }
		DataIterator<TDatum>& operator++() { Next(); return *this; }
		DataIterator<TDatum> operator++(int) { auto result = *this; operator++(); return result; }
		TDatum* operator->() const { return static_cast<TDatum*>(Array->GetAddress(CurrentDatumIndex)); }
		TDatum& operator*() const { return *operator->(); }
		bool operator==(const DataIterator<TDatum> &rhs) const { return Array == rhs.Array && CurrentIndex == rhs.CurrentIndex && CurrentDatumIndex == rhs.CurrentDatumIndex; }
		bool operator!=(const DataIterator<TDatum> &rhs) const { return !(*this == rhs); }
	};
	static_assert(sizeof(DataIterator<DatumBase>) == sizeof(DataIteratorBase), "Invalid DataIterator size");

	// Type-safe struct for a const forward iterator which iterates over the values in a DataArray.
	template<class TDatum>
	struct ConstDataIterator : DataIteratorBase, std::iterator<std::forward_iterator_tag, TDatum>
	{
		static_assert(std::is_base_of<DatumBase, TDatum>::value, "TDatum must inherit from DatumBase");

		// Creates a const data iterator for an array.
		explicit ConstDataIterator(const DataArray<TDatum> *data) : DataIteratorBase(data) { }

		// Moves to the next datum and returns a pointer to it.
		// Returns null if at the end of the array.
		const TDatum* Next() { return static_cast<TDatum*>(DataIteratorBase::Next()); }

		ConstDataIterator() : DataIteratorBase(nullptr) { }
		ConstDataIterator<TDatum>& operator++() { Next(); return *this; }
		ConstDataIterator<TDatum> operator++(int) { auto result = *this; operator++(); return result; }
		const TDatum* operator->() const { return static_cast<TDatum*>(Array->GetAddress(CurrentDatumIndex)); }
		const TDatum& operator*() const { return *operator->(); }
		bool operator==(const ConstDataIterator<TDatum> &rhs) const { return Array == rhs.Array && CurrentIndex == rhs.CurrentIndex && CurrentDatumIndex == rhs.CurrentDatumIndex; }
		bool operator!=(const ConstDataIterator<TDatum> &rhs) const { return !(*this == rhs); }
	};
	static_assert(sizeof(ConstDataIterator<DatumBase>) == sizeof(DataIteratorBase), "Invalid ConstDataIterator size");
}