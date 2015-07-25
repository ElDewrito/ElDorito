#define _CRT_SECURE_NO_WARNINGS

#include "ServerChat.hpp"
#include "../Patches/CustomPackets.hpp"

namespace
{
	using namespace Server::Chat;

	typedef Patches::CustomPackets::Packet<ChatMessage> ChatMessagePacket;
	typedef Patches::CustomPackets::PacketSender<ChatMessage> ChatMessagePacketSender;

	std::shared_ptr<ChatMessagePacketSender> PacketSender;

	bool HostReceivedMessage(Blam::Network::Session *session, int peer, const ChatMessage &message);
	void ClientReceivedMessage(const ChatMessage &message);

	// Packet handler for chat messages.
	class ChatMessagePacketHandler: public Patches::CustomPackets::PacketHandler<ChatMessage>
	{
	public:
		void Serialize(Blam::BitStream *stream, const ChatMessage *data) override
		{
			// Message type
			stream->WriteUnsigned(static_cast<uint32_t>(data->Type), 0U, static_cast<uint32_t>(ChatMessageType::Count));

			// Body
			stream->WriteString(data->Body);

			// For non-server messages, serialize the sender name
			if (data->Type != ChatMessageType::Server)
				stream->WriteString(data->Sender);

			// For whisper messages, serialize the target UID
			if (data->Type == ChatMessageType::Whisper)
				stream->WriteUnsigned(data->Target, 64);
		}

		bool Deserialize(Blam::BitStream *stream, ChatMessage *data) override
		{
			memset(data, 0, sizeof(*data));

			// Message type
			data->Type = static_cast<ChatMessageType>(stream->ReadUnsigned(0U, static_cast<uint32_t>(ChatMessageType::Count)));
			if (static_cast<uint32_t>(data->Type) >= static_cast<uint32_t>(ChatMessageType::Count))
				return false;

			// Body
			if (!stream->ReadString(data->Body))
				return false;

			// For non-server messages, deserialize the sender name
			if (data->Type != ChatMessageType::Server && !stream->ReadString(data->Sender))
				return false;

			// For whisper messages, deserialize the target UID
			if (data->Type == ChatMessageType::Whisper)
				data->Target = stream->ReadUnsigned<uint64_t>(64);
			return true;
		}

		void HandlePacket(Blam::Network::ObserverChannel *sender, const ChatMessagePacket *packet) override
		{
			auto session = Blam::Network::GetActiveSession();
			if (!session)
				return;
			auto peer = session->GetChannelPeer(sender);
			if (peer < 0)
				return;
			if (session->IsHost())
				HostReceivedMessage(session, peer, packet->Data);
			else if (peer == session->MembershipInfo.HostPeerIndex)
				ClientReceivedMessage(packet->Data);
		}
	};

	// Registered chat handlers
	std::vector<std::shared_ptr<ChatHandler>> chatHandlers;

	// Sends a message to a peer as a packet.
	bool SendMessagePacket(int peer, const ChatMessage &message)
	{
		if (peer < 0)
			return false;
		ChatMessagePacket packet;
		packet.Data = message;
		PacketSender->Send(peer, packet);
		return true;
	}

	// Broadcasts a message to a set of peers.
	bool BroadcastMessage(Blam::Network::Session *session, int senderPeer, ChatMessage *message, PeerBitSet peers)
	{
		if (senderPeer < 0)
			return false;

		// Run the message through each registered handler
		auto ignore = false;
		for (auto &&handler : chatHandlers)
			handler->MessageSent(senderPeer, message, &ignore);
		if (ignore)
			return true; // Message was rejected by a handler

		// Loop through each peer and send them a packet (or handle the message
		// immediately if it's being sent to the local peer)
		auto membership = &session->MembershipInfo;
		for (auto peer = membership->FindFirstPeer(); peer >= 0; peer = membership->FindNextPeer(peer))
		{
			if (!peers[peer])
				continue; // Not being sent to this peer
			if (peer != membership->LocalPeerIndex)
			{
				if (!SendMessagePacket(peer, *message))
					return false;
			}
			else
			{
				ClientReceivedMessage(*message);
			}
		}
		return true;
	}

	// Returns a bitset of peers to send a message to.
	PeerBitSet GetMessagePeers(int senderPeer, const ChatMessage &message)
	{
		// Just send the message to everyone for now
		return PeerBitSet().set();
	}

	// Fills in the sender name field of a message.
	bool FillInSenderName(Blam::Network::Session *session, int senderPeer, ChatMessage *message)
	{
		// Look up the player associated with the peer and copy the name in
		auto membership = &session->MembershipInfo;
		auto playerIndex = membership->GetPeerPlayer(senderPeer);
		if (playerIndex < 0)
			return false;
		memset(message->Sender, 0, sizeof(message->Sender));
		wcsncpy(message->Sender, membership->PlayerSessions[playerIndex].DisplayName, sizeof(message->Sender) / sizeof(message->Sender[0]) - 1);
		return true;
	}

	// Callback for when a message is received as the host.
	bool HostReceivedMessage(Blam::Network::Session *session, int peer, const ChatMessage &message)
	{
		// Verify that the message isn't empty and that the type is valid
		// TODO: Implement support for message types other than Global
		if (peer < 0 || !message.Body[0] || message.Type != ChatMessageType::Global)
			return false;

		// Don't trust the Sender field
		auto broadcastMessage = message;
		if (!FillInSenderName(session, peer, &broadcastMessage))
			return false;

		auto targetPeers = GetMessagePeers(peer, message);
		return BroadcastMessage(session, peer, &broadcastMessage, targetPeers);
	}

	// Callback for when a message is received as the client.
	void ClientReceivedMessage(const ChatMessage &message)
	{
		// Send the message out to handlers
		for (auto &&handler : chatHandlers)
			handler->MessageReceived(message);
	}

	// Sends a message as a client.
	bool SendClientMessage(Blam::Network::Session *session, const ChatMessage &message)
	{
		if (session->IsHost())
		{
			// We're the host, so pretend a client sent us the message
			return HostReceivedMessage(session, session->MembershipInfo.LocalPeerIndex, message);
		}

		// Send the message across the network to the host
		return SendMessagePacket(session->MembershipInfo.HostPeerIndex, message);
	}
}

namespace Server
{
	namespace Chat
	{
		void Initialize()
		{
			// Register custom packet type
			auto handler = std::make_shared<ChatMessagePacketHandler>();
			PacketSender = Patches::CustomPackets::RegisterPacket<ChatMessage>(handler);
		}

		bool SendGlobalMessage(const std::string &body)
		{
			auto session = Blam::Network::GetActiveSession();
			if (!session || !session->IsEstablished())
				return false;

			ChatMessage message(ChatMessageType::Global, body);
			return SendClientMessage(session, message);
		}

		bool SendServerMessage(const std::string &body, PeerBitSet peers)
		{
			auto session = Blam::Network::GetActiveSession();
			if (!session || !session->IsEstablished() || !session->IsHost())
				return false;

			ChatMessage message(ChatMessageType::Server, body);
			return BroadcastMessage(session, session->MembershipInfo.LocalPeerIndex, &message, peers);
		}

		void AddHandler(std::shared_ptr<ChatHandler> handler)
		{
			chatHandlers.push_back(handler);
		}

		ChatMessage::ChatMessage(ChatMessageType type, const std::string &body)
		{
			memset(this, 0, sizeof(*this));
			Type = type;
			strncpy(Body, body.c_str(), sizeof(Body) / sizeof(Body[0]) - 1);
		}
	}
}