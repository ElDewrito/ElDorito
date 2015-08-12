#pragma once

#include <string>
#include <bitset>
#include <memory>
#include "../Blam/BlamNetwork.hpp"

namespace Server
{
	namespace Chat
	{
		// Chat message types.
		enum class ChatMessageType: uint32_t
		{
			// The message should be sent to all players.
			Global,

			// The message should only be sent to players on the same team as
			// the sender.
			Team,

			// The message should be sent to a specific player.
			Whisper,

			// The message was sent automatically by the server to a particular
			// player or set of players.
			Server,

			// Not actually a message type, just used to indicate the number of
			// valid message types.
			Count
		};

		// Chat message data.
		struct ChatMessage
		{
			ChatMessage() { }

			// Initializes a chat message from a type and a body.
			ChatMessage(ChatMessageType type, const std::string &body);

			// The message type.
			ChatMessageType Type;

			// The display name of the sender.
			// Not available for messages sent from clients or for server messages.
			wchar_t Sender[16];

			// For Whisper messages, the UID of the player to send the message to.
			// Not available for other message types.
			uint64_t Target;

			// The message body.
			char Body[512];
		};

		// Interface for a class which processes and handles chat messages.
		class ChatHandler
		{
		public:
			virtual ~ChatHandler() { }

			// Called after a player sends a message before the message is
			// sent to other players. The message can be modified, and if
			// ignore is set to true, it will not be sent to any clients.
			virtual void MessageSent(int senderPeer, ChatMessage *message, bool *ignore) = 0;

			// Called after a message has been received.
			virtual void MessageReceived(const ChatMessage &message) = 0;
		};

		// A std::bitset of peers.
		typedef std::bitset<Blam::Network::MaxPeers> PeerBitSet;

		// Initializes the server chat system.
		void Initialize();

		// Sends a message to every peer. Returns true if successful.
		bool SendGlobalMessage(const std::string &body);

		// Sends a message to every player on the local player's team. Returns
		// true if successful.
		bool SendTeamMessage(const std::string &body);

		// Sends a server message to specific peers. Only works if you are
		// host. Returns true if successful.
		bool SendServerMessage(const std::string &body, PeerBitSet peers);

		// Registers a chat handler object.
		void AddHandler(std::shared_ptr<ChatHandler> handler);
	}
}