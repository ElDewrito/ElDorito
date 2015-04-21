#include "Armor.h"
#include "PlayerPropertiesExtension.h"

#include <iostream>

namespace
{
	struct ArmorData
	{
		uint8_t helmet;
		uint8_t chest;
		uint8_t shoulders;
		uint8_t arms;
		uint8_t legs;
		uint8_t acc;
		uint8_t pelvis;
	};

	const uint8_t MaxHelmet = 81;
	const uint8_t MaxChest = 82;
	const uint8_t MaxShoulders = 82;
	const uint8_t MaxArms = 50;
	const uint8_t MaxLegs = 52;
	const uint8_t MaxAcc = 24;
	const uint8_t MaxPelvis = 4;

	class ArmorExtension : public Patches::Network::PlayerPropertiesExtension<ArmorData>
	{
	protected:
		void BuildData(int playerIndex, ArmorData *out)
		{
			// just for testing purposes
			out->helmet = 80;
			out->chest = 80;
			out->shoulders = 80;
			out->arms = 43;
			out->legs = 39;
			out->acc = 0;
			out->pelvis = 0;
		}

		void ApplyData(int playerIndex, void *session, const ArmorData &data)
		{
			std::cout << "Applying armor data to player " << playerIndex << ":" << std::endl;
			std::cout << "helmet    = " << static_cast<int>(data.helmet) << std::endl;
			std::cout << "chest     = " << static_cast<int>(data.chest) << std::endl;
			std::cout << "shoulders = " << static_cast<int>(data.shoulders) << std::endl;
			std::cout << "arms      = " << static_cast<int>(data.arms) << std::endl;
			std::cout << "legs      = " << static_cast<int>(data.legs) << std::endl;
			std::cout << "acc       = " << static_cast<int>(data.acc) << std::endl;
			std::cout << "pelvis    = " << static_cast<int>(data.pelvis) << std::endl;

			uint8_t *armorSessionData = reinterpret_cast<uint8_t*>(session) + 0x20 + 0x6DC;
			memcpy(armorSessionData, &data, 7);
		}

		void Serialize(Blam::BitStream *stream, const ArmorData &data)
		{
			stream->WriteUnsigned<uint8_t>(data.helmet, 0, MaxHelmet);
			stream->WriteUnsigned<uint8_t>(data.chest, 0, MaxChest);
			stream->WriteUnsigned<uint8_t>(data.shoulders, 0, MaxShoulders);
			stream->WriteUnsigned<uint8_t>(data.arms, 0, MaxArms);
			stream->WriteUnsigned<uint8_t>(data.legs, 0, MaxLegs);
			stream->WriteUnsigned<uint8_t>(data.acc, 0, MaxAcc);
			stream->WriteUnsigned<uint8_t>(data.pelvis, 0, MaxPelvis);
		}

		void Deserialize(Blam::BitStream *stream, ArmorData *out)
		{
			out->helmet = stream->ReadUnsigned<uint8_t>(0, MaxHelmet);
			out->chest = stream->ReadUnsigned<uint8_t>(0, MaxChest);
			out->shoulders = stream->ReadUnsigned<uint8_t>(0, MaxShoulders);
			out->arms = stream->ReadUnsigned<uint8_t>(0, MaxArms);
			out->legs = stream->ReadUnsigned<uint8_t>(0, MaxLegs);
			out->acc = stream->ReadUnsigned<uint8_t>(0, MaxAcc);
			out->pelvis = stream->ReadUnsigned<uint8_t>(0, MaxPelvis);
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