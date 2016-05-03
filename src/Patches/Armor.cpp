
#include <unordered_map>
#include <map>

#include "Armor.hpp"
#include "PlayerPropertiesExtension.hpp"
#include "../Patch.hpp"
#include "../Modules/ModulePlayer.hpp"

#include "../Blam/Cache/StringIdCache.hpp"
#include "../Blam/Tags/Tags.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Globals.hpp"
#include "../Blam/Tags/MultiplayerGlobals.hpp"

using namespace Blam::Players;

namespace
{
	// Used during bitstream operations to automatically calculate the size of each armor component
	const uint8_t MaxArmorIndices[] = { 81, 82, 82, 50, 52, 24, 4 };

	// TODO: These are defined at the bottom of the file...should probably move them elsewhere
	extern std::unordered_map<std::string, uint8_t> helmetIndexes;
	extern std::unordered_map<std::string, uint8_t> chestIndexes;
	extern std::unordered_map<std::string, uint8_t> shouldersIndexes;
	extern std::unordered_map<std::string, uint8_t> armsIndexes;
	extern std::unordered_map<std::string, uint8_t> legsIndexes;
	extern std::unordered_map<std::string, uint8_t> accIndexes;
	extern std::unordered_map<std::string, uint8_t> pelvisIndexes;
	extern std::map<std::string, uint16_t> weaponIndices;

	uint8_t GetArmorIndex(const std::string &name, const std::unordered_map<std::string, uint8_t> &indexes)
	{
		auto it = indexes.find(name);
		return (it != indexes.end()) ? it->second : 0;
	}

	void BuildPlayerCustomization(Modules::ModulePlayer &playerVars, PlayerCustomization *out)
	{
		memset(out, 0, sizeof(PlayerCustomization));

		memset(out->Colors, 0, 5 * sizeof(uint32_t));

		uint32_t temp = 0;
		if (playerVars.VarColorsPrimary->ValueString.length() > 0 && playerVars.VarColorsPrimary->ValueString.substr(0, 1) == "#")
			out->Colors[ColorIndices::Primary] = std::stoi(playerVars.VarColorsPrimary->ValueString.substr(1), 0, 16);
		if (playerVars.VarColorsSecondary->ValueString.length() > 0 && playerVars.VarColorsSecondary->ValueString.substr(0, 1) == "#")
			out->Colors[ColorIndices::Secondary] = std::stoi(playerVars.VarColorsSecondary->ValueString.substr(1), 0, 16);
		if (playerVars.VarColorsLights->ValueString.length() > 0 && playerVars.VarColorsLights->ValueString.substr(0, 1) == "#")
			out->Colors[ColorIndices::Lights] = std::stoi(playerVars.VarColorsLights->ValueString.substr(1), 0, 16);
		if (playerVars.VarColorsVisor->ValueString.length() > 0 && playerVars.VarColorsVisor->ValueString.substr(0, 1) == "#")
			out->Colors[ColorIndices::Visor] = std::stoi(playerVars.VarColorsVisor->ValueString.substr(1), 0, 16);
		if (playerVars.VarColorsHolo->ValueString.length() > 0 && playerVars.VarColorsHolo->ValueString.substr(0, 1) == "#")
			out->Colors[ColorIndices::Holo] = std::stoi(playerVars.VarColorsHolo->ValueString.substr(1), 0, 16);

		out->Armor[ArmorIndices::Helmet] = GetArmorIndex(playerVars.VarArmorHelmet->ValueString, helmetIndexes);
		out->Armor[ArmorIndices::Chest] = GetArmorIndex(playerVars.VarArmorChest->ValueString, chestIndexes);
		out->Armor[ArmorIndices::Shoulders] = GetArmorIndex(playerVars.VarArmorShoulders->ValueString, shouldersIndexes);
		out->Armor[ArmorIndices::Arms] = GetArmorIndex(playerVars.VarArmorArms->ValueString, armsIndexes);
		out->Armor[ArmorIndices::Legs] = GetArmorIndex(playerVars.VarArmorLegs->ValueString, legsIndexes);
		out->Armor[ArmorIndices::Acc] = GetArmorIndex(playerVars.VarArmorAccessory->ValueString, accIndexes);
		out->Armor[ArmorIndices::Pelvis] = GetArmorIndex(playerVars.VarArmorPelvis->ValueString, pelvisIndexes);
	}

	uint8_t ValidateArmorPiece(std::unordered_map<std::string, uint8_t> indexes, uint8_t index)
	{
		// Just do a quick check to see if the index has a key associated with it,
		// and force it to 0 if not
		for (auto pair : indexes)
		{
			if (pair.second == index)
				return index;
		}
		return 0;
	}

	class ArmorExtension : public Patches::Network::PlayerPropertiesExtension<PlayerCustomization>
	{
	protected:
		void BuildData(int playerIndex, PlayerCustomization *out) override
		{
			BuildPlayerCustomization(Modules::ModulePlayer::Instance(), out);
		}

		void ApplyData(int playerIndex, PlayerProperties *properties, const PlayerCustomization &data) override
		{
			auto armorSessionData = &properties->Customization;
			armorSessionData->Armor[ArmorIndices::Helmet] = ValidateArmorPiece(helmetIndexes, data.Armor[ArmorIndices::Helmet]);
			armorSessionData->Armor[ArmorIndices::Chest] = ValidateArmorPiece(chestIndexes, data.Armor[ArmorIndices::Chest]);
			armorSessionData->Armor[ArmorIndices::Shoulders] = ValidateArmorPiece(shouldersIndexes, data.Armor[ArmorIndices::Shoulders]);
			armorSessionData->Armor[ArmorIndices::Arms] = ValidateArmorPiece(armsIndexes, data.Armor[ArmorIndices::Arms]);
			armorSessionData->Armor[ArmorIndices::Legs] = ValidateArmorPiece(legsIndexes, data.Armor[ArmorIndices::Legs]);
			armorSessionData->Armor[ArmorIndices::Acc] = ValidateArmorPiece(accIndexes, data.Armor[ArmorIndices::Acc]);
			armorSessionData->Armor[ArmorIndices::Pelvis] = ValidateArmorPiece(pelvisIndexes, data.Armor[ArmorIndices::Pelvis]);
			memcpy(armorSessionData->Colors, data.Colors, sizeof(data.Colors));
		}

		void Serialize(Blam::BitStream *stream, const PlayerCustomization &data) override
		{
			// Colors
			for (int i = 0; i < ColorIndices::Count; i++)
				stream->WriteUnsigned<uint32_t>(data.Colors[i], 24);
			
			// Armor
			for (int i = 0; i < ArmorIndices::Count; i++)
				stream->WriteUnsigned<uint8_t>(data.Armor[i], 0, MaxArmorIndices[i]);
		}

		void Deserialize(Blam::BitStream *stream, PlayerCustomization *out) override
		{
			memset(out, 0, sizeof(PlayerCustomization));

			// Colors
			for (int i = 0; i < ColorIndices::Count; i++)
				out->Colors[i] = stream->ReadUnsigned<uint32_t>(24);

			// Armor
			for (int i = 0; i < ArmorIndices::Count; i++)
				out->Armor[i] = stream->ReadUnsigned<uint8_t>(0, MaxArmorIndices[i]);
		}
	};

	bool updateUiPlayerArmor = false; // Set to true to update the Spartan on the main menu
	void UiPlayerModelArmorHook();
	void ScoreboardPlayerModelArmorHook();
}

namespace Patches
{
	namespace Armor
	{
		void ApplyAll()
		{
			Network::PlayerPropertiesExtender::Instance().Add(std::make_shared<ArmorExtension>());

			// Fix the player model on the main menu
			Hook(0x20086D, UiPlayerModelArmorHook, HookFlags::IsCall).Apply();

			// Fix rendering the scoreboard player model
			// (todo: figure out why your biped doesn't show on the postgame screen...there's probably something missing here)
			Patch::NopFill(Pointer::Base(0x435DAB), 0x50);
			Hook(0x4360D9, ScoreboardPlayerModelArmorHook, HookFlags::IsCall).Apply();
			Patch::NopFill(Pointer::Base(0x4360DE), 0x1A9);
			Pointer::Base(0x43628A).Write<uint8_t>(0x1C);
			Patch::NopFill(Pointer::Base(0x43628B), 0x3);
		}

		void RefreshUiPlayer()
		{
			updateUiPlayerArmor = true;
		}

		void ApplyAfterTagsLoaded()
		{
			auto *matg = Blam::Tags::TagInstance(0x0016).GetDefinition<Blam::Tags::Globals>();
			auto *mulg = Blam::Tags::TagInstance(matg->MultiplayerGlobals.Index).GetDefinition<Blam::Tags::MultiplayerGlobals>();

			for (auto &element : mulg->Universal2[0].GameVariantWeapons)
			{
				auto string = std::string(Blam::Cache::StringIDCache::Instance.GetString(element.Name));
				auto index = (uint16_t)element.Weapon.Index;

				if (index != 0xFFFF)
				{
					weaponIndices.emplace(string, index);
				}
			}
		}

		bool CommandPlayerListRenderWeapons(const std::vector<std::string>& Arguments, std::string& returnInfo)
		{
			std::stringstream ss;

			for (auto &entry : weaponIndices)
			{
				ss << entry.first << std::endl;
			}

			returnInfo = ss.str();
			return true;
		}
	}
}

namespace
{
	__declspec(naked) void PoseWithWeapon(uint32_t unit, uint32_t weaponTag)
	{
		// This is a pretty big hack, basically I don't know where the function pulls the weapon index from
		// so this lets us skip over the beginning of the function and set the weapon tag to whatever we want
		__asm
		{
			push ebp
			mov ebp, esp
			sub esp, 0x18C
			push esi
			push edi
			sub esp, 0x8
			mov esi, unit
			mov edi, weaponTag
			push 0x7B77DA
			ret
		}
	}

	void CustomizeBiped(uint32_t bipedObject)
	{
		auto &playerVars = Modules::ModulePlayer::Instance();

		// Generate customization data
		PlayerCustomization customization;
		BuildPlayerCustomization(playerVars, &customization);

		// Apply armor to the biped
		typedef void(*ApplyArmorPtr)(PlayerCustomization *customization, uint32_t objectDatum);
		ApplyArmorPtr ApplyArmor = reinterpret_cast<ApplyArmorPtr>(0x5A4430);
		ApplyArmor(&customization, bipedObject);

		// Apply each color
		for (int i = 0; i < ColorIndices::Count; i++)
		{
			// Convert the color data from RGB to float3
			float colorData[3];
			typedef void(*RgbToFloatColorPtr)(uint32_t rgbColor, float *result);
			RgbToFloatColorPtr RgbToFloatColor = reinterpret_cast<RgbToFloatColorPtr>(0x521300);
			RgbToFloatColor(customization.Colors[i], colorData);

			// Apply the color
			typedef void(*ApplyArmorColorPtr)(uint32_t objectDatum, int colorIndex, float *colorData);
			ApplyArmorColorPtr ApplyArmorColor = reinterpret_cast<ApplyArmorColorPtr>(0xB328F0);
			ApplyArmorColor(bipedObject, i, colorData);
		}

		// Need to call this or else colors don't actually show up
		typedef void(*UpdateArmorColorsPtr)(uint32_t objectDatum);
		UpdateArmorColorsPtr UpdateArmorColors = reinterpret_cast<UpdateArmorColorsPtr>(0x5A2FA0);
		UpdateArmorColors(bipedObject);

		// Give the biped a weapon (0x151E = tag index for Assault Rifle)
		auto weaponName = playerVars.VarRenderWeapon->ValueString;

		if (weaponIndices.find(weaponName) == weaponIndices.end())
			weaponName = (playerVars.VarRenderWeapon->ValueString = "assault_rifle");

		PoseWithWeapon(bipedObject, weaponIndices.find(weaponName)->second);
	}

	void UiPlayerModelArmorHook()
	{
		// This function runs every tick, so only update if necessary
		if (!updateUiPlayerArmor)
			return;

		// Try to get the UI player biped
		uint32_t uiPlayerBiped = Pointer::Base(0x4BE67A0).Read<uint32_t>();
		if (uiPlayerBiped == 0xFFFFFFFF)
			return;

		CustomizeBiped(uiPlayerBiped);
		updateUiPlayerArmor = false;

		// Note: the call that this hook replaces is for setting up armor based on the server data,
		// so it's not necessary to call here
	}

	void ScoreboardPlayerModelArmorHook()
	{
		uint32_t scoreboardBiped = Pointer::Base(0x4C4C698).Read<uint32_t>();
		if (scoreboardBiped != 0xFFFFFFFF)
			CustomizeBiped(scoreboardBiped);
	}
}

namespace
{
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

	std::map<std::string, uint16_t> weaponIndices = {

	};
}
