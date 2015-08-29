/* HOW TO SEND AND RECEIVE CUSTOM PACKETS:
 * 
 * First of all, there are two types of custom packets you can define:
 *
 * - Fixed-size packets consist of a single struct.
 * - "Variadic" (or variable-length) packets consist of a struct followed by an
 *   array of optional values.
 *
 * You should prefer to use fixed-size packets unless you actually need to send
 * variable-length data (such as chat messages).
 *
 * ********************************************
 * * SENDING AND RECEIVING FIXED-SIZE PACKETS *
 * ********************************************
 *
 * 1. Define a POD (plain old data) struct which will hold the packet data. Add
 *    fields to it as necessary.
 *
 * 2. You'll probably want to make a couple of typedefs now to make things
 *    easier (this assumes your struct is called ExamplePacketData):
 *
 *    typedef Patches::CustomPackets::Packet<ExamplePacketData> ExamplePacket;
 *    typedef Patches::CustomPackets::PacketSender<ExamplePacketData> ExamplePacketSender;
 *
 *    The first typedef creates an alias for the Packet<> object, which is the
 *    object you will actually be sending over the network (it contains a
 *    header followed by the packet data).
 *
 *    The second typedef creates an alias for the PacketSender<> object, which
 *    is what you will use to send packets.
 *
 * 3. Define a handler class which implements PacketHandler. Specialize
 *    PacketHandler with the type of the POD struct you just created.
 *
 *    a. In your Serialize() method, write out fields to the provided
 *       BitStream.
 *
 *    b. In your Deserialize() method, read in fields from the provided
 *       BitStream. If you return false from this, the packet will be
 *       discarded and the game may disconnect. Make sure to do error checking!
 *
 *    c. Finally, HandlePacket() will be called when a packet is received. To
 *       get the player or peer that sent the packet, see below.
 *
 * 4. On initialization, register your packet by calling RegisterPacket and
 *    passing in a unique packet name and a shared_ptr to an instance of the
 *    handler class you just defined. This will return a shared_ptr to a sender
 *    object which you will use to send packets.
 *
 *    It is recommended that you prefix the packet name with your plugin name,
 *    or "eldewrito" for packets built into ElDewrito. This helps ensure
 *    uniqueness.
 *
 *    auto handler = std::make_shared<ExamplePacketHandler>();
 *    auto myPacketSender = Patches::CustomPacket::RegisterPacket<ExamplePacketData>("eldewrito-example-packet", handler);
 *
 * 5. Finally, to send packets, use the New() method on your PacketSender
 *    object to create a packet with a properly-initialized header. Fill in its
 *    Data structure, and then pass it to your sender with a peer index to send
 *    to (see below on how to get this):
 *
 *    auto myPacket = myPacketSender->New();
 *    myPacket.Data.Foo = 42;
 *    myPacketSender->Send(peer, myPacket);
 *
 * ******************************************
 * * SENDING AND RECEIVING VARIADIC PACKETS *
 * ******************************************
 *
 * 1. Define a POD struct which will hold the base (fixed-size) packet data.
 *    Add fields to it as necessary.
 *
 * 2. If you will be using a non-primitive type for each optional data element,
 *    also define a POD struct for it.
 *
 * 3. Create typedefs as in step 2 for fixed-size packets, but use
 *    VariadicPacket and VariadicPacketSender instead.
 *
 * 4. Define a handler class which implements VariadicPacketHandler. This
 *    mostly works the same way as fixed-size packet handlers, except each
 *    serialization method also accepts a count and an array of optional
 *    elements. Additionally, you will need to provide a constructor which
 *    calls the base class constructor and sets the minimum and maximum number
 *    of optional elements which can be accepted. If you don't specify a
 *    maximum (by calling the constructor with only one argument), it will be
 *    set as large as possible.
 *
 *    IMPORTANT: It is your responsibility to ensure that you stay within these
 *    limits when you send packets. Otherwise, your packets will silently fail
 *    to serialize!
 *
 * 5. Register your packet like you would a fixed-size packet, but using
 *    RegisterVariadicPacket instead.
 *
 * 6. Finally, to send packets, you will need to allocate a VariadicPacket by
 *    using the New() method on your PacketSender. Pass in the number of extra
 *    elements you want:
 *
 *    auto examplePacket = myPacketSender->New(123);
 *
 *    Note that this will give you a shared_ptr to the packet instead of just
 *    returning a struct as with the fixed-size packets. This is done because
 *    variable-length structs cannot be stack-allocated by definition.
 *
 *    Extra elements are made available by the ExtraData[] array on the packet
 *    object. After you fill out the Data and ExtraData[] arrays, you can then
 *    send the packet using the sender object like you would a fixed-size
 *    packet.
 *
 * ***********************
 * * BLAM NETWORKING 101 *
 * ***********************
 *
 * === Players vs. Peers ===
 *
 * There is a very important distinction between "players" and "peers" in the
 * Blam engine:
 *
 * - A "player" takes up a slot on the roster and represents a client who can
 *   control a biped and participate in the game.
 *
 * - A "peer" represents a direct connection to another machine. A peer does
 *   NOT have to be associated with a player - for example, a dedicated server
 *   host will not be bound to a player. Likewise, a player also does NOT have
 *   to be associated with a peer - if you are not the host, there will not be
 *   peers associated with the other non-host players in the game because you
 *   will not have direct connections to them. There is always guaranteed to be
 *   a peer available which represents your local machine (but you can't send
 *   packets to it).
 *
 *   DO NOT MAKE THE MISTAKE OF ASSUMING THAT PEER INDEXES ARE EQUIVALENT TO
 *   PLAYER INDEXES. YOUR CODE MIGHT EVEN WORK IF YOU GET LUCKY, BUT IT IS
 *   COMPLETELY INCORRECT TO DO THIS.
 *
 * === Common Tasks ===
 *
 * Getting a pointer to the current network session (DO NOT HARDCODE AN
 * ADDRESS):
 * 
 *   auto session = Blam::Network::GetActiveSession();
 *   if (session)
 *   {
 *       // ...
 *   }
 *
 * Checking if you are the host:
 *
 *   if (session->IsHost())
 *   {
 *       // ...
 *   }
 *
 * Getting the host peer:
 *
 *   auto hostPeer = session->MembershipInfo.HostPeer;
 *
 * Looping through each connected peer (for example, to broadcast a packet):
 *
 *   auto membership = &session->MembershipInfo;
 *   for (auto peerIndex = membership->FindFirstPeer(); peerIndex >= 0; peerIndex = membership->FindNextPeer(peerIndex))
 *   {
 *       if (peerIndex == membership->LocalPeerIndex)
 *           continue;
 *       // ...
 *   }
 * 
 * Getting the index of a player associated with a peer:
 *
 *   auto playerIndex = membership->GetPeerPlayer(peerIndex);
 *   if (playerIndex >= 0)
 *   {
 *       // ...
 *   }
 *
 * Getting the index of the peer who sent a packet:
 *
 *   virtual void HandlePacket(Blam::Network::ObserverChannel *sender, const ExamplePacket *packet) override
 *   {
 *       auto session = Blam::Network::GetActiveSession();
 *       auto senderPeer = session->GetChannelPeer(sender);
 *       if (senderPeer < 0)
 *           return;
 *       // ...
 *   }
 */

#pragma once

#include "../Blam/BitStream.hpp"
#include "../Blam/BlamNetwork.hpp"

#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include <type_traits>
#include <limits>

namespace Patches
{
	namespace CustomPackets
	{
		void ApplyAll();

		// Type used for a custom packet GUID.
		typedef uint32_t PacketGuid;

		// Sends raw packet data.
		void SendPacket(int targetPeer, const void *packet, int packetSize);

		// Base class for raw packet data handlers.
		class RawPacketHandler
		{
		public:
			virtual ~RawPacketHandler() { }

			// Gets the minimum size of an acceptable packet in bytes.
			virtual int GetMinRawPacketSize() const = 0;

			// Gets the maximum size of an acceptable packet in bytes.
			virtual int GetMaxRawPacketSize() const = 0;

			// Serializes raw packet data to a bitstream.
			virtual void SerializeRawPacket(Blam::BitStream *stream, int packetSize, const void *packet) = 0;

			// Deserializes raw packet data from a bitstream.
			virtual bool DeserializeRawPacket(Blam::BitStream *stream, int packetSize, void *packet) = 0;

			// Called whenever raw packet data is received.
			virtual void HandleRawPacket(Blam::Network::ObserverChannel *sender, const void *packet) = 0;
		};

		// Base struct for custom packets.
		struct PacketBase
		{
			explicit PacketBase(PacketGuid guid)
				: TypeGuid(guid)
			{
			}

			// The packet's header (initialized automatically).
			Blam::Network::PacketHeader Header;

			// The GUID of the packet's type.
			PacketGuid TypeGuid;
		};

		// A packet which can be sent across the network.
		template<class TData>
		struct Packet: PacketBase
		{
			explicit Packet(PacketGuid guid)
				: PacketBase(guid)
			{
			}

			// The packet's data.
			TData Data;
		};

		// A packet which can be sent across the network with extra data that
		// varies in size.
		template<class TData, class TExtraData>
		class VariadicPacket: PacketBase
		{
			// Private - use Allocate() to create a VariadicPacket
			VariadicPacket(PacketGuid guid, int extraDataCount)
				: PacketBase(guid), extraDataCount(extraDataCount)
			{
			}

			VariadicPacket(const VariadicPacket&) = delete;
			VariadicPacket& operator=(const VariadicPacket&) = delete;

			typedef VariadicPacket<TData, TExtraData> TPacketType;

		public:
			// Calculates the number of extra data objects that can fit in a
			// packet of a given size, or -1 if the size is incorrect.
			static int CalculateExtraDataCount(int packetSize)
			{
				packetSize -= CalculateSize(0);
				if (packetSize < 0)
					return -1;
				if (packetSize % sizeof(TExtraData) != 0)
					return -1;
				return packetSize / sizeof(TExtraData);
			}

			// Calculates the size of a packet in bytes given a count of extra
			// data objects.
			static int CalculateSize(int extraDataCount)
			{
				return offsetof(TPacketType, ExtraData) + extraDataCount * sizeof(TExtraData);
			}

			// Gets the number of extra data objects in the packet.
			int GetExtraDataCount() const
			{
				return extraDataCount;
			}

			// Get the size of the packet in bytes.
			int GetSize() const
			{
				return CalculateSize(extraDataCount);
			}

			// Allocates a new variadic packet.
			static std::shared_ptr<TPacketType> Allocate(PacketGuid guid, int extraDataCount)
			{
				auto packetSize = CalculateSize(extraDataCount);
				auto buffer = new uint8_t[packetSize];
				new (buffer) VariadicPacket<TData, TExtraData>(guid, extraDataCount);
				return std::shared_ptr<TPacketType>(reinterpret_cast<TPacketType*>(buffer), [](TPacketType* x)
				{
					delete[] reinterpret_cast<uint8_t*>(x);
				});
			}

			// Initializes a variadic packet from a buffer.
			static TPacketType *FromBuffer(uint8_t *buffer, int extraDataCount)
			{
				auto result = reinterpret_cast<TPacketType*>(buffer);
				result->extraDataCount = extraDataCount;
				return result;
			}

			// The packet's data.
			TData Data;

		private:
			int extraDataCount;

		public:
			// Gets the extra data in the packet. Note that this array is not
			// actually 1 element in length - it depends on what was passed to
			// the Allocate() function.
			TExtraData ExtraData[1];
		};

		// Base class for packet data handlers
		template<class TData>
		class PacketHandler : public RawPacketHandler
		{
		public:
			virtual void Serialize(Blam::BitStream *stream, const TData *data) = 0;
			virtual bool Deserialize(Blam::BitStream *stream, TData *data) = 0;
			virtual void HandlePacket(Blam::Network::ObserverChannel *sender, const Packet<TData> *packet) = 0;

			int GetMinRawPacketSize() const override
			{
				return static_cast<int>(sizeof(Packet<TData>));
			}

			int GetMaxRawPacketSize() const override
			{
				return static_cast<int>(sizeof(Packet<TData>));
			}

			void SerializeRawPacket(Blam::BitStream *stream, int packetSize, const void *packet) override
			{
				// Verify the packet size and typecast to the type-safe packet data
				if (packetSize != sizeof(Packet<TData>))
					return;
				auto fullPacket = static_cast<const Packet<TData>*>(packet);
				Serialize(stream, &fullPacket->Data);
			}

			bool DeserializeRawPacket(Blam::BitStream *stream, int packetSize, void *packet) override
			{
				// Verify the packet size and typecast to the type-safe packet data
				if (packetSize != sizeof(Packet<TData>))
					return false;
				auto fullPacket = static_cast<Packet<TData>*>(packet);
				return Deserialize(stream, &fullPacket->Data);
			}

			void HandleRawPacket(Blam::Network::ObserverChannel *sender, const void *packet) override
			{
				HandlePacket(sender, static_cast<const Packet<TData>*>(packet));
			}
		};

		// Base class for a type-safe variadic packet handler.
		template<class TData, class TExtraData>
		class VariadicPacketHandler : public RawPacketHandler
		{
			typedef VariadicPacket<TData, TExtraData> TPacket;

		public:
			// Constructs a variadic packet handler with a minimum extra data
			// count, but with the maximum packet size set as large as possible.
			explicit VariadicPacketHandler(int extraDataMin)
			{
				minPacketSize = TPacket::CalculateSize(extraDataMin);
				maxPacketSize = std::numeric_limits<int>::max();
			}

			// Constructs a variadic packet handler with minimum and maximum
			// extra data counts.
			VariadicPacketHandler(int extraDataMin, int extraDataMax)
			{
				minPacketSize = TPacket::CalculateSize(extraDataMin);
				maxPacketSize = TPacket::CalculateSize(extraDataMax);
			}

			// Serializes packet data to a bitstream.
			virtual void Serialize(Blam::BitStream *stream, const TData *data, int extraDataCount, const TExtraData *extraData) = 0;

			// Deserializes packet data from a bitstream.
			virtual bool Deserialize(Blam::BitStream *stream, TData *data, int extraDataCount, TExtraData *extraData) = 0;

			// Called when a packet is received.
			virtual void HandlePacket(Blam::Network::ObserverChannel *sender, const VariadicPacket<TData, TExtraData> *packet) = 0;

			int GetMinRawPacketSize() const override
			{
				return minPacketSize;
			}

			int GetMaxRawPacketSize() const override
			{
				return maxPacketSize;
			}

			void SerializeRawPacket(Blam::BitStream *stream, int packetSize, const void *packet) override
			{
				// Verify the packet size and typecast to the type-safe packet data
				if (packetSize < minPacketSize || packetSize > maxPacketSize)
					return;
				auto fullPacket = static_cast<const VariadicPacket<TData, TExtraData>*>(packet);
				if (TPacket::CalculateExtraDataCount(packetSize) != fullPacket->GetExtraDataCount())
					return;

				// Serialize the extra data count
				stream->WriteUnsigned(static_cast<uint32_t>(fullPacket->GetExtraDataCount()), 32);

				// Pass the packet to the Serialize() function
				Serialize(stream, &fullPacket->Data, fullPacket->GetExtraDataCount(), fullPacket->ExtraData);
			}

			bool DeserializeRawPacket(Blam::BitStream *stream, int packetSize, void *packet) override
			{
				// Verify the packet size
				if (packetSize < minPacketSize || packetSize > maxPacketSize)
					return false;
				auto calculatedExtraDataCount = TPacket::CalculateExtraDataCount(packetSize);
				if (calculatedExtraDataCount < 0)
					return false;

				// Deserialize the extra data count
				auto extraDataCount = static_cast<int>(stream->ReadUnsigned<uint32_t>(32));
				if (extraDataCount != calculatedExtraDataCount)
					return false;

				// Initialize the packet inside the buffer and pass the rest to Deserialize()
				auto fullPacket = TPacket::FromBuffer(static_cast<uint8_t*>(packet), extraDataCount);
				return Deserialize(stream, &fullPacket->Data, extraDataCount, fullPacket->ExtraData);
			}

			void HandleRawPacket(Blam::Network::ObserverChannel *sender, const void *packet) override
			{
				HandlePacket(sender, static_cast<const TPacket*>(packet));
			}

		private:
			int minPacketSize;
			int maxPacketSize;
		};

		// Sends fixed-size packets across the network.
		template<class TData>
		class PacketSender
		{
			typedef Packet<TData> TPacket;

		public:
			explicit PacketSender(PacketGuid id)
			{
				this->id = id;
			}

			// Creates a new packet.
			TPacket New() const
			{
				return Packet<TData>(id);
			}

			// Sends packet data to a peer.
			void Send(int targetPeer, const TPacket &packet) const
			{
				SendPacket(targetPeer, &packet, sizeof(packet));
			}

		private:
			PacketGuid id;
		};

		// Sends variable-length packets across the network.
		template<class TData, class TExtraData>
		class VariadicPacketSender
		{
			typedef VariadicPacket<TData, TExtraData> TPacket;

		public:
			explicit VariadicPacketSender(PacketGuid id)
			{
				this->id = id;
			}

			// Creates a new packet.
			std::shared_ptr<TPacket> New(int extraDataCount) const
			{
				return TPacket::Allocate(id, extraDataCount);
			}

			// Sends packet data to a peer.
			void Send(int targetPeer, const TPacket &packet)
			{
				SendPacket(targetPeer, &packet, packet.GetSize());
			}

			// Sends packet data to a peer.
			void Send(int targetPeer, std::shared_ptr<const TPacket> packet)
			{
				Send(targetPeer, *packet);
			}

		private:
			PacketGuid id;
		};

		// Registers a packet handler under a particular name and returns the
		// GUID of the new packet type.
		PacketGuid RegisterPacketImpl(const std::string &name, std::shared_ptr<RawPacketHandler> handler);

		// Registers a packet handler under a particular name and returns an
		// object which can be used to easily send packets.
		template<class TPacket>
		std::shared_ptr<PacketSender<TPacket>> RegisterPacket(const std::string &name, std::shared_ptr<PacketHandler<TPacket>> handler)
		{
			auto id = RegisterPacketImpl(name, handler);
			return std::make_shared<PacketSender<TPacket>>(id);
		}

		// Registers a variadic packet handler under a particular name and
		// returns an object which can be used to easily send packets.
		template<class TPacket, class TExtraData>
		std::shared_ptr<VariadicPacketSender<TPacket, TExtraData>> RegisterVariadicPacket(const std::string &name, std::shared_ptr<VariadicPacketHandler<TPacket, TExtraData>> handler)
		{
			auto id = RegisterPacketImpl(name, handler);
			return std::make_shared<VariadicPacketSender<TPacket, TExtraData>>(id);
		}
	}
}