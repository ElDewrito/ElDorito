#pragma once

#include "../Utils/Bits.hpp"

#include <cstdint>

namespace Blam
{
	// Interface for the bitstream objects the engine uses
	class BitStream
	{
	public:
		// TODO: Add a constructor

		bool ReadBool()
		{
			return ReadBit();
		}

		template<class T>
		T ReadUnsigned(int bits)
		{
			return static_cast<T>(ReadBits(bits));
		}

		template<class T>
		T ReadUnsigned(T minValue, T maxValue)
		{
			return static_cast<T>(ReadUnsigned<T>(Utils::Bits::CountBits(maxValue - minValue)));
		}

		void WriteBool(bool b)
		{
			WriteUnsigned(b, 1);
		}

		template<class T>
		void WriteUnsigned(T val, int bits)
		{
			WriteBits(static_cast<uint64_t>(val), bits);
		}

		template<class T>
		void WriteUnsigned(T val, T minValue, T maxValue)
		{
			WriteUnsigned(val - minValue, Utils::Bits::CountBits(maxValue - minValue));
		}

	private:
		uint8_t *start;      // 0x00
		uint8_t *end;        // 0x04
		uint32_t size;       // 0x08
		uint32_t unkC;       // 0x0C
		uint32_t unk10;      // 0x10 - mode?
		void *unk14;         // 0x14
		uint32_t unk18;      // 0x18
		uint32_t position;   // 0x1C
		uint64_t window;     // 0x20
		int windowBitsUsed;  // 0x28
		uint8_t *currentPtr; // 0x2C
		uint32_t unk30;      // 0x30
		uint8_t unk34[100];  // 0x34 - debug info? the constructor doesn't initialize this
		uint32_t unk98;      // 0x98
		uint32_t unk9C;      // 0x9C

		int GetAvailableBits() const { return sizeof(window) * 8 - windowBitsUsed; }
		bool BitsAvailable(int bits) const { return bits <= GetAvailableBits(); }

		bool ReadBit();
		uint64_t ReadBits(int bits);
		void WriteBits(uint64_t val, int bits);
	};
}