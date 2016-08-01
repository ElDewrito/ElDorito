#include "TagSync.hpp"

#include <cstdint>
#include <algorithm>

#include "../Patches/Core.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Cache/StringIdCache.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"
#include "../Patches/CustomPackets.hpp"
#include "../Utils/Cryptography.hpp"

#include "../Console.hpp"
#include "../ElDorito.hpp"

namespace
{
	using namespace Server::TagSync;
	using namespace Patches::CustomPackets;

	typedef VariadicPacket<TagSyncPacketData, TagSyncPacketVar> TagSyncUpdatePacket;
	typedef VariadicPacketSender<TagSyncPacketData, TagSyncPacketVar> TagSyncUpdateSender;
	std::shared_ptr<TagSyncUpdateSender> updateSender;

	class TagSyncUpdateHandler : public VariadicPacketHandler<TagSyncPacketData, TagSyncPacketVar>
	{
	public:
		TagSyncUpdateHandler() : VariadicPacketHandler(1){ }
		void Serialize(Blam::BitStream* stream, const TagSyncPacketData* data, int extraDataCount, const TagSyncPacketVar* extraData) override;
		bool Deserialize(Blam::BitStream* stream, TagSyncPacketData* data, int extraDataCount, TagSyncPacketVar* extraData) override;
		void HandlePacket(Blam::Network::ObserverChannel* sender, const VariadicPacket<TagSyncPacketData, TagSyncPacketVar>* packet) override;
	};

	void HostTick(Blam::Network::Session *session);
}

namespace Server
{
	namespace TagSync
	{
		std::vector<Tag> indicies;

		std::vector<std::string> blacklist; //list of indicies that causes crash when accessed (just flamethrower that I know of, vehicle & turrets might also crash)
		int indexIt = 0;

		void ApplyAfterTagsLoaded(const char *mapPath)
		{
			if (indicies.size())
				return;
			auto *matg = Blam::Tags::TagInstance(0x0016).GetDefinition<Blam::Tags::Game::Globals>();
			auto *mulg = Blam::Tags::TagInstance(matg->MultiplayerGlobals.TagIndex).GetDefinition<Blam::Tags::Game::MultiplayerGlobals>();

			for (auto &element : mulg->Universal[0].GameVariantWeapons)
			{
				auto name = std::string(Blam::Cache::StringIDCache::Instance.GetString(element.Name));
				auto index = (uint16_t)element.Weapon.TagIndex;

				if (index != 0xFFFF && std::find(blacklist.begin(), blacklist.end(), name) == blacklist.end())
				{
					indicies.push_back({ index, eSyncAutoAim });
					indicies.push_back({ index, eSyncRangeShort });
					indicies.push_back({ index, eSyncRangeLong });
				}
			}

			/*for (auto &element : mulg->Universal[0].GameVariantVehicles) { }*/ //TODO: Loop through vehicles and sync auto aim for vehicle weapons and turrets.
		}

		void Initialize()
		{
			auto updateHandler = std::make_shared<TagSyncUpdateHandler>();
			updateSender = RegisterVariadicPacket<TagSyncPacketData, TagSyncPacketVar>("eldewrito-tag-sync", updateHandler);
			Patches::Core::OnMapLoaded(ApplyAfterTagsLoaded);
			blacklist.push_back("flamethrower");//causes hang when accessed
		}

		void Tick()
		{
			auto session = Blam::Network::GetActiveSession();
			if (session && session->IsHost())
				HostTick(session);
		}
	}
}

namespace
{
	const int TAGSPERPACKET = 15;

	void BuildTagUpdate(uint16_t index, TagSyncPacketVar *result, SyncType syncType)
	{
		result->TagIndex = index;
		result->Type = syncType;

		auto *tag = Blam::Tags::TagInstance(index).GetDefinition<Blam::Tags::Items::Weapon>();

		switch (result->Type)
		{
		case eSyncAutoAim:
			result->Value.Float = tag->AutoaimAngle.Value;
			break;
		case eSyncRangeLong:
			result->Value.Float = tag->AutoaimRangeLong;
			break;
		case eSyncRangeShort:
			result->Value.Float = tag->AutoaimRangeShort;
			break;
		default:
			throw std::runtime_error("Invalid tag sync type");
		}
		return;
	}

	std::shared_ptr<TagSyncUpdatePacket> BuildTagUpdatePacket()
	{
		auto result = updateSender->New(TAGSPERPACKET);
		int indexStart = indexIt;
		for (int i = 0; i < TAGSPERPACKET; i++)
		{
			auto update = &result->ExtraData[i];
			BuildTagUpdate(indicies[indexIt].index, update, indicies[indexIt].type);
			indexIt++;
			if (indexIt == indicies.size() - 1)
				indexIt = 0;
		}
		indexIt = indexStart;
		return result;
	}

	bool IsPeerReady(Blam::Network::Session *session, int peerIndex)
	{
		auto membership = &session->MembershipInfo;
		if (peerIndex == membership->LocalPeerIndex || !membership->Peers[peerIndex].IsEstablished())
			return false;
		auto channelInfo = &membership->PeerChannels[peerIndex];
		return !channelInfo->Unavailable && channelInfo->ChannelIndex >= 0;
	}

	void UpdatePeersTags(Blam::Network::Session *session)
	{
		auto membership = &session->MembershipInfo;
		if (!indicies.size())
			return;
		auto packet = BuildTagUpdatePacket();

		for (auto peer = membership->FindFirstPeer(); peer != -1; peer = membership->FindNextPeer(peer)) //everyone in case someone is cheating...
		{
			if (peer != membership->LocalPeerIndex && IsPeerReady(session, peer))
			{
				updateSender->Send(peer, packet);
			}
		}

		for (int i = 0; i < TAGSPERPACKET; i++)
		{
			indexIt++;
			if (indexIt == indicies.size() - 1)
				indexIt = 0;
		}
	}

	static int tagTick = 0;
	void HostTick(Blam::Network::Session *session)
	{
		if (tagTick > 100)
		{
			tagTick = 0;
			auto objectHeadersPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset);
			auto objectHeaders = objectHeadersPtr.Read<const Blam::DataArray<Blam::Objects::ObjectHeader>*>();
			if (!objectHeaders)
				return;  //make sure host is in-game and not in a loading screen
			UpdatePeersTags(session);
		}
		else
			tagTick++;
	}

	void TagSyncUpdateHandler::Serialize(Blam::BitStream* stream, const TagSyncPacketData* data, int extraDataCount, const TagSyncPacketVar* extraData)
	{
		for (auto i = 0; i < extraDataCount; i++)
		{
			auto var = &extraData[i];
			stream->WriteUnsigned<uint32_t>(var->Type, 0, eSyncCount - 1);
			stream->WriteUnsigned<uint16_t>(var->TagIndex, 16);
			switch (var->Type)
			{
			case eSyncAutoAim:
			case eSyncRangeLong:
			case eSyncRangeShort:
				stream->WriteBlock(32, reinterpret_cast<const uint8_t*>(&var->Value.Float));
				break;
			default:
				throw std::runtime_error("Unsupported variable type");
			}
		}
	}

	bool TagSyncUpdateHandler::Deserialize(Blam::BitStream* stream, TagSyncPacketData* data, int extraDataCount, TagSyncPacketVar* extraData)
	{
		for (auto i = 0; i < extraDataCount; i++)
		{
			auto var = &extraData[i];
			var->Type = static_cast<SyncType>(stream->ReadUnsigned<uint32_t>(0, eSyncCount - 1));
			var->TagIndex = stream->ReadUnsigned<uint16_t>(16);
			switch (var->Type)
			{
			case eSyncAutoAim:
			case eSyncRangeLong:
			case eSyncRangeShort:
				stream->ReadBlock(32, reinterpret_cast<uint8_t*>(&var->Value.Float));
				break;
			default:
				return false;
			}
		}
		return true;
	}

	void TagSyncUpdateHandler::HandlePacket(Blam::Network::ObserverChannel* sender, const VariadicPacket<TagSyncPacketData, TagSyncPacketVar>* packet)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || session->IsHost())
			return; // Ignore packets sent by clients

		auto objectHeadersPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset);
		auto objectHeaders = objectHeadersPtr.Read<const Blam::DataArray<Blam::Objects::ObjectHeader>*>();
		if (!objectHeaders)
			return;  //no map is loaded, prolly not tags either.

		for (auto i = 0; i < packet->GetExtraDataCount(); i++)
		{
			auto value = &packet->ExtraData[i];

			if (std::find(indicies.begin(), indicies.end(), (Tag { value->TagIndex, value->Type })) == indicies.end())  //Since we are writing to memory, let the client decide if the index is one that should be modified.
				return;

			auto *tag = Blam::Tags::TagInstance(value->TagIndex).GetDefinition<Blam::Tags::Items::Weapon>();

			switch (value->Type)
			{
			case eSyncAutoAim:
				tag->AutoaimAngle = Blam::Math::Angle(value->Value.Float);
				break;
			case eSyncRangeLong:
				tag->AutoaimRangeLong = value->Value.Float;
				break;
			case eSyncRangeShort:
				tag->AutoaimRangeShort = value->Value.Float;
				break;
			default:
				break;
			}
		}
	}
}