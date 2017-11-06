#include "Patch.hpp"

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
	std::fill(Data.begin(), Data.end(), (unsigned char)byteValue);

	Orig.resize(Data.size());
	base(Offset).Read(Orig.data(), Orig.size());
}

Hook::Hook() :
	Offset(0), Flags(0)
{
	Orig.resize(5);
	Pointer::Base(Offset).Read(Orig.data(), Orig.size());
}

Hook::Hook(size_t Offset, void* destFunc, int flags, std::initializer_list<uint8_t> Reset) :
	Offset(Offset), DestFunc(destFunc), Flags(flags), Orig(Reset)
{
	if (Orig.size() <= 0)
	{
		Orig.resize(((Flags & HookFlags::IsJmpIfEqual) ? 6 : 5));
		Pointer::Base(Offset).Read(Orig.data(), Orig.size());
	}
}