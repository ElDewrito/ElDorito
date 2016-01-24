#include "BlamNetwork.hpp"
#include "../Pointer.hpp"

namespace
{
	Blam::Network::PingPacket MakePingPacket(uint16_t id)
	{
		Blam::Network::PingPacket packet;
		packet.ID = id;
		packet.QosResponse = false;
		packet.Timestamp = timeGetTime();
		return packet;
	}
}

namespace Blam
{
	namespace Network
	{
		Session *GetActiveSession()
		{
			auto networkSessionPtr = Pointer::Base(0x15AB848);
			return networkSessionPtr.Read<Session*>();
		}

		int SessionMembership::FindFirstPeer() const
		{
			typedef int(__thiscall *MembershipFindFirstPeerPtr)(const SessionMembership *thisPtr);
			auto MembershipFindFirstPeer = reinterpret_cast<MembershipFindFirstPeerPtr>(0x44E690);
			return MembershipFindFirstPeer(this);
		}

		int SessionMembership::FindNextPeer(int lastPeer) const
		{
			typedef int(__thiscall *MembershipFindNextPeerPtr)(const SessionMembership *thisPtr, int lastPeer);
			auto MembershipFindNextPeer = reinterpret_cast<MembershipFindNextPeerPtr>(0x44E710);
			return MembershipFindNextPeer(this, lastPeer);
		}

		int SessionMembership::GetPeerPlayer(int peer) const
		{
			typedef int(*MembershipGetPeerPlayerPtr)(const uint32_t *playerMasks, int max);
			auto MembershipGetPeerPlayer = reinterpret_cast<MembershipGetPeerPlayerPtr>(0x52E280);
			return MembershipGetPeerPlayer(Peers[peer].PlayerMasks, 16);
		}

		int SessionMembership::GetPlayerPeer(int player) const
		{
			if (player < 0 || player >= MaxPlayers)
				return -1;
			return PlayerSessions[player].PeerIndex;
		}

		int SessionMembership::GetPeerTeam(int peer) const
		{
			auto playerIndex = GetPeerPlayer(peer);
			if (playerIndex < 0)
				return -1;
			return PlayerSessions[playerIndex].TeamIndex;
		}

		void Observer::ObserverChannelSendMessage(int ownerIndex, int channelIndex, bool secure, int id, int packetSize, const void *packet)
		{
			typedef int(__thiscall *ObserverChannelSendMessagePtr)(Observer *thisPtr, int ownerIndex, int channelIndex, bool secure, int id, int packetSize, const void *packet);
			auto ObserverChannelSendMessage = reinterpret_cast<ObserverChannelSendMessagePtr>(0x4474F0);
			ObserverChannelSendMessage(this, ownerIndex, channelIndex, secure, id, packetSize, packet);
		}

		void Observer::Ping(int ownerIndex, int channelIndex, uint16_t id)
		{
			auto packet = MakePingPacket(id);
			ObserverChannelSendMessage(ownerIndex, channelIndex, true, ePacketIDPing, sizeof(packet), &packet);
		}

		PacketHeader::PacketHeader()
		{
			auto session = GetActiveSession();
			if (!session)
			{
				memset(this, 0, sizeof(*this));
				return;
			}
			
			typedef void(*InitPacketPtr)(int addressIndex, PacketHeader *packet);
			auto InitPacket = reinterpret_cast<InitPacketPtr>(0x482040);
			InitPacket(session->AddressIndex, this);
		}

		int Session::GetChannelPeer(const ObserverChannel *channel) const
		{
			// Look up the channel index
			typedef int(__thiscall *GetChannelIndexPtr)(const Network::Observer *thisPtr, uint32_t unk0, const ObserverChannel *channel);
			auto GetChannelIndex = reinterpret_cast<GetChannelIndexPtr>(0x447150);
			auto index = GetChannelIndex(Observer, Unknown10, channel);
			if (index < 0)
				return index;

			// Use the channel index to look up the peer
			typedef int(__thiscall *GetChannelPeerPtr)(const SessionMembership *thisPtr, int channelIndex);
			auto GetChannelPeer = reinterpret_cast<GetChannelPeerPtr>(0x44E860);
			return GetChannelPeer(&MembershipInfo, index);
		}

		bool Session::HasTeams() const
		{
			auto gameVariantParameter = &Parameters.GameVariant;
			if (!gameVariantParameter->IsAvailable())
				return false;
			auto gameVariant = gameVariantParameter->Get();
			if (!gameVariant)
				return false;
			return (gameVariant->TeamGame & 1) != 0;
		}

		NetworkAddress Session::GetPeerAddress(int peerIndex) const
		{
			if (peerIndex != MembershipInfo.LocalPeerIndex)
			{
				auto channelIndex = MembershipInfo.PeerChannels[peerIndex].ChannelIndex;
				if (channelIndex >= 0)
					return Observer->Channels[channelIndex].Address;
			}
			return NetworkAddress::FromInAddr(0x0100007F, 11774);
		}

		bool SessionParameters::SetSessionMode(int mode)
		{
			typedef bool(__thiscall *SetSessionModePtr)(SessionParameters *thisPtr, int mode);
			auto SetSessionMode = reinterpret_cast<SetSessionModePtr>(0x459A40);
			return SetSessionMode(this, mode);
		}

		bool SessionParameter::IsAvailable() const
		{
			typedef bool(__thiscall *IsAvailablePtr)(const SessionParameter *thisPtr);
			auto IsAvailable = reinterpret_cast<IsAvailablePtr>(0x450CF0);
			return IsAvailable(this);
		}

		PBLAM_GAME_VARIANT GameVariantSessionParameter::Get() const
		{
			typedef PBLAM_GAME_VARIANT(__thiscall *GetGameVariantPtr)(const GameVariantSessionParameter *thisPtr);
			auto GetGameVariant = reinterpret_cast<GetGameVariantPtr>(0x456140);
			return GetGameVariant(this);
		}

		PacketTable *GetPacketTable()
		{
			auto packetTablePtr = Pointer::Base(0x1E4A498);
			return packetTablePtr.Read<PacketTable*>();
		}

		void SetPacketTable(const PacketTable *newTable)
		{
			auto packetTablePtr = Pointer::Base(0x1E4A498);
			packetTablePtr.Write<const PacketTable*>(newTable);
		}

		void PacketTable::Register(int index, const char *name, int unk8, int minSize, int maxSize, SerializePacketFn serializeFunc, DeserializePacketFn deserializeFunc, int unk1C, int unk20)
		{
			typedef void(__thiscall *RegisterPacketPtr)(PacketTable *thisPtr, int id, const char *name, int unk8, int minSize, int maxSize, Blam::Network::SerializePacketFn serializeFunc, Blam::Network::DeserializePacketFn deserializeFunc, int unk1C, int unk20);
			auto RegisterPacket = reinterpret_cast<RegisterPacketPtr>(0x4801B0);
			RegisterPacket(this, index, name, unk8, minSize, maxSize, serializeFunc, deserializeFunc, unk1C, unk20);
		}

		bool MessageGateway::SendDirectedMessage(const NetworkAddress &address, int id, int packetSize, const void *packet)
		{
			typedef bool(__thiscall *SendDirectedMessagePtr)(MessageGateway *thisPtr, const NetworkAddress &address, int id, int packetSize, const void *packet);
			auto SendDirectedMessage = reinterpret_cast<SendDirectedMessagePtr>(0x4840C0);
			return SendDirectedMessage(this, address, id, packetSize, packet);
		}

		bool MessageGateway::Ping(NetworkAddress &address, uint16_t id)
		{
			auto packet = MakePingPacket(id);
			return SendDirectedMessage(address, ePacketIDPing, sizeof(packet), &packet);
		}

		bool BootPlayer(int playerIndex, int reason)
		{
			typedef bool(__cdecl *Network_squad_session_boot_playerPtr)(int playerIndex, int reason);
			auto Network_squad_session_boot_player = reinterpret_cast<Network_squad_session_boot_playerPtr>(0x437D60);
			return Network_squad_session_boot_player(playerIndex, reason);
		}

		bool EndGame()
		{
			typedef bool(__cdecl *Network_squad_session_end_gamePtr)();
			auto Network_squad_session_end_game = reinterpret_cast<Network_squad_session_end_gamePtr>(0x438780);
			return Network_squad_session_end_game();
		}

		void LeaveGame()
		{
			typedef void(__cdecl *Network_LeaveGamePtr)();
			auto Network_LeaveGame = reinterpret_cast<Network_LeaveGamePtr>(0xA81270);
			Network_LeaveGame();
		}
	}
}