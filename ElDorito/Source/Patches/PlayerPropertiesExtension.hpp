#pragma once

#include "../Utils/Singleton.hpp"
#include "../Blam/BitStream.hpp"
#include "../Blam/BlamPlayers.hpp"

#include <vector>
#include <memory>

namespace Patches::Network
{
	// Base class for a class which adds data to player properties.
	class PlayerPropertiesExtensionBase
	{
	public:
		virtual ~PlayerPropertiesExtensionBase() { }

		// Builds extension data for a player.
		virtual void BuildData(int playerIndex, void *out) = 0;

		// Gets the size of the extension data.
		virtual size_t GetDataSize() const = 0;

		// Applies extension data to a player.
		virtual void ApplyData(int playerIndex, Blam::Players::PlayerProperties *properties, const void *data) = 0;

		// Serializes the extension data to be sent across the network.
		virtual void Serialize(Blam::BitStream *stream, const void *data) = 0;

		// Deserializes extension data that was received from the network.
		virtual void Deserialize(Blam::BitStream *stream, void *out) = 0;
	};

	// Helper class which adds type safety to PlayerPropertiesExtensionBase.
	template <class TData>
	class PlayerPropertiesExtension : public PlayerPropertiesExtensionBase
	{
	protected:
		// Builds extension data for a player.
		virtual void BuildData(int playerIndex, TData *out) = 0;

		// Applies extension data to a player.
		virtual void ApplyData(int playerIndex, Blam::Players::PlayerProperties *properties, const TData &data) = 0;

		// Serializes the extension data to be sent across the network.
		virtual void Serialize(Blam::BitStream *stream, const TData &data) = 0;

		// Deserializes extension data that was received from the network.
		virtual void Deserialize(Blam::BitStream *stream, TData *out) = 0;

	public:
		void BuildData(int playerIndex, void *out) override
		{
			BuildData(playerIndex, static_cast<TData*>(out));
		}

		size_t GetDataSize() const override
		{
			return sizeof(TData);
		}

		void ApplyData(int playerIndex, Blam::Players::PlayerProperties *properties, const void *data) override
		{
			ApplyData(playerIndex, properties, *static_cast<const TData*>(data));
		}

		void Serialize(Blam::BitStream *stream, const void *data) override
		{
			Serialize(stream, *static_cast<const TData*>(data));
		}

		void Deserialize(Blam::BitStream *stream, void *out) override
		{
			Deserialize(stream, static_cast<TData*>(out));
		}
	};

	// Singleton object which lets the player-properties packet be extended with custom data
	// TODO: Make this more generic and not so specific to player-properties
	class PlayerPropertiesExtender : public Utils::Singleton<PlayerPropertiesExtender>
	{
	public:
		// Adds an extension to the player-properties packet.
		void Add(std::shared_ptr<PlayerPropertiesExtensionBase> extension)
		{
			extensions.push_back(extension);
		}

		// Gets the total size of the player-properties extension data.
		size_t GetTotalSize();

		// Writes all extension data out to a player-properties structure.
		void BuildData(int playerIndex, void *out);

		// Applies all extension data in a player-properties structure.
		void ApplyData(int playerIndex, Blam::Players::PlayerProperties *properties, const void *data);

		// Serializes all extension data in a player-properties structure.
		void SerializeData(Blam::BitStream *stream, const void *data);

		// Deserializes all extension data in a player-properties structure.
		void DeserializeData(Blam::BitStream *stream, void *out);

	private:
		std::vector<std::shared_ptr<PlayerPropertiesExtensionBase>> extensions;
	};
}