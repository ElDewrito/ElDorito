#include "HaloStats.hpp"
#include <d3dx9core.h>
#include <algorithm>

#include "../Patches/Network.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Patches/CustomPackets.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ElDorito.hpp"
#include "../Server/ServerChat.hpp"
#include "../ThirdParty/HttpRequest.hpp"
#include <iomanip>
#include <unordered_map>
namespace Server
{
	namespace HaloStatsPackets
	{

		void  ReceivedHaloStatsMessage(Blam::Network::Session *session, int peer, const HaloStatsMessage &message);
		typedef Patches::CustomPackets::Packet<HaloStatsMessage> HaloStatsMessagePacket;
		typedef Patches::CustomPackets::PacketSender<HaloStatsMessage> HaloStatsMessagePacketSender;

		std::shared_ptr<HaloStatsMessagePacketSender> HaloStatsPacketSender;

		// Packet handler for HaloStats messages
		class HaloStatsMessagePacketHandler : public Patches::CustomPackets::PacketHandler<HaloStatsMessage>
		{
		public:
			void Serialize(Blam::BitStream *stream, const HaloStatsMessage *data) override
			{
				// Message type
				stream->WriteUnsigned(static_cast<uint32_t>(data->Type), 0U, static_cast<uint32_t>(HaloStatsMessageType::Count));

				if (data->Type == HaloStatsMessageType::Rank)
					stream->WriteUnsigned<uint8_t>(data->rank, 0, 49);

			}

			bool Deserialize(Blam::BitStream *stream, HaloStatsMessage *data) override
			{
				memset(data, 0, sizeof(*data));

				// Message type
				data->Type = static_cast<HaloStatsMessageType>(stream->ReadUnsigned(0U, static_cast<uint32_t>(HaloStatsMessageType::Count)));
				if (static_cast<uint32_t>(data->Type) >= static_cast<uint32_t>(HaloStatsMessageType::Count))
					return false;

				if (data->Type == HaloStatsMessageType::Rank)
				{
					data->rank = stream->ReadUnsigned<uint8_t>(0, 49);
				}

				return true;
			}

			void HandlePacket(Blam::Network::ObserverChannel *sender, const HaloStatsMessagePacket *packet) override
			{
				auto session = Blam::Network::GetActiveSession();
				if (!session)
					return;
				auto peer = session->GetChannelPeer(sender);
				//if this packet didnt come from the host, ignore it
				if (peer < 0 || peer != session->MembershipInfo.HostPeerIndex)
					return;

				ReceivedHaloStatsMessage(session, peer, packet->Data);

			}
		};
		// Registered message handlers
		std::vector<std::shared_ptr<HaloStatsMessageHandler>> haloStatsMessageHandlers;

		// Callback for when a message is received.
		void ReceivedHaloStatsMessage(Blam::Network::Session *session, int peer, const HaloStatsMessage &message)
		{
			// Send the message out to handlers
			for (auto &&handler : haloStatsMessageHandlers)
				handler->MessageReceived(message);
		}

		// Sends a HaloStats message to a peer as a packet.
		bool SendHaloStatsPacket(const HaloStatsMessage  &message, int peer)
		{
			if (peer < 0)
				return false;
			auto session = Blam::Network::GetActiveSession();
			// if we are trying to send something to ourself, pretend someone else sent it to us. 
			if (peer == session->MembershipInfo.LocalPeerIndex)
			{
				ReceivedHaloStatsMessage(session, peer, message);
				return true;
			}

			auto packet = HaloStatsPacketSender->New();
			packet.Data = message;
			HaloStatsPacketSender->Send(peer, packet);
			return true;
		}

		
		void AddMessageHandler(std::shared_ptr<HaloStatsMessageHandler> handler)
		{
			haloStatsMessageHandlers.push_back(handler);
		}

		class HaloStatsOutputHandler : public HaloStatsMessageHandler
		{
		public:
			HaloStatsOutputHandler(){}

			void MessageReceived(const HaloStatsMessage &message) override
			{
				if (message.Type == HaloStatsMessageType::Rank)
				{

					rapidjson::StringBuffer jsonBuffer;
					rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
					jsonWriter.StartObject();
					jsonWriter.Key("Rank");
					jsonWriter.Int(message.rank);
					jsonWriter.EndObject();
					Web::Ui::ScreenLayer::Notify("HaloStatsRankMessage", jsonBuffer.GetString(), true);
				}

			}

		};
		void Init()
		{
			auto handler = std::make_shared<HaloStatsMessagePacketHandler>();
			HaloStatsPacketSender = Patches::CustomPackets::RegisterPacket<HaloStatsMessage>("eldewrito-halostats-message", handler);
			
		}
	}
}

namespace Web
{
	namespace Ui
	{
		namespace HaloStats
		{

			void Init()
			{
				Server::HaloStatsPackets::AddMessageHandler(std::make_shared<Server::HaloStatsPackets::HaloStatsOutputHandler>());
			}

			void Show()
			{
				ScreenLayer::Show("halostats", "{}");
			}

			void Hide()
			{
				ScreenLayer::Hide("halostats");
			}

		}

	}
}
