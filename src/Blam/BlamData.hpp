#pragma once

#include <cstdint>

namespace Blam
{
	// A unique handle used to refer to data.
	struct DatumIndex
	{
		// Represents a null datum index.
		static const DatumIndex Null;

		typedef uint16_t TSalt;  // Type of a salt value
		typedef uint16_t TIndex; // Type of an index value

		// Creates a datum index from a handle.
		DatumIndex(uint32_t handle) : Handle(handle) { }

		// Creates a datum index from a salt and an index.
		DatumIndex(TSalt salt, TIndex index)
		{
			Handle = (salt << 16) | index;
		}

		// The value of the datum index as a 32-bit integer.
		uint32_t Handle;

		// Gets the datum index's salt value.
		TSalt Salt() const { return Handle >> 16; }

		// Gets the datum index's index value.
		TIndex Index() const { return Handle & 0xFFFF; }

		bool operator==(const DatumIndex other) const { return Handle == other.Handle; }
		bool operator!=(const DatumIndex other) const { return !(*this == other); }
		operator uint32_t() const { return Handle; }
		operator bool() const { return *this != Null; }
	};
	static_assert(sizeof(DatumIndex) == 4, "Invalid DatumIndex size");

	// Base for allocated data structures.
	struct DatumBase
	{
	private:
		DatumIndex::TSalt salt;

	public:
		explicit DatumBase(DatumIndex::TSalt salt) : salt(salt) { }

		// Gets the datum's salt value.
		DatumIndex::TSalt GetSalt() const { return salt; }

		// Returns true if this datum is null.
		bool IsNull() const { return salt == 0; }
	};
	static_assert(sizeof(DatumBase) == 2, "Invalid DatumBase size");

	// An array of data objects which can be accessed by datum index.
	struct DataArray
	{
		char Name[0x20];            // Name given to the array when it was allocated (e.g. "players")
		int MaxCount;               // The total number of data slots available
		int DatumSize;              // Size of each datum in bytes
		uint8_t Unknown28;
		uint8_t Unknown29;
		uint8_t Flags;
		uint8_t Unknown2B;
		int Signature;              // 'd@t@'
		void *Allocator;
		int NextIndex;              // Index to start searching at to allocate a new datum
		int FirstUnallocated;       // Data starting at this index is guaranteed to be unallocated
		int NumActive;              // Number of used data slots
		DatumIndex::TSalt NextSalt; // Next salt value to use
		uint16_t Unknown42;
		void *Data;                 // The data objects
		uint32_t *UsedSlots;        // Bitarray with 1 bit per slot, where 1 = used and 0 = unused
		int HeaderSize;             // Size of this object
		int TotalSize;              // Total size allocated for the data array

		// Gets a pointer to the datum corresponding to a datum index.
		// Returns null if the datum index does not match a valid datum.
		void* Get(DatumIndex index) const;

		// Gets a typed pointer to the datum corresponding to a datum index.
		// Returns null if the datum index does not match a valid datum.
		template<class TDatum>
		TDatum* Get(DatumIndex index) const { return static_cast<TDatum*>(Get(index)); }
	};
	static_assert(sizeof(DataArray) == 0x54, "Invalid DataArray size");

	// An iterator which iterates over the values in a DataArray.
	struct DataIterator
	{
		// Creates a data iterator for an array.
		explicit DataIterator(DataArray *data) : Array(data), CurrentIndex(-1), CurrentDatumIndex(DatumIndex::Null) { }

		// Moves to the next datum and returns a pointer to it.
		// Returns null if at the end of the array.
		void* Next();

		// Moves to the next datum and returns a typed pointer to it.
		// Returns null if at the end of the array.
		template<class TDatum>
		TDatum* Next() { return static_cast<TDatum*>(Next()); }

		DataArray *Array;             // The data array that the iterator operates on
		int CurrentIndex;             // The index of the current datum
		DatumIndex CurrentDatumIndex; // The datum index of the current datum
	};
	static_assert(sizeof(DataIterator) == 0xC, "Invalid DataIterator size");
}