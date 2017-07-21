#include "PlayerPropertiesExtension.hpp"

namespace Patches:: Network
{
	size_t PlayerPropertiesExtender::GetTotalSize()
	{
		size_t result = 0;
		for (auto it = extensions.begin(); it != extensions.end(); ++it)
			result += (*it)->GetDataSize();
		return result;
	}

	void PlayerPropertiesExtender::BuildData(int playerIndex, void *out)
	{
		// Write all of the data structures in order
		uint8_t *ptr = static_cast<uint8_t*>(out);
		for (auto it = extensions.begin(); it != extensions.end(); ++it)
		{
			(*it)->BuildData(playerIndex, ptr);
			ptr += (*it)->GetDataSize();
		}
	}

	void PlayerPropertiesExtender::ApplyData(int playerIndex, Blam::Players::PlayerProperties *properties, const void *data)
	{
		// Apply all of the data structures in order
		const uint8_t *ptr = static_cast<const uint8_t*>(data);
		for (auto it = extensions.begin(); it != extensions.end(); ++it)
		{
			(*it)->ApplyData(playerIndex, properties, ptr);
			ptr += (*it)->GetDataSize();
		}
	}

	void PlayerPropertiesExtender::SerializeData(Blam::BitStream *stream, const void *data)
	{
		// Serialize all of the structures in order
		const uint8_t *ptr = static_cast<const uint8_t*>(data);
		for (auto it = extensions.begin(); it != extensions.end(); ++it)
		{
			(*it)->Serialize(stream, ptr);
			ptr += (*it)->GetDataSize();
		}
	}

	void PlayerPropertiesExtender::DeserializeData(Blam::BitStream *stream, void *out)
	{
		// Deserialize all of the structures in order
		uint8_t *ptr = static_cast<uint8_t*>(out);
		for (auto it = extensions.begin(); it != extensions.end(); ++it)
		{
			(*it)->Deserialize(stream, ptr);
			ptr += (*it)->GetDataSize();
		}
	}
}