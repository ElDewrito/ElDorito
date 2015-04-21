#include "BitStream.h"

namespace Blam
{
	bool BitStream::ReadBit()
	{
		typedef bool (__thiscall* ReadBitPtr)(BitStream *thisPtr);
		ReadBitPtr ReadBit = reinterpret_cast<ReadBitPtr>(0x558570);
		return ReadBit(this);
	}

	uint64_t BitStream::ReadBits(int bits)
	{
		typedef uint64_t (__thiscall* ReadBitsPtr)(BitStream *thisPtr, int bits);
		ReadBitsPtr ReadBits = reinterpret_cast<ReadBitsPtr>(0x5589A0);
		return ReadBits(this, bits);
	}

	void BitStream::WriteBits(uint64_t val, int bits)
	{
		if (BitsAvailable(bits))
		{
			position += bits;
			windowBitsUsed += bits;
			window <<= bits;
			window |= val & (0xFFFFFFFFFFFFFFFF >> (64 - bits));
		}
		else
		{
			typedef void (__thiscall* WriteBitsPtr)(BitStream *thisPtr, uint64_t val, int bits);
			WriteBitsPtr WriteBits = reinterpret_cast<WriteBitsPtr>(0x559EB0);
			return WriteBits(this, val, bits);
		}
	}
}