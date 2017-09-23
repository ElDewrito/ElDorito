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

		void ReadBlock(size_t bits, uint8_t *out);
		void WriteBlock(size_t bits, const uint8_t *data);

		// Serializes a string to the BitStream.
		template<class CharType, size_t MaxSize>
		void WriteString(const CharType(&str)[MaxSize])
		{
			// Compute length
			size_t length = 0;
			while (length < MaxSize - 1 && str[length])
				length++;

			// Write length
			WriteUnsigned<uint64_t>(length, 0U, MaxSize - 1);

			// Write string
			WriteBlock(length * sizeof(CharType) * 8, reinterpret_cast<const uint8_t*>(&str));
		}

		// Deserializes a string from the BitStream.
		// Returns false if the string is invalid.
		template<class CharType, size_t MaxSize>
		bool ReadString(CharType(&str)[MaxSize])
		{
			// Length
			auto length = ReadUnsigned<size_t>(0U, MaxSize - 1);
			if (length >= MaxSize)
				return false;

			// String
			memset(&str, 0, MaxSize * sizeof(CharType));
			ReadBlock(length * sizeof(CharType) * 8, reinterpret_cast<uint8_t*>(&str));
			return true;
		}

		inline int Size() const
		{
			return size;
		}

		inline int Position() const
		{
			return position;
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