#include "CustomPackets.hpp"
#include "../Pointer.hpp"
#include "../Patch.hpp"

namespace
{
	const int FirstCustomPacketId = 0x27;

	struct CustomPacket
	{
		std::string Name;
		std::shared_ptr<Patches::CustomPackets::RawPacketHandler> Handler;
	};
	std::vector<CustomPacket> customPackets;

	void *GetNetworkSession();

	void InitializePacketsHook();
	void DeserializePacketHook();
	void __fastcall SerializePacketHook(void *thisPtr, int unused, Blam::BitStream *stream, int id, int packetSize, void *packet);
	void HandlePacketHook();
}

namespace Patches
{
	namespace CustomPackets
	{
		void ApplyAll()
		{
			Hook(0x9E226, InitializePacketsHook, HookFlags::IsCall).Apply();
			Hook(0x7FFAF, DeserializePacketHook).Apply();

			Hook(0x7D618, SerializePacketHook, HookFlags::IsCall).Apply();
			Hook(0x7D81D, SerializePacketHook, HookFlags::IsCall).Apply();
			Hook(0x841F5, SerializePacketHook, HookFlags::IsCall).Apply();

			Hook(0x9CAFA, HandlePacketHook).Apply();
		}

		void SendPacket(int targetPeer, int id, const void *packet, int packetSize)
		{
			auto session = Blam::Network::GetActiveSession();
			if (!session)
				return;
			auto channelIndex = session->MembershipInfo.PeerChannels[targetPeer].ChannelIndex;
			if (channelIndex == -1)
				return;
			session->Observer->ObserverChannelSendMessage(0, channelIndex, false, id, packetSize, packet);
		}

		int RegisterRawPacket(const std::string &name, std::shared_ptr<RawPacketHandler> handler)
		{
			auto id = FirstCustomPacketId + customPackets.size();
			CustomPacket packet;
			packet.Name = name;
			packet.Handler = handler;
			customPackets.push_back(packet);
			return id;
		}
	}
}

namespace
{
	void InitializePacketsHook()
	{
		// Replace the packet buffer with one we control
		auto packetHandlerPtr = Pointer::Base(0x1E4A498);
		auto packetCount = FirstCustomPacketId + customPackets.size();
		auto packetBufferSize = packetCount * 0x24;
		auto customPacketBuffer = new uint8_t[packetBufferSize];
		packetHandlerPtr.Write<uint8_t*>(customPacketBuffer);
		memset(customPacketBuffer, 0, packetBufferSize);

		// Register custom packets
		typedef void(__thiscall *RegisterCustomPacketPtr)(uint8_t *thisPtr, int id, const char *name, int unk8, int minSize, int maxSize, void *serializeFunc, void *deserializeFunc, int unk1C, int unk20);
		auto RegisterCustomPacket = reinterpret_cast<RegisterCustomPacketPtr>(0x4801B0);
		for (size_t i = 0; i < customPackets.size(); i++)
		{
			auto id = FirstCustomPacketId + i;
			auto name = customPackets[i].Name.c_str();
			auto handler = customPackets[i].Handler;
			auto minSize = handler->GetMinRawPacketSize();
			auto maxSize = handler->GetMaxRawPacketSize();

			// NOTE: The serialize and deserialize functions are null here because the hooks take care of
			// finding the handler for custom packets
			RegisterCustomPacket(customPacketBuffer, id, name, 0, minSize, maxSize, nullptr, nullptr, 0, 0);
		}

		// HACK: Patch the packet verification function and change the max packet ID it accepts
		Patch(0x80022, { static_cast<uint8_t>(packetCount) }).Apply();
	}

	bool DeserializeCustomPacket(int packetId, Blam::BitStream *stream, int packetSize, void *packet)
	{
		auto packetIndex = packetId - FirstCustomPacketId;
		if (packetIndex < 0 || packetIndex >= static_cast<int>(customPackets.size()))
			return false;
		return customPackets[packetIndex].Handler->DeserializeRawPacket(stream, packetSize, packet);
	}

	__declspec(naked) void DeserializePacketHook()
	{
		__asm
		{
			// Set up function arguments
			push esi
			push dword ptr [edi]
			push dword ptr [ebp + 8]

			// Get the packet ID and check if it's custom
			mov eax, dword ptr [ebp + 0xC]
			mov eax, dword ptr [eax]
			cmp eax, FirstCustomPacketId
			jge custom

			// Packet is built-in
			// Execute replaced code and return
			mov eax, dword ptr [ebx + 0x18]
			push 0x47FFB8
			ret

		custom:
			// Push the packet id and call our custom deserializer
			push eax
			call DeserializeCustomPacket
			mov dword ptr [esp], 0x47FFBA
			ret
		}
	}

	void __fastcall SerializePacketHook(void *thisPtr, int unused, Blam::BitStream *stream, int id, int packetSize, void *packet)
	{
		// If the packet is custom, invoke its handler to serialize it
		auto packetIndex = id - FirstCustomPacketId;
		if (packetIndex >= 0 && packetIndex < static_cast<int>(customPackets.size()))
		{
			typedef void(__thiscall *SerializePacketInfoPtr)(void *thisPtr, Blam::BitStream *stream, int id, int packetSize);
			auto SerializePacketInfo = reinterpret_cast<SerializePacketInfoPtr>(0x4800D0);
			SerializePacketInfo(thisPtr, stream, id, packetSize);

			customPackets[packetIndex].Handler->SerializeRawPacket(stream, packetSize, packet);
			return;
		}

		// Serialize built-in packet
		typedef void(__thiscall *SerializePacketPtr)(void *thisPtr, Blam::BitStream *stream, int id, int packetSize, void *packet);
		auto SerializePacket = reinterpret_cast<SerializePacketPtr>(0x480090);
		SerializePacket(thisPtr, stream, id, packetSize, packet);
	}

	bool HandleCustomPacket(int id, Blam::Network::ObserverChannel *sender, const void *packet)
	{
		auto packetIndex = id - FirstCustomPacketId;
		if (packetIndex < 0 || packetIndex >= static_cast<int>(customPackets.size()))
			return false;
		customPackets[packetIndex].Handler->HandleRawPacket(sender, packet);
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