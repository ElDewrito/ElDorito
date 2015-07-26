#pragma once

#include <cstdint>
#include "BlamTypes.hpp"

namespace Blam
{
	namespace Network
	{
		// The maximum number of peers in a network session.
		const int MaxPeers = 17;

		// The maximum number of players in a network session.
		const int MaxPlayers = 16;

		struct PeerInfo
		{
			uint8_t Unknown0[0xF0];
			uint32_t PlayerMasks[2];
		};
		static_assert(sizeof(PeerInfo) == 0xF8, "Invalid PeerInfo size");

		struct PeerChannel
		{
			int ChannelIndex; // Can be -1 for none
			int Unknown4;
			int UnknownC;
		};
		static_assert(sizeof(PeerChannel) == 0xC, "Invalid PeerChannel size");

		struct PlayerSession
		{
			uint8_t Unknown0[0x50];
			uint64_t Uid;
			wchar_t DisplayName[16];
			int TeamIndex;
			uint8_t Unknown78[0x15CC];
		};
		static_assert(sizeof(PlayerSession) == 0x1648, "Invalid PlayerSession size");

		// c_network_session_membership
		struct SessionMembership
		{
			int Unknown0;
			int Unknown4;
			int Unknown8;
			int UnknownC;
			int HostPeerIndex;
			int Unknown14;
			int Unknown18;
			int Unknown1C;
			int Unknown20;
			int Unknown24;
			PeerInfo Peers[MaxPeers];
			uint8_t Unknown10A0[0x8];
			PlayerSession PlayerSessions[MaxPlayers];
			uint8_t Unknown17528[0x18C7F8];
			int LocalPeerIndex;
			int Unknown1A3D24;
			int Unknown1A3D28;
			PeerChannel PeerChannels[MaxPeers];
			uint8_t Unknown1A3DF8[0x128];

			// Finds the first available connected peer, or -1 if none.
			int FindFirstPeer() const;

			// Finds the next available connected peer, or -1 if none.
			int FindNextPeer(int lastPeer) const;

			// Gets the player index corresponding to a peer, or -1 if none.
			int GetPeerPlayer(int peer) const;

			// Gets a peer's team index, or -1 on failure.
			// Note that -1 does NOT mean that teams are disabled.
			int GetPeerTeam(int peer) const;
		};
		static_assert(sizeof(SessionMembership) == 0x1A3F20, "Invalid c_network_session_membership size");

		// c_network_session_parameter
		struct SessionParameter
		{
			// Returns whether the parameter has data available.
			bool IsAvailable() const;
		};

		// c_network_session_parameter_game_variant
		struct GameVariantSessionParameter: SessionParameter
		{
			uint8_t Unknown0[0x3570];

			// Gets the current variant data, or null if not available.
			PBLAM_GAME_VARIANT Get() const;
		};
		static_assert(sizeof(GameVariantSessionParameter) == 0x3570, "Invalid c_network_session_parameter_game_variant size");

		// c_network_session_parameters
		struct SessionParameters
		{
			uint8_t Unknown0[0x2D2D8];
			GameVariantSessionParameter GameVariant;
			uint8_t Unknown30848[0x870DC]; // approx size

			// Sets the session mode parameter.
			// TODO: Map out this enum
			bool SetSessionMode(int mode);
		};
		static_assert(sizeof(SessionParameters) == 0xB7924, "Invalid c_network_session_parameters size");

		struct ObserverChannel
		{
			uint8_t Unknown0[0x10D8];
		};
		static_assert(sizeof(ObserverChannel) == 0x10D8, "Invalid ObserverChannel size");

		// Header for a packet buffer.
		struct PacketHeader
		{
			// Initializes a packet header based off of information from the
			// current session.
			PacketHeader();

			uint8_t XnkAddr[0x10];
		};

		// c_network_observer
		struct Observer
		{
			uint8_t Unknown0[0x23F20]; // approx size

			// Sends a message across a channel.
			void ObserverChannelSendMessage(int ownerIndex, int channelIndex, bool secure, int id, int packetSize, const void *packet);
		};
		static_assert(sizeof(Observer) == 0x23F20, "Invalid c_network_observer size");

		// c_network_message_gateway
		struct MessageGateway
		{
			uint8_t Unknown0[0x688];
		};
		static_assert(sizeof(MessageGateway) == 0x688, "Invalid c_network_message_gateway size");

		// c_network_session
		struct Session
		{
			uint32_t Unknown0;
			MessageGateway *Gateway;
			Observer *Observer;
			uint32_t UnknownC;
			uint32_t Unknown10;
			uint32_t Unknown14;
			uint32_t Unknown18;
			uint32_t Unknown1C;
			SessionMembership MembershipInfo;
			SessionParameters Parameters;
			int Unknown25B864;
			int Unknown25B868;
			int Unknown25B86C;
			int Type; // 6 = host, 7 = local game
			uint8_t Unknown25B874[0x37C];
			int AddressIndex; // Idk what this actually is, but it's used to look up xnkaddr
			uint8_t Unknown25BBF4[0x4C];

			// Gets the index of the peer associated with a channel, or -1 on
			// failure.
			int GetChannelPeer(const ObserverChannel *channel) const;

			// Gets whether a session is established.
			bool IsEstablished() const { return Type > 3; }

			// Gets whether the local peer is hosting the session.
			bool IsHost() const { return Type == 6 || Type == 7; }

			// Gets whether teams are enabled.
			bool HasTeams() const;
		};
		static_assert(sizeof(Session) == 0x25BC40, "Invalid c_network_session size");

		// Gets a pointer to the active network session.
		// Can be null!
		Session *GetActiveSession();
	}
}