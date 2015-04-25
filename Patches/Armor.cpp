#include "Armor.h"
#include "PlayerPropertiesExtension.h"
#include "../ElPreferences.h"

#include <iostream>
#include <unordered_map>

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

	std::unordered_map<std::string, uint8_t> helmetIndexes = {
		{ "base", 0 },
		{ "stealth", 2 },
		{ "air_assault", 3 },
		{ "renegade", 12 },
		{ "nihard", 13 },
		{ "gladiator", 16 },
		{ "mac", 17 },
		{ "shark", 18 },
		{ "juggernaut", 20 },
		{ "dutch", 23 },
		{ "chameleon", 27 },
		{ "halberd", 29 },
		{ "cyclops", 30 },
		{ "scanner", 34 },
		{ "mercenary", 36 },
		{ "hoplite", 41 },
		{ "ballista", 47 },
		{ "strider", 54 },
		{ "demo", 56 },
		{ "orbital", 57 },
		{ "spectrum", 58 },
		{ "gungnir", 63 },
		{ "hammerhead", 67 },
		{ "omni", 68 },
		{ "oracle", 69 },
		{ "silverback", 79 },
		{ "widow_maker", 80 },
	};

	std::unordered_map<std::string, uint8_t> chestIndexes = {
		{ "base", 0 },
		{ "stealth", 2 },
		{ "air_assault", 3 },
		{ "renegade", 12 },
		{ "nihard", 13 },
		{ "gladiator", 16 },
		{ "mac", 17 },
		{ "shark", 18 },
		{ "juggernaut", 20 },
		{ "dutch", 23 },
		{ "chameleon", 27 },
		{ "halberd", 29 },
		{ "cyclops", 30 },
		{ "scanner", 34 },
		{ "mercenary", 36 },
		{ "hoplite", 41 },
		{ "ballista", 47 },
		{ "strider", 54 },
		{ "demo", 56 },
		{ "spectrum", 57 },
		{ "gungnir", 62 },
		{ "orbital", 63 },
		{ "hammerhead", 67 },
		{ "omni", 68 },
		{ "oracle", 69 },
		{ "silverback", 79 },
		{ "widow_maker", 80 },
		{ "tankmode_human", 82 },
	};

	std::unordered_map<std::string, uint8_t> shouldersIndexes = {
		{ "base", 0 },
		{ "stealth", 2 },
		{ "air_assault", 3 },
		{ "renegade", 12 },
		{ "nihard", 13 },
		{ "gladiator", 16 },
		{ "mac", 17 },
		{ "shark", 18 },
		{ "juggernaut", 20 },
		{ "dutch", 23 },
		{ "chameleon", 27 },
		{ "halberd", 29 },
		{ "cyclops", 30 },
		{ "scanner", 34 },
		{ "mercenary", 36 },
		{ "hoplite", 41 },
		{ "ballista", 47 },
		{ "strider", 54 },
		{ "demo", 56 },
		{ "spectrum", 57 },
		{ "gungnir", 61 },
		{ "orbital", 62 },
		{ "hammerhead", 67 },
		{ "omni", 68 },
		{ "oracle", 69 },
		{ "silverback", 79 },
		{ "widow_maker", 80 },
		{ "tankmode_human", 82 },
	};

	std::unordered_map<std::string, uint8_t> armsIndexes = {
		{ "base", 0 },
		{ "stealth", 1 },
		{ "renegade", 6 },
		{ "nihard", 7 },
		{ "gladiator", 10 },
		{ "mac", 11 },
		{ "shark", 12 },
		{ "juggernaut", 14 },
		{ "dutch", 17 },
		{ "chameleon", 21 },
		{ "scanner", 25 },
		{ "mercenary", 26 },
		{ "hoplite", 29 },
		{ "ballista", 30 },
		{ "strider", 33 },
		{ "demo", 34 },
		{ "spectrum", 35 },
		{ "gungnir", 38 },
		{ "orbital", 39 },
		{ "oracle", 41 },
		{ "widow_maker", 43 },
		{ "tankmode_human", 44 },
		{ "air_assault", 45 },
		{ "hammerhead", 46 },
		{ "omni", 47 },
		{ "silverback", 48 },
		{ "cyclops", 49 },
		{ "halberd", 50 },
	};

	std::unordered_map<std::string, uint8_t> legsIndexes = {
		{ "base", 0 },
		{ "stealth", 1 },
		{ "renegade", 5 },
		{ "nihard", 6 },
		{ "gladiator", 9 },
		{ "mac", 10 },
		{ "shark", 11 },
		{ "juggernaut", 13 },
		{ "dutch", 16 },
		{ "chameleon", 20 },
		{ "scanner", 24 },
		{ "mercenary", 25 },
		{ "hoplite", 29 },
		{ "ballista", 30 },
		{ "strider", 33 },
		{ "spectrum", 34 },
		{ "oracle", 37 },
		{ "widow_maker", 39 },
		{ "tankmode_human", 40 },
		{ "gungnir", 41 },
		{ "orbital", 42 },
		{ "demo", 43 },
		{ "air_assault", 44 },
		{ "hammerhead", 45 },
		{ "omni", 46 },
		{ "silverback", 47 },
		{ "cyclops", 48 },
		{ "halberd", 49 },
		{ "hammerhead", 50 },
		{ "omni", 51 },
		{ "silverback", 52 },
	};

	std::unordered_map<std::string, uint8_t> accIndexes = {
	};

	std::unordered_map<std::string, uint8_t> pelvisIndexes = {
		{ "base", 0 },
		{ "tankmode_human", 4 },
	};

	uint8_t getArmorIndex(const std::string &name, const std::unordered_map<std::string, uint8_t> &indexes)
	{
		auto it = indexes.find(name);
		return (it != indexes.end()) ? it->second : 0;
	}

	class ArmorExtension : public Patches::Network::PlayerPropertiesExtension<CustomizationData>
	{
	protected:
		void BuildData(int playerIndex, CustomizationData *out)
		{
			memset(out, 0, sizeof(CustomizationData));

			// Load armor settings from preferences
			ElPreferences &prefs = ElPreferences::Instance();
			out->colors.primaryColor = prefs.getPrimaryColor();
			out->colors.secondaryColor = prefs.getSecondaryColor();
			out->colors.lightsColor = prefs.getLightsColor();
			out->colors.visorColor = prefs.getVisorColor();
			out->colors.holoColor = prefs.getHoloColor();
			out->armor.helmet = getArmorIndex(prefs.getHelmetName(), helmetIndexes);
			out->armor.chest = getArmorIndex(prefs.getChestName(), chestIndexes);
			out->armor.shoulders = getArmorIndex(prefs.getShouldersName(), shouldersIndexes);
			out->armor.arms = getArmorIndex(prefs.getArmsName(), armsIndexes);
			out->armor.legs = getArmorIndex(prefs.getLegsName(), legsIndexes);
			out->armor.acc = getArmorIndex(prefs.getAccesoryName(), accIndexes);
			out->armor.pelvis = getArmorIndex(prefs.getPelvisName(), pelvisIndexes);
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