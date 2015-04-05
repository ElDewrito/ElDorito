#include "Patch.h"

#include <algorithm>

Patch::Patch(size_t Offset, const InitializerListType &Data, const InitializerListType &Reset /*= {}*/)
	: Offset(Offset),
	Data(Data),
	Orig(Reset)
{
}

Patch::Patch() :
Offset(0)
{
}

Patch::Patch(size_t Offset, const InitializerListType &Data, Pointer base /*= Pointer::Base()*/)
	: Offset(Offset),
	Data(Data)
{
	Orig.resize(Data.size());
	base(Offset).Read(Orig.data(), Orig.size());
}

Patch::Patch(size_t Offset, size_t byteValue, size_t numBytes, Pointer base /*= Pointer::Base()*/)
	: Offset(Offset)
{
	// Fill with NOPs
	Data.resize(numBytes);
	std::fill(Data.begin(), Data.end(), byteValue);

	Orig.resize(Data.size());
	base(Offset).Read(Orig.data(), Orig.size());
}