#include "BlamNetwork.hpp"
#include "../Pointer.hpp"

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

		void Observer::ObserverChannelSendMessage(int ownerIndex, int channelIndex, bool secure, int id, int packetSize, const void *packet)
		{
			typedef int(__thiscall *ObserverChannelSendMessagePtr)(Observer *thisPtr, int ownerIndex, int channelIndex, bool secure, int id, int packetSize, const void *packet);
			auto ObserverChannelSendMessage = reinterpret_cast<ObserverChannelSendMessagePtr>(0x4474F0);
			ObserverChannelSendMessage(this, ownerIndex, channelIndex, secure, id, packetSize, packet);
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

		bool SessionParameters::SetSessionMode(int mode)
		{
			typedef bool(__thiscall *SetSessionModePtr)(SessionParameters *thisPtr, int mode);
			auto SetSessionMode = reinterpret_cast<SetSessionModePtr>(0x459A40);
			return SetSessionMode(this, mode);
		}
	}
}