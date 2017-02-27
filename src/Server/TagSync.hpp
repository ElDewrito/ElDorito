#include <string>
#include <cstdint>

namespace Server
{
	namespace TagSync
	{
		void Initialize();
		void Tick();

		enum SyncType
		{
			eSyncAutoAim,
			eSyncRangeShort,
			eSyncRangeLong,

			eSyncCount
		};

		struct TagSyncPacketVar
		{
			uint16_t TagIndex;
			SyncType Type;
			union//add more as needed
			{
				float Float;
			} Value;
		};

		struct TagSyncPacketData
		{
		};

		struct Tag
		{
			uint16_t index;
			SyncType type;
			bool operator==(const Tag& other) const
			{
				return index == other.index && type == other.type;
			}
		};
	}
}