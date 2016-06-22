#pragma once
#include <string>
#include <d3dx9core.h>
#include <chrono>
#include "../Blam/BlamEvents.hpp"
#include "../Patches/Events.hpp"
#include <vector>
#include <memory>
#include "../Utils/Utils.hpp"

namespace Web
{
	namespace Ui
	{
		namespace HaloStats
		{
			void Init();
			void Show();
			void Hide();
		}
	}
}

namespace Server
{
	namespace HaloStatsPackets
	{
		void Init();
		enum class HaloStatsMessageType : uint32_t
		{
			// A request packet for the client to send the values
			Rank,

			Achievement,

			Banned,

			// valid message types.
			Count
		};
		
		// Voting Message Data
		struct HaloStatsMessage
		{
			HaloStatsMessage() { }

			HaloStatsMessage(HaloStatsMessageType type);
			// The message type.
			HaloStatsMessageType Type;

			int rank;

			int achievementID;

		};
		class HaloStatsMessageHandler
		{
		public:
			virtual ~HaloStatsMessageHandler() { }

			// Called after a message has been received.
			virtual void MessageReceived(const HaloStatsMessage &message) = 0;
		};

		void AddMessageHandler(std::shared_ptr<HaloStatsMessageHandler> handler);

		
	}
}