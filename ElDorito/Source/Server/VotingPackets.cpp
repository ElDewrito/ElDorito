#include "VotingPackets.hpp"
#include "../Patches/CustomPackets.hpp"
#include "../Server/Voting.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Utils/Utils.hpp"


using namespace Server::Voting;

namespace
{
	typedef Patches::CustomPackets::Packet<VotingMessage> VotingMessagePacket;
	typedef Patches::CustomPackets::PacketSender<VotingMessage> VotingMessagePacketSender;

	std::shared_ptr<VotingMessagePacketSender> VotingPacketSender;
	void ReceivedVotingMessage(Blam::Network::Session *session, int peer, const VotingMessage &message);


	// Packet handler for voting messages
	class VotingMessagePacketHandler : public Patches::CustomPackets::PacketHandler<VotingMessage>
	{
	public:
		void Serialize(Blam::BitStream *stream, const VotingMessage *data) override
		{
			// Message type
			stream->WriteUnsigned(static_cast<uint32_t>(data->Type), 0U, static_cast<uint32_t>(VotingMessageType::Count));

			if (data->Type == VotingMessageType::VotingOptions || data->Type == VotingMessageType::VetoOption)
			{
				for (int i = 0; i < 5; i++)
				{
					stream->WriteString(data->votingOptions[i].mapName);
					stream->WriteString(data->votingOptions[i].typeName);
					stream->WriteBool(data->votingOptions[i].canVeto);
					stream->WriteUnsigned<uint16_t>(data->votingOptions[i].mapId, 0, 10000);
				}

				stream->WriteUnsigned<uint8_t>(data->voteTime, 0, 120);
			}

			if (data->Type == VotingMessageType::Vote || data->Type == VotingMessageType::VetoVote)
				stream->WriteUnsigned<uint8_t>(data->Vote, 0, 5);

			if (data->Type == VotingMessageType::VoteTally)
			{
				for (int i = 0; i < 5; i++)
				{
					stream->WriteUnsigned<uint8_t>(data->votes[i], 0, Blam::Network::MaxPlayers);
				}
			}

			if(data->Type == VotingMessageType::VoteTally || data->Type == VotingMessageType::VetoOption)
				stream->WriteUnsigned<uint8_t>(data->votesNeededToPass, 0, 16);

			if (data->Type == VotingMessageType::Winner) {
				stream->WriteUnsigned<uint8_t>(data->voteTime, 0, 120);
				stream->WriteUnsigned<uint8_t>(data->winner, 0, 5);
			}
				
		}

		bool Deserialize(Blam::BitStream *stream, VotingMessage *data) override
		{
			memset(data, 0, sizeof(*data));

			// Message type
			data->Type = static_cast<VotingMessageType>(stream->ReadUnsigned(0U, static_cast<uint32_t>(VotingMessageType::Count)));
			if (static_cast<uint32_t>(data->Type) >= static_cast<uint32_t>(VotingMessageType::Count))
				return false;

			if (data->Type == VotingMessageType::VotingOptions || data->Type == VotingMessageType::VetoOption)
			{
				for (int i = 0; i < 5; i++)
				{
					stream->ReadString(data->votingOptions[i].mapName);
					stream->ReadString(data->votingOptions[i].typeName);
					data->votingOptions[i].canVeto = stream->ReadBool();
					data->votingOptions[i].mapId = stream->ReadUnsigned<uint16_t>(0, 10000);
				}

				data->voteTime = stream->ReadUnsigned<uint8_t>(0, 120);
			}

			if (data->Type == VotingMessageType::Vote || data->Type == VotingMessageType::VetoVote)
				data->Vote = stream->ReadUnsigned<uint8_t>(0, 5);

			if (data->Type == VotingMessageType::VoteTally)
			{
				for (int i = 0; i < 5; i++)
				{
					data->votes[i] = stream->ReadUnsigned<uint8_t>(0, Blam::Network::MaxPlayers);
				}
			}


			if (data->Type == VotingMessageType::VoteTally || data->Type == VotingMessageType::VetoOption)
				data->votesNeededToPass = stream->ReadUnsigned<uint8_t>(0, 16);

			if (data->Type == VotingMessageType::Winner) {
				data->voteTime = stream->ReadUnsigned<uint8_t>(0, 120);
				data->winner = stream->ReadUnsigned<uint8_t>(0, 5);
			}

			return true;
		}

		void HandlePacket(Blam::Network::ObserverChannel *sender, const VotingMessagePacket *packet) override
		{
			auto session = Blam::Network::GetActiveSession();
			if (!session)
				return;

			//at this level, it doesnt matter if we are the host or not. If we receive any sort of voting message, handle it
			ReceivedVotingMessage(session, session->GetChannelPeer(sender), packet->Data);

		}
	};
	// Registered message handlers
	std::vector<std::shared_ptr<VotingMessageHandler>> votingMessageHandlers;

	// Callback for when a message is received.
	void ReceivedVotingMessage(Blam::Network::Session *session, int peer, const VotingMessage &message)
	{
		//Vote messages will not be passed on to the message handler. 
		if (message.Type == VotingMessageType::Vote && peer >= 0)
		{
			auto &membership = session->MembershipInfo;
			auto &player = membership.PlayerSessions[membership.GetPeerPlayer(peer)];
			std::string name = Utils::String::ThinString(player.Properties.DisplayName);

			Server::Voting::LogVote(message, name);
			return;
		}

		// Send the message out to handlers
		for (auto &&handler : votingMessageHandlers)
			handler->MessageReceived(message);
	}

	// Sends a voting message to a peer as a packet.
	bool SendVotingMessagePacket(Blam::Network::Session *session, int peer, const VotingMessage &message)
	{
		if (peer < 0)
			return false;
		// if we are trying to send something to ourself, pretend someone else sent it to us. 
		if (peer == session->MembershipInfo.LocalPeerIndex)
		{
			ReceivedVotingMessage(session, peer, message);
			return true;
		}
		auto packet = VotingPacketSender->New();
		packet.Data = message;
		VotingPacketSender->Send(peer, packet);
		return true;
	}


}


namespace Server::Voting
{
	VotingMessage::VotingMessage(VotingMessageType type)
	{
		memset(this, 0, sizeof(*this));
		Type = type;
	}
	void AddMessageHandler(std::shared_ptr<VotingMessageHandler> handler)
	{
		votingMessageHandlers.push_back(handler);
	}
	void InitializePackets()
	{
		// Register custom packet type
		auto handler = std::make_shared<VotingMessagePacketHandler>();
		VotingPacketSender = Patches::CustomPackets::RegisterPacket<VotingMessage>("eldewrito-voting-message", handler);
	}

	//Sends a voting message to all peers
	bool BroadcastVotingMessage(VotingMessage &message)
	{
		auto session = Blam::Network::GetActiveSession();
		auto membership = &session->MembershipInfo;
		for (int peer = membership->FindFirstPeer(); peer >= 0; peer = membership->FindNextPeer(peer))
		{
			if (!SendVotingMessagePacket(session, peer, message))
				return false;
		}
		return true;
	}

	//Sends a voting message to a specific peer only
	bool SendVotingMessageToPeer(VotingMessage &message, int peer)
	{
		auto session = Blam::Network::GetActiveSession();
		return SendVotingMessagePacket(session, peer, message);
	}

	//To be called anytime you vote for something. Vote gets sent to the host. 
	bool SendVoteToHost(const int vote)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session)
			return false;

		VotingMessage message(VotingMessageType::Vote);
		message.Vote = vote;

		return SendVotingMessagePacket(session, session->MembershipInfo.HostPeerIndex, message);
	}
}
