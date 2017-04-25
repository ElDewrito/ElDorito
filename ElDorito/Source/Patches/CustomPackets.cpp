#include "CustomPackets.hpp"
#include "../Pointer.hpp"
#include "../Patch.hpp"
#include "../Utils/Cryptography.hpp"
#include <unordered_map>
#include <limits>

namespace
{
	using namespace Patches::CustomPackets;

	const int CustomPacketId = 0x27; // Last packet ID used by the game is 0x26

	struct CustomPacket
	{
		std::string Name;
		std::shared_ptr<RawPacketHandler> Handler;
	};
	std::unordered_map<PacketGuid, CustomPacket> customPackets;
	CustomPacket* LookUpPacketType(PacketGuid guid);

	void InitializePacketsHook();
	void HandlePacketHook();

	void SerializeCustomPacket(Blam::BitStream *stream, int packetSize, const void *packet);
	bool DeserializeCustomPacket(Blam::BitStream *stream, int packetSize, void *packet);
}

namespace Patches
{
	namespace CustomPackets
	{
		void ApplyAll()
		{
			Hook(0x9E226, InitializePacketsHook, HookFlags::IsCall).Apply();
			Hook(0x9CAFA, HandlePacketHook).Apply();
		}

		void SendPacket(int targetPeer, const void *packet, int packetSize)
		{
			auto session = Blam::Network::GetActiveSession();
			if (!session)
				return;
			auto channelIndex = session->MembershipInfo.PeerChannels[targetPeer].ChannelIndex;
			if (channelIndex == -1)
				return;
			session->Observer->ObserverChannelSendMessage(0, channelIndex, false, CustomPacketId, packetSize, packet);
		}

		PacketGuid RegisterPacketImpl(const std::string &name, std::shared_ptr<RawPacketHandler> handler)
		{
			PacketGuid guid;
			if (!Utils::Cryptography::Hash32(name, &guid))
				throw std::runtime_error("Failed to generate packet GUID");
			if (LookUpPacketType(guid))
				throw std::runtime_error("Duplicate packet GUID"); // TODO: Throwing an exception here might not be the best idea...
			CustomPacket packet;
			packet.Name = name;
			packet.Handler = handler;
			customPackets[guid] = packet;
			return guid;
		}
	}
}

namespace
{
	void InitializePacketsHook()
	{
		// Replace the packet table with one we control
		// Only one extra packet type is ever allocated
		auto packetCount = CustomPacketId + 1;
		auto customPacketTable = reinterpret_cast<Blam::Network::PacketTable*>(new Blam::Network::RegisteredPacket[packetCount]);
		Blam::Network::SetPacketTable(customPacketTable);
		memset(customPacketTable, 0, packetCount * sizeof(Blam::Network::RegisteredPacket));

		// Register the "master" custom packet
		auto name = "eldewrito-custom-packet";
		auto minSize = 0;
		auto maxSize = std::numeric_limits<int>::max();
		customPacketTable->Register(CustomPacketId, name, 0, minSize, maxSize, SerializeCustomPacket, DeserializeCustomPacket, 0, 0);

		// HACK: Patch the packet verification function and change the max packet ID it accepts
		Patch(0x80022, { static_cast<uint8_t>(CustomPacketId + 1) }).Apply();
	}

	CustomPacket* LookUpPacketType(PacketGuid guid)
	{
		auto it = customPackets.find(guid);
		if (it == customPackets.end())
			return nullptr;
		return &it->second;
	}

	void SerializeCustomPacket(Blam::BitStream *stream, int packetSize, const void *packet)
	{
		auto packetBase = static_cast<const PacketBase*>(packet);

		// Serialize the packet header
		stream->WriteBlock(sizeof(packetBase->Header) * 8, reinterpret_cast<const uint8_t*>(&packetBase->Header));

		// Serialize the type GUID
		stream->WriteUnsigned(packetBase->TypeGuid, sizeof(packetBase->TypeGuid) * 8);

		// Look up the handler to use and serialize the rest of the packet
		auto type = LookUpPacketType(packetBase->TypeGuid);
		if (!type)
			return;

		// Verify the packet size with the handler
		auto minSize = type->Handler->GetMinRawPacketSize();
		auto maxSize = type->Handler->GetMaxRawPacketSize();
		if (packetSize < minSize || packetSize > maxSize)
			return;

		// Serialize the rest of the packet
		type->Handler->SerializeRawPacket(stream, packetSize, packet);
	}

	bool DeserializeCustomPacket(Blam::BitStream *stream, int packetSize, void *packet)
	{
		if (packetSize < static_cast<int>(sizeof(PacketBase)))
			return false;
		auto packetBase = static_cast<PacketBase*>(packet);

		// Deserialize the packet header
		stream->ReadBlock(sizeof(packetBase->Header) * 8, reinterpret_cast<uint8_t*>(&packetBase->Header));

		// Deserialize the type GUID and look up the handler to use
		packetBase->TypeGuid = stream->ReadUnsigned<PacketGuid>(sizeof(PacketGuid) * 8);
		auto type = LookUpPacketType(packetBase->TypeGuid);
		if (!type)
			return false;

		// Verify the packet size with the handler
		auto minSize = type->Handler->GetMinRawPacketSize();
		auto maxSize = type->Handler->GetMaxRawPacketSize();
		if (packetSize < minSize || packetSize > maxSize)
			return false;

		// Deserialize the rest of the packet
		return type->Handler->DeserializeRawPacket(stream, packetSize, packet);
	}

	bool HandleCustomPacket(int id, Blam::Network::ObserverChannel *sender, const void *packet)
	{
		// Only handle the master custom packet
		if (id != CustomPacketId)
			return false;

		// Use the type GUID to look up the handler to use
		auto packetBase = static_cast<const PacketBase*>(packet);
		auto type = LookUpPacketType(packetBase->TypeGuid);
		if (!type)
			return false;
		type->Handler->HandleRawPacket(sender, packet);
		return true;
	}

	__declspec(naked) void HandlePacketHook()
	{
		__asm
		{
			// Check if the channel is closed
			mov edx, dword ptr [ebp + 8]
			cmp dword ptr [edx + 0xA10], 5
			jnz failed

			// Pass the packet to the custom handler
			push dword ptr [ebp + 0x14]
			push edx
			push edi
			call HandleCustomPacket
			test al, al
			jz failed

			// Return gracefully
			push 0x49CB14
			ret

		failed:
			// Execute replaced code
			mov ecx, [esi + 8]
			push 0
			push 0x49CAFF
			ret
		}
	}
}