#pragma once

#include <cstdint>
#include <bitset>

namespace Forge
{
	class ObjectSet
	{
	public:
		bool Contains(uint32_t objectIndex) const
		{
			return objectIndex != -1 && m_Bits.test(objectIndex & 0xFFFF);
		}

		int Count() const
		{
			return m_Bits.count();
		}

		bool Any() const
		{
			return m_Bits.any();
		}

		void Add(uint32_t objectIndex)
		{
			m_Bits.set(objectIndex & 0xFFFF);
		}

		void Remove(uint32_t objectIndex)
		{
			m_Bits.set(objectIndex & 0xFFFF, false);
		}

		void Clear()
		{
			m_Bits.reset();
		}

	private:
		std::bitset<2048> m_Bits;
	};
}
