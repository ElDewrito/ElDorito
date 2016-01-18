#include "VariableSynchronization.hpp"

#include <cstdint>
#include <unordered_map>
#include <bitset>

#include "../Console/GameConsole.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Utils/Cryptography.hpp"
#include "../Patches/CustomPackets.hpp"

namespace
{
	using namespace Patches::CustomPackets;
	using namespace Modules;

	typedef uint32_t SyncID;
	const size_t MaxStringLength = 512;

	// Binds a server variable to a client variable.
	struct SynchronizationBinding
	{
		SyncID ID;
		Command *ServerVariable;
		Command *ClientVariable;

		// This is used to determine which peers have updated their binding.
		// If a peer's bit is set to 1, then it's up-to-date.
		std::bitset<Blam::Network::MaxPeers> SynchronizedPeers;
	};

	std::unordered_map<SyncID, SynchronizationBinding> syncBindings;

	// Packet structures
	struct SyncUpdatePacketData
	{
	};
	struct SyncUpdatePacketVar
	{
		SyncID ID;
		CommandType Type;
		union
		{
			uint64_t Int;
			float Float;
			char String[MaxStringLength + 1];
		} Value;
	};

	typedef VariadicPacket<SyncUpdatePacketData, SyncUpdatePacketVar> SyncUpdatePacket;
	typedef VariadicPacketSender<SyncUpdatePacketData, SyncUpdatePacketVar> SyncUpdatePacketSender;
	std::shared_ptr<SyncUpdatePacketSender> updateSender;

	class SyncUpdateHandler : public VariadicPacketHandler<SyncUpdatePacketData, SyncUpdatePacketVar>
	{
	public:
		SyncUpdateHandler() : VariadicPacketHandler(1) { }

		void Serialize(Blam::BitStream* stream, const SyncUpdatePacketData* data, int extraDataCount, const SyncUpdatePacketVar* extraData) override;
		bool Deserialize(Blam::BitStream* stream, SyncUpdatePacketData* data, int extraDataCount, SyncUpdatePacketVar* extraData) override;
		void HandlePacket(Blam::Network::ObserverChannel* sender, const VariadicPacket<SyncUpdatePacketData, SyncUpdatePacketVar>* packet) override;
	};

	SyncID GenerateID(const SynchronizationBinding &binding);
	void AddBinding(const SynchronizationBinding &binding);
	void HostTick(Blam::Network::Session *session);
}

namespace Server
{
	namespace VariableSynchronization
	{
		void Initialize()
		{
			auto updateHandler = std::make_shared<SyncUpdateHandler>();
			updateSender = RegisterVariadicPacket<SyncUpdatePacketData, SyncUpdatePacketVar>("eldewrito-sync-var", updateHandler);
		}

		void Synchronize(Command *serverVariable, Command *clientVariable)
		{
			if (serverVariable->Type != clientVariable->Type)
				throw std::runtime_error("Server and client variable types do not match");
			SynchronizationBinding binding;
			binding.ServerVariable = serverVariable;
			binding.ClientVariable = clientVariable;
			binding.ID = GenerateID(binding);
			binding.SynchronizedPeers.reset();
			AddBinding(binding);
		}

		void Tick()
		{
			// We only need to do anything if we're the host, otherwise the
			// packet handler will take care of client stuff
			auto session = Blam::Network::GetActiveSession();
			if (session && session->IsHost())
				HostTick(session);
		}
	}
}

namespace
{
	SyncID GenerateID(const SynchronizationBinding &binding)
	{
		// Hash a concatenation of the server var name and the client var name
		auto str = binding.ServerVariable->Name + binding.ClientVariable->Name;
		SyncID result;
		if (!Utils::Cryptography::Hash32(str, &result))
			throw std::runtime_error("Failed to generate sync ID");
		return result;
	}

	void AddBinding(const SynchronizationBinding &binding)
	{
		if (syncBindings.find(binding.ID) != syncBindings.end())
			throw std::runtime_error("Duplicate sync ID");
		syncBindings[binding.ID] = binding;
	}

	// TODO: Maybe move some of this variable stuff into CommandMap, since it's
	// not really specific to synchronization
	bool CompareVariables(const Command *lhs, const Command *rhs)
	{
		if (lhs->Type != rhs->Type)
			throw std::runtime_error("Cannot compare variables of different types");
		switch (lhs->Type)
		{
		case eCommandTypeVariableInt:
			return lhs->ValueInt == rhs->ValueInt;
		case eCommandTypeVariableInt64:
			return lhs->ValueInt64 == rhs->ValueInt64;
		case eCommandTypeVariableFloat:
			return lhs->ValueFloat == rhs->ValueFloat;
		case eCommandTypeVariableString:
			return lhs->ValueString == rhs->ValueString;
		default:
			throw std::runtime_error("Unsupported variable type");
		}
	}

	void SetVariable(Command *var, const Command *value)
	{
		if (var->Type != value->Type)
			throw std::runtime_error("Cannot set a variable of one type to a variable of another type");
		var->ValueString = value->ValueString;
		switch (var->Type)
		{
		case eCommandTypeVariableInt:
			var->ValueInt = value->ValueInt;
			break;
		case eCommandTypeVariableInt64:
			var->ValueInt64 = value->ValueInt64;
			break;
		case eCommandTypeVariableFloat:
			var->ValueFloat = value->ValueFloat;
			break;
		case eCommandTypeVariableString:
			// ValueString is always set
			break;
		default:
			throw std::runtime_error("Unsupported variable type");
		}
		if (var->UpdateEvent)
		{
			std::string returnInfo;
			var->UpdateEvent({ var->ValueString }, returnInfo);
		}
	}

	void SetVariable(Command *var, const SyncUpdatePacketVar *value)
	{
		// Note: unlike the other overload, this can't throw or else it could
		// be abused to crash someone's game. Validation is done during
		// deserialization.
		if (var->Type != value->Type)
			return;
		switch (var->Type)
		{
		case eCommandTypeVariableInt:
			var->ValueInt = static_cast<uint32_t>(value->Value.Int);
			var->ValueString = std::to_string(var->ValueInt);
			break;
		case eCommandTypeVariableInt64:
			var->ValueInt64 = value->Value.Int;
			var->ValueString = std::to_string(var->ValueInt64);
			break;
		case eCommandTypeVariableFloat:
			var->ValueFloat = value->Value.Float;
			var->ValueString = std::to_string(var->ValueFloat);
			break;
		case eCommandTypeVariableString:
			var->ValueString = value->Value.String;
			break;
		default:
			return;
		}
		if (var->UpdateEvent)
		{
			std::string returnInfo;
			var->UpdateEvent({ var->ValueString }, returnInfo);
		}
	}

	void TickBinding(SynchronizationBinding *binding)
	{
		// Only update if the server variable changed
		if (CompareVariables(binding->ServerVariable, binding->ClientVariable))
			return;

		// Synchronize the client variable locally
		SetVariable(binding->ClientVariable, binding->ServerVariable);

		// Clear the binding's peer synchronization status so that it will be
		// batched in with the next update
		binding->SynchronizedPeers.reset();
	}

	void TickBindings()
	{
		for (auto &&binding : syncBindings)
			TickBinding(&binding.second);
	}

	std::vector<SynchronizationBinding*> FindOutOfDateBindings(int peerIndex)
	{
		// Find bindings which don't have the peer in their set
		std::vector<SynchronizationBinding*> result;
		for (auto &&binding : syncBindings)
		{
			if (!binding.second.SynchronizedPeers[peerIndex])
				result.push_back(&binding.second);
		}
		return result;
	}

	void BuildVariableUpdate(const SynchronizationBinding *binding, SyncUpdatePacketVar *result)
	{
		auto var = binding->ServerVariable;
		result->ID = binding->ID;
		result->Type = var->Type;
		switch (var->Type)
		{
		case eCommandTypeVariableInt:
			result->Value.Int = var->ValueInt;
			break;
		case eCommandTypeVariableInt64:
			result->Value.Int = var->ValueInt64;
			break;
		case eCommandTypeVariableFloat:
			result->Value.Float = var->ValueFloat;
			break;
		case eCommandTypeVariableString:
			strncpy_s(result->Value.String, var->ValueString.c_str(), MaxStringLength);
			result->Value.String[MaxStringLength] = 0;
			break;
		default:
			throw std::runtime_error("Unsupported variable type");
		}
	}

	std::shared_ptr<SyncUpdatePacket> BuildUpdatePacket(const std::vector<SynchronizationBinding*> &bindings)
	{
		auto result = updateSender->New(bindings.size());
		for (size_t i = 0; i < bindings.size(); i++)
		{
			auto binding = bindings[i];
			auto update = &result->ExtraData[i];
			BuildVariableUpdate(binding, update);
		}
		return result;
	}

	void SynchronizePeer(int peerIndex)
	{
		// Get the bindings which need to be sent to the peer
		auto outOfDateBindings = FindOutOfDateBindings(peerIndex);
		if (!outOfDateBindings.size())
			return;

		// Build and send an update packet
		auto packet = BuildUpdatePacket(outOfDateBindings);
		updateSender->Send(peerIndex, packet);

		// Mark the peer as synchronized
		for (auto binding : outOfDateBindings)
			binding->SynchronizedPeers[peerIndex] = true;
	}

	bool IsPeerReady(Blam::Network::Session *session, int peerIndex)
	{
		auto membership = &session->MembershipInfo;
		if (peerIndex == membership->LocalPeerIndex || !membership->Peers[peerIndex].IsEstablished())
			return false;
		auto channelInfo = &membership->PeerChannels[peerIndex];
		return !channelInfo->Unavailable && channelInfo->ChannelIndex >= 0;
	}

	void SynchronizePeers(Blam::Network::Session *session)
	{
		std::bitset<Blam::Network::MaxPeers> visitedPeers;
		auto membership = &session->MembershipInfo;
		for (auto peer = membership->FindFirstPeer(); peer != -1; peer = membership->FindNextPeer(peer))
		{
			// Only synchronize remote peers which are completely established
			if (peer != membership->LocalPeerIndex && IsPeerReady(session, peer))
			{
				SynchronizePeer(peer);
				visitedPeers[peer] = true;
			}
		}

		// Clear the synchronization statuses for peers which weren't visited
		// (this takes care of disconnecting peers)
		for (auto &&binding : syncBindings)
			binding.second.SynchronizedPeers &= visitedPeers;
	}

	void HostTick(Blam::Network::Session *session)
	{
		// First, go through each binding and check for updates
		TickBindings();

		// Now make sure all peers are synchronized with us
		SynchronizePeers(session);
	}

	void SyncUpdateHandler::Serialize(Blam::BitStream* stream, const SyncUpdatePacketData* data, int extraDataCount, const SyncUpdatePacketVar* extraData)
	{
		for (auto i = 0; i < extraDataCount; i++)
		{
			// Send ID and type followed by value
			auto var = &extraData[i];
			stream->WriteUnsigned(var->ID, 32);
			stream->WriteUnsigned<uint32_t>(var->Type, 0, eCommandType_Count - 1);
			switch (var->Type)
			{
			case eCommandTypeVariableInt:
				stream->WriteUnsigned(var->Value.Int, 32);
				break;
			case eCommandTypeVariableInt64:
				stream->WriteUnsigned(var->Value.Int, 64);
				break;
			case eCommandTypeVariableFloat:
				stream->WriteBlock(32, reinterpret_cast<const uint8_t*>(&var->Value.Float));
				break;
			case eCommandTypeVariableString:
				stream->WriteString(var->Value.String);
				break;
			default:
				throw std::runtime_error("Unsupported variable type");
			}
		}
	}

	bool SyncUpdateHandler::Deserialize(Blam::BitStream* stream, SyncUpdatePacketData* data, int extraDataCount, SyncUpdatePacketVar* extraData)
	{
		for (auto i = 0; i < extraDataCount; i++)
		{
			// Read ID and type followed by value
			auto var = &extraData[i];
			var->ID = stream->ReadUnsigned<uint32_t>(32);
			var->Type = static_cast<CommandType>(stream->ReadUnsigned<uint32_t>(0, eCommandType_Count - 1));
			switch (var->Type)
			{
			case eCommandTypeVariableInt:
				var->Value.Int = stream->ReadUnsigned<uint64_t>(32);
				break;
			case eCommandTypeVariableInt64:
				var->Value.Int = stream->ReadUnsigned<uint64_t>(64);
				break;
			case eCommandTypeVariableFloat:
				stream->ReadBlock(32, reinterpret_cast<uint8_t*>(&var->Value.Float));
				break;
			case eCommandTypeVariableString:
				stream->ReadString(var->Value.String);
				break;
			default:
				return false;
			}
		}
		return true;
	}

	void SyncUpdateHandler::HandlePacket(Blam::Network::ObserverChannel* sender, const VariadicPacket<SyncUpdatePacketData, SyncUpdatePacketVar>* packet)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || session->IsHost())
			return; // Ignore packets sent by clients

		// Update each variable based on the binding ID
		for (auto i = 0; i < packet->GetExtraDataCount(); i++)
		{
			auto value = &packet->ExtraData[i];
			auto it = syncBindings.find(value->ID);
			if (it == syncBindings.end())
				continue;
			SetVariable(it->second.ClientVariable, value);
		}
	}
}