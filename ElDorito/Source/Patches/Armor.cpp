
#include <unordered_map>
#include <map>

#include "Armor.hpp"
#include "PlayerPropertiesExtension.hpp"
#include "../Patch.hpp"
#include "../Modules/ModulePlayer.hpp"

#include "../Blam/Cache/StringIdCache.hpp"
#include "../Blam/Tags/Tags.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"

using namespace Blam::Players;

namespace
{
	// Used during bitstream operations to automatically calculate the size of each armor component
	const uint8_t MaxArmorIndices[] = { 81, 82, 82, 50, 52, 24, 4 };
	
	std::map<std::string, uint8_t> helmetIndices;
	std::map<std::string, uint8_t> chestIndices;
	std::map<std::string, uint8_t> shouldersIndices;
	std::map<std::string, uint8_t> armsIndices;
	std::map<std::string, uint8_t> legsIndices;
	std::map<std::string, uint8_t> accIndices;
	std::map<std::string, uint8_t> pelvisIndices;
	std::map<std::string, uint16_t> weaponIndices;

	uint8_t GetArmorIndex(const std::string &name, const std::map<std::string, uint8_t> &Indices)
	{
		auto it = Indices.find(name);
		return (it != Indices.end()) ? it->second : 0;
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

		out->Armor[ArmorIndices::Helmet] = GetArmorIndex(playerVars.VarArmorHelmet->ValueString, helmetIndices);
		out->Armor[ArmorIndices::Chest] = GetArmorIndex(playerVars.VarArmorChest->ValueString, chestIndices);
		out->Armor[ArmorIndices::Shoulders] = GetArmorIndex(playerVars.VarArmorShoulders->ValueString, shouldersIndices);
		out->Armor[ArmorIndices::Arms] = GetArmorIndex(playerVars.VarArmorArms->ValueString, armsIndices);
		out->Armor[ArmorIndices::Legs] = GetArmorIndex(playerVars.VarArmorLegs->ValueString, legsIndices);
		out->Armor[ArmorIndices::Acc] = GetArmorIndex(playerVars.VarArmorAccessory->ValueString, accIndices);
		out->Armor[ArmorIndices::Pelvis] = GetArmorIndex(playerVars.VarArmorPelvis->ValueString, pelvisIndices);

		out->Unknown1C = (playerVars.VarRepresentation->ValueString == "elite") ? 0xCC : 0x129;
	}

	uint8_t ValidateArmorPiece(const std::map<std::string, uint8_t> &indices, const uint8_t index)
	{
		// Just do a quick check to see if the index has a key associated with it,
		// and force it to 0 if not
		for (auto pair : indices)
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
			if (data.Unknown1C == 0xCC /* "dervish" stringid */)
				properties->PlayerRepresentation = 1; // elite
			else if (data.Unknown1C == 0x129)
				properties->PlayerRepresentation = 0; // spartan

			auto armorSessionData = &properties->Customization;
			armorSessionData->Armor[ArmorIndices::Helmet] = ValidateArmorPiece(helmetIndices, data.Armor[ArmorIndices::Helmet]);
			armorSessionData->Armor[ArmorIndices::Chest] = ValidateArmorPiece(chestIndices, data.Armor[ArmorIndices::Chest]);
			armorSessionData->Armor[ArmorIndices::Shoulders] = ValidateArmorPiece(shouldersIndices, data.Armor[ArmorIndices::Shoulders]);
			armorSessionData->Armor[ArmorIndices::Arms] = ValidateArmorPiece(armsIndices, data.Armor[ArmorIndices::Arms]);
			armorSessionData->Armor[ArmorIndices::Legs] = ValidateArmorPiece(legsIndices, data.Armor[ArmorIndices::Legs]);
			armorSessionData->Armor[ArmorIndices::Acc] = ValidateArmorPiece(accIndices, data.Armor[ArmorIndices::Acc]);
			armorSessionData->Armor[ArmorIndices::Pelvis] = ValidateArmorPiece(pelvisIndices, data.Armor[ArmorIndices::Pelvis]);
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

			stream->WriteUnsigned<uint32_t>(data.Unknown1C, 0, 0xFFFFFFFF);
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

			out->Unknown1C = stream->ReadUnsigned<uint32_t>(0, 0xFFFFFFFF);
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
		const auto sub_59A6F0 = reinterpret_cast<bool(__cdecl *)(Blam::DatumIndex, uint32_t *, uint32_t *)>(0x59A6F0);

		int32_t __cdecl Player_GetRepresentationIndex(Blam::DatumIndex p_PlayerDatumHandle)
		{
			auto *s_GlobalsDefinition = *reinterpret_cast<Blam::Tags::Game::Globals **>(0x22AAEB8);
			auto &s_PlayerDatum = Blam::Players::GetPlayers()[p_PlayerDatumHandle];

			// TODO: Make this proper
			if (sub_59A6F0(p_PlayerDatumHandle, nullptr, nullptr))
				return 5; // monitor
			else if (s_PlayerDatum.Properties.PlayerRepresentation == 1)
				return 3; // elite
			else
				return 2; // spartan
		}

		struct Blam::Tags::Game::Globals::PlayerRepresentation *__cdecl Player_GetRepresentation(Blam::DatumIndex p_PlayerDatumHandle)
		{
			auto *s_GlobalsDefinition = *reinterpret_cast<Blam::Tags::Game::Globals **>(0x22AAEB8);
			auto s_PlayerRepresentationIndex = Player_GetRepresentationIndex(p_PlayerDatumHandle);

			return &s_GlobalsDefinition->PlayerRepresentation[s_PlayerRepresentationIndex];
		}

		const auto sub_6D9900 = reinterpret_cast<int(__cdecl *)(int, int)>(0x6D9900);

		int __cdecl Player_GetThirdPersonUnitVariant(Blam::DatumIndex p_PlayerDatumHandle)
		{
			if (!p_PlayerDatumHandle)
				return 0;

			auto *s_GlobalsDefinition = *reinterpret_cast<Blam::Tags::Game::Globals **>(0x22AAEB8);
			auto s_PlayerRepresentationIndex = Player_GetRepresentationIndex(p_PlayerDatumHandle);

			if (s_PlayerRepresentationIndex >= s_GlobalsDefinition->PlayerRepresentation.Count)
				return 0;

			auto s_UnitIndex = s_GlobalsDefinition->PlayerRepresentation[s_PlayerRepresentationIndex].ThirdPersonUnit.TagIndex;
			auto *s_Biped = Blam::Tags::TagInstance(s_UnitIndex).GetDefinition<uint8_t>();
			auto *s_BipedModel = Blam::Tags::TagInstance(*(uint32_t *)(s_Biped + 64)).GetDefinition<uint8_t>();

			// TODO: Make this proper
			auto s_ModelVariantIndex = sub_6D9900(*(uint32_t *)(s_Biped + 64), *(uint32_t *)((uint8_t *)&s_GlobalsDefinition->PlayerRepresentation[s_PlayerRepresentationIndex] + 56));

			if (s_ModelVariantIndex == -1)
				return 0;

			// TODO: Make this proper
			return *(uint32_t *)(s_BipedModel + 104) + 80 * s_ModelVariantIndex;
		}

		int *__cdecl Player_GetThirdPersonRepresentation(Blam::DatumIndex p_PlayerDatumHandle, int *p_ThirdPersonTagIndex, int *p_ThirdPersonVariant)
		{
			auto *s_GlobalsDefinition = *reinterpret_cast<Blam::Tags::Game::Globals **>(0x22AAEB8);
			auto s_PlayerRepresentationIndex = Player_GetRepresentationIndex(p_PlayerDatumHandle);

			auto s_ThirdPersonTagIndex = -1;
			auto s_ThirdPersonVariant = -1;

			if (s_PlayerRepresentationIndex < s_GlobalsDefinition->PlayerRepresentation.Count)
			{
				auto &s_ThirdPersonRepresentation = s_GlobalsDefinition->PlayerRepresentation[s_PlayerRepresentationIndex];
				s_ThirdPersonTagIndex = s_ThirdPersonRepresentation.ThirdPersonUnit.TagIndex;
				s_ThirdPersonVariant = s_ThirdPersonRepresentation.ThirdPersonVariant;
			}

			if (p_ThirdPersonTagIndex)
				*p_ThirdPersonTagIndex = s_ThirdPersonTagIndex;

			if (p_ThirdPersonVariant)
				*p_ThirdPersonVariant = s_ThirdPersonVariant;

			return p_ThirdPersonVariant;
		}

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

			// Player representation crap
			Hook(0x13A6F0, Player_GetRepresentation).Apply();
			Hook(0x13A770, Player_GetRepresentationIndex).Apply();
			Hook(0x137430, Player_GetThirdPersonUnitVariant).Apply();
			Hook(0x139F70, Player_GetThirdPersonRepresentation).Apply();
		}

		void RefreshUiPlayer()
		{
			updateUiPlayerArmor = true;
		}

		void AddArmorPermutations(const Blam::Tags::Game::MultiplayerGlobals::Universal::ArmorCustomization &element, std::map<std::string, uint8_t> &map)
		{
			for (auto i = 0; i < element.Permutations.Count; i++)
			{
				auto &perm = element.Permutations[i];

				if (!perm.FirstPersonArmorModel && !perm.ThirdPersonArmorObject)
				{
					continue;
				}

				auto permName = std::string(Blam::Cache::StringIDCache::Instance.GetString(perm.Name));

				map.emplace(permName, i);
			}
		}

		void ApplyAfterTagsLoaded()
		{
			using Blam::Tags::TagInstance;
			using Blam::Tags::Game::Globals;
			using Blam::Tags::Game::MultiplayerGlobals;

			auto *matg = TagInstance(0x0016).GetDefinition<Globals>();
			auto *mulg = TagInstance(matg->MultiplayerGlobals.TagIndex).GetDefinition<MultiplayerGlobals>();

			for (auto &element : mulg->Universal->SpartanArmorCustomization)
			{
				auto string = std::string(Blam::Cache::StringIDCache::Instance.GetString(element.PieceRegion));

				if (string == "helmet")
				{
					AddArmorPermutations(element, helmetIndices);
				}
				else if (string == "chest")
				{
					AddArmorPermutations(element, chestIndices);
				}
				else if (string == "shoulders")
				{
					AddArmorPermutations(element, shouldersIndices);
				}
				else if (string == "arms")
				{
					AddArmorPermutations(element, armsIndices);
				}
				else if (string == "legs")
				{
					AddArmorPermutations(element, legsIndices);
				}
				else if (string == "acc")
				{
					AddArmorPermutations(element, accIndices);
				}
				else if (string == "pelvis")
				{
					AddArmorPermutations(element, pelvisIndices);
				}
				else
				{
					throw std::exception("Invalid armor section");
				}
			}

			for (auto &element : mulg->Universal->GameVariantWeapons)
			{
				auto string = std::string(Blam::Cache::StringIDCache::Instance.GetString(element.Name));
				auto index = (uint16_t)element.Weapon.TagIndex;

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
		//
		// TODO: Fix this for elites/other races
		//

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