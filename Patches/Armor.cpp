#include "Armor.h"
#include "PlayerPropertiesExtension.h"

#include <iostream>

namespace
{
	struct ColorData
	{
		// All colors are RGB
		uint32_t primaryColor;
		uint32_t secondaryColor;
		uint32_t visorColor;
		uint32_t lightsColor;
		uint32_t holoColor;
	};

	struct ArmorData
	{
		// These are indexes into the armor tag blocks in mulg
		uint8_t helmet;
		uint8_t chest;
		uint8_t shoulders;
		uint8_t arms;
		uint8_t legs;
		uint8_t acc;
		uint8_t pelvis;
		uint8_t unused;
	};

	struct CustomizationData
	{
		ColorData colors;
		ArmorData armor;
	};

	const uint8_t MaxHelmet = 81;
	const uint8_t MaxChest = 82;
	const uint8_t MaxShoulders = 82;
	const uint8_t MaxArms = 50;
	const uint8_t MaxLegs = 52;
	const uint8_t MaxAcc = 24;
	const uint8_t MaxPelvis = 4;

	class ArmorExtension : public Patches::Network::PlayerPropertiesExtension<CustomizationData>
	{
	protected:
		void BuildData(int playerIndex, CustomizationData *out)
		{
			memset(out, 0, sizeof(CustomizationData));

			// just for testing purposes
			out->colors.primaryColor = 0xFFFFFF;
			out->colors.secondaryColor = 0x800000;
			out->colors.lightsColor = 0x8080FF;
			out->colors.visorColor = 0x0000FF;
			out->colors.holoColor = 0x000000;
			out->armor.helmet = 80;
			out->armor.chest = 80;
			out->armor.shoulders = 80;
			out->armor.arms = 43;
			out->armor.legs = 39;
			out->armor.acc = 0;
			out->armor.pelvis = 0;
		}

		void ApplyData(int playerIndex, void *session, const CustomizationData &data)
		{
			std::cout << "Applying customization data to player " << playerIndex << ":" << std::endl;
			std::cout << "helmet    = " << static_cast<int>(data.armor.helmet) << std::endl;
			std::cout << "chest     = " << static_cast<int>(data.armor.chest) << std::endl;
			std::cout << "shoulders = " << static_cast<int>(data.armor.shoulders) << std::endl;
			std::cout << "arms      = " << static_cast<int>(data.armor.arms) << std::endl;
			std::cout << "legs      = " << static_cast<int>(data.armor.legs) << std::endl;
			std::cout << "acc       = " << static_cast<int>(data.armor.acc) << std::endl;
			std::cout << "pelvis    = " << static_cast<int>(data.armor.pelvis) << std::endl;

			CustomizationData *armorSessionData = reinterpret_cast<CustomizationData*>(reinterpret_cast<uint8_t*>(session) + 0x6E8);
			*armorSessionData = data;
		}

		void Serialize(Blam::BitStream *stream, const CustomizationData &data)
		{
			// Colors
			stream->WriteUnsigned<uint32_t>(data.colors.primaryColor, 24);
			stream->WriteUnsigned<uint32_t>(data.colors.secondaryColor, 24);
			stream->WriteUnsigned<uint32_t>(data.colors.visorColor, 24);
			stream->WriteUnsigned<uint32_t>(data.colors.lightsColor, 24);
			stream->WriteUnsigned<uint32_t>(data.colors.holoColor, 24);

			// Armor
			stream->WriteUnsigned<uint8_t>(data.armor.helmet, 0, MaxHelmet);
			stream->WriteUnsigned<uint8_t>(data.armor.chest, 0, MaxChest);
			stream->WriteUnsigned<uint8_t>(data.armor.shoulders, 0, MaxShoulders);
			stream->WriteUnsigned<uint8_t>(data.armor.arms, 0, MaxArms);
			stream->WriteUnsigned<uint8_t>(data.armor.legs, 0, MaxLegs);
			stream->WriteUnsigned<uint8_t>(data.armor.acc, 0, MaxAcc);
			stream->WriteUnsigned<uint8_t>(data.armor.pelvis, 0, MaxPelvis);
		}

		void Deserialize(Blam::BitStream *stream, CustomizationData *out)
		{
			memset(out, 0, sizeof(CustomizationData));

			// Colors
			out->colors.primaryColor = stream->ReadUnsigned<uint32_t>(24);
			out->colors.secondaryColor = stream->ReadUnsigned<uint32_t>(24);
			out->colors.visorColor = stream->ReadUnsigned<uint32_t>(24);
			out->colors.lightsColor = stream->ReadUnsigned<uint32_t>(24);
			out->colors.holoColor = stream->ReadUnsigned<uint32_t>(24);

			// Armor
			out->armor.helmet = stream->ReadUnsigned<uint8_t>(0, MaxHelmet);
			out->armor.chest = stream->ReadUnsigned<uint8_t>(0, MaxChest);
			out->armor.shoulders = stream->ReadUnsigned<uint8_t>(0, MaxShoulders);
			out->armor.arms = stream->ReadUnsigned<uint8_t>(0, MaxArms);
			out->armor.legs = stream->ReadUnsigned<uint8_t>(0, MaxLegs);
			out->armor.acc = stream->ReadUnsigned<uint8_t>(0, MaxAcc);
			out->armor.pelvis = stream->ReadUnsigned<uint8_t>(0, MaxPelvis);
		}
	};
}

namespace Patches
{
	namespace Armor
	{
		void ApplyAll()
		{
			Network::PlayerPropertiesExtender::Instance().Add(std::make_shared<ArmorExtension>());
		}
	}
}