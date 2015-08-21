#pragma once

#include <cstdint>
#include "BlamTypes.hpp"
#include "BitStream.hpp"

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
			int Unknown0;
			int ChannelIndex; // Can be -1 for none
			int Unknown8;
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
			PeerChannel PeerChannels[MaxPeers];
			uint8_t Unknown1A3DF8[0x12C];

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

		// Represents a network address.
		struct NetworkAddress
		{
			union
			{
				uint32_t IPv4;
				uint8_t Data[16];
			} Address;
			uint16_t Port;
			uint16_t AddressSize; // 4 for IPv4, 16 for IPv6

			static NetworkAddress FromInAddr(unsigned long address, uint16_t port)
			{
				NetworkAddress result;
				result.Address.IPv4 = _byteswap_ulong(address);
				result.Port = port;
				result.AddressSize = 4;
				return result;
			}

			unsigned long ToInAddr() const
			{
				return _byteswap_ulong(Address.IPv4);
			}
		};
		static_assert(sizeof(NetworkAddress) == 0x14, "Invalid NetworkAddress size");

		struct ObserverChannel
		{
			uint8_t Unknown0[0xAD4];
			NetworkAddress Address;
			uint8_t UnknownAE8[0x5F0];
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

		// Packet serialization function pointer types.
		typedef void(*SerializePacketFn)(BitStream *stream, int packetSize, const void *packet);
		typedef bool(*DeserializePacketFn)(BitStream *stream, int packetSize, void *packet);

		// Contains information about a registered packet type.
		struct RegisteredPacket
		{
			bool Initialized;
			const char *Name;
			int Unknown8;
			int MinSize;
			int MaxSize;
			SerializePacketFn Serialize;
			DeserializePacketFn Deserialize;
			int Unknown1C;
			int Unknown20;
		};
		static_assert(sizeof(RegisteredPacket) == 0x24, "Invalid RegisteredPacket size");

		// A table of registered packets.
		struct PacketTable
		{
			// There's more than 1 registered packet, but the array size can
			// vary if custom packets are installed.
			RegisteredPacket Packets[1];

			// Registers a packet at an index.
			void Register(int index, const char *name, int unk8, int minSize, int maxSize, SerializePacketFn serializeFunc, DeserializePacketFn deserializeFunc, int unk1C, int unk20);
		};

		// "ping" packets
		struct PingPacket
		{
			uint16_t ID;
			uint32_t Timestamp; // timeGetTime()
			bool Unknown8;
		};
		static_assert(sizeof(PingPacket) == 0xC, "Invalid PingPacket size");

		// "pong" packets
		struct PongPacket
		{
			uint16_t ID;        // Copied from ping packet
			uint32_t Timestamp; // Copied from ping packet
			int Unknown8;       // Always 2
		};
		static_assert(sizeof(PongPacket) == 0xC, "Invalid PongPacket size");

		enum PacketID
		{
			ePacketIDPing,
			ePacketIDPong,
			// TODO: Finish
		};

		// c_network_observer
		struct Observer
		{
			uint8_t Unknown0[0x23F20]; // approx size

			// Sends a message across a channel.
			void ObserverChannelSendMessage(int ownerIndex, int channelIndex, bool outOfBand, int id, int packetSize, const void *packet);

			// Sends a ping across a channel.
			// See MessageGateway::Ping().
			void Ping(int ownerIndex, int channelIndex, uint16_t id);
		};
		static_assert(sizeof(Observer) == 0x23F20, "Invalid c_network_observer size");

		// c_network_message_gateway
		struct MessageGateway
		{
			uint8_t Unknown0[0x688];

			// Sends a message directly to a network address.
			bool SendDirectedMessage(const NetworkAddress &address, int id, int packetSize, const void *packet);

			// Sends a ping to a network address. ID can be anything.
			// Once a pong is received back, callbacks registered with
			// Patches::Network::OnPong() will be invoked.
			bool Ping(NetworkAddress &address, uint16_t id);
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

		// Lifecycle states.
		enum LifeCycleState
		{
			eLifeCycleStateNone,
			eLifeCycleStatePreGame,
			eLifeCycleStateStartGame,
			eLifeCycleStateInGame,
			eLifeCycleStateEndGameWriteStats,
			eLifeCycleStateLeaving,
			eLifeCycleStateJoining,
			eLifeCycleStateMatchmakingStart,
			eLifeCycleStateMatchmakingFindMatchClient,
			eLifeCycleStateMatchmakingFindMatch,
			eLifeCycleStateMatchmakingFindAndAssembleMatch,
			eLifeCycleStateMatchmakingAssembleMatch,
			eLifeCycleStateMatchmakingArbitration,
			eLifeCycleStateMatchmakingSelectHost,
			eLifeCycleStateMatchmakingPrepareMap,
			eLifeCycleStateInMatch,
			eLifeCycleStateEndMatchWriteStats,
			eLifeCycleStatePostMatch,
		};

		// Gets a pointer to the active network session.
		// Can be null!
		Session *GetActiveSession();

		// Gets a pointer to the active packet table.
		// Can be null!
		PacketTable *GetPacketTable();

		// Sets the active packet table.
		// Only use this if you know what you're doing!
		void SetPacketTable(const PacketTable *newTable);
	}
}