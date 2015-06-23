#pragma once

#include "Pointer.hpp"

#include <initializer_list>
#include <array>
#include <vector>

// Todo: Stray away from vectors, use initializer list to stick to compile-time constant data
//       without any run-time overhead. Optimized code should propogate to simple memcpys

class Patch
{
public:
	typedef std::initializer_list<uint8_t> InitializerListType;

	Patch();
	Patch(size_t Offset,
		const InitializerListType &Data,
		const InitializerListType &Reset);
	Patch(size_t Offset,
		const InitializerListType &Data,
		Pointer base = Pointer::Base());

	// Constructor fills data with a given byte
	Patch(size_t Offset, size_t byteValue, size_t numBytes, Pointer base = Pointer::Base());

	inline void Apply(bool revert = false, Pointer offset = Pointer::Base()) const
	{
		if (revert)
			return Reset(offset);

		if( Data.size() )
			offset(Offset).Write(&Data[0], Data.size());
	}

	inline void Reset(Pointer offset = Pointer::Base()) const
	{
		if( Orig.size() )
			offset(Offset).Write(&Orig[0], Orig.size());
	}

	static inline void NopFill(Pointer offset, size_t length)
	{
		memset(offset, 0x90, length);
	}
private:
	size_t Offset;
	std::vector<uint8_t> Data, Orig;
};

class Hook
{
public:
	Hook();
	Hook(size_t Offset, void* destFunc, int flags = 0, std::initializer_list<uint8_t> Reset = {});

	inline void Apply(bool revert = false, Pointer offset = Pointer::Base()) const
	{
		if (revert)
			return Reset(offset);

		if (Flags & HookFlags::IsCall)
			offset(Offset).WriteCall(DestFunc);
		else
			offset(Offset).WriteJump(DestFunc, Flags);
	}

	inline void Reset(Pointer offset = Pointer::Base()) const
	{
		if( Orig.size() )
			offset(Offset).Write(&Orig[0], Orig.size());
	}

private:
	size_t Offset;
	void* DestFunc;
	const int Flags;
	std::vector<uint8_t> Orig;
};