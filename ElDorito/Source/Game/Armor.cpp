
#include <unordered_map>
#include <map>

#include "Armor.hpp"
#include "../Patch.hpp"
#include "../Modules/ModulePlayer.hpp"

#include "../Blam/Cache/StringIdCache.hpp"
#include "../Blam/Tags/Tags.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/Math/RealQuaternion.hpp"
#include <boost/regex.hpp>

using namespace Blam::Players;

namespace
{
	struct UiPlayerModelState
	{
		enum StateFlags
		{
			eStateFlagsNone = 0,
			eStateFlagsRotation = 1 << 0,
			eStateFlagsTranslation = 1 << 1
		};

		uint16_t Flags;
		Blam::Math::RealVector3D Position;
		float RotationAngle;

	} s_UiPlayerModelState;

	// Used during bitstream operations to automatically calculate the size of each armor component
	const uint8_t MaxArmorIndices[] = { 81, 82, 82, 50, 52, 24, 4 };

	std::map<std::string, uint8_t> helmetIndices;
	std::map<std::string, uint8_t> chestIndices;
	std::map<std::string, uint8_t> rightShoulderIndices;
	std::map<std::string, uint8_t> leftShoulderIndices;
	std::map<std::string, uint16_t> weaponIndices;

	bool updateUiPlayerArmor = false; // Set to true to update the Spartan on the main menu

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

		boost::cmatch what;
		boost::regex expression("#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})");
		if(boost::regex_match(playerVars.VarColorsPrimary->ValueString.c_str(), what, expression))
			out->Colors[ColorIndices::Primary] = std::stoi(playerVars.VarColorsPrimary->ValueString.substr(1), 0, 16);
		if (boost::regex_match(playerVars.VarColorsSecondary->ValueString.c_str(), what, expression))
			out->Colors[ColorIndices::Secondary] = std::stoi(playerVars.VarColorsSecondary->ValueString.substr(1), 0, 16);
		if (boost::regex_match(playerVars.VarColorsLights->ValueString.c_str(), what, expression))
			out->Colors[ColorIndices::Lights] = std::stoi(playerVars.VarColorsLights->ValueString.substr(1), 0, 16);
		if (boost::regex_match(playerVars.VarColorsVisor->ValueString.c_str(), what, expression))
			out->Colors[ColorIndices::Visor] = std::stoi(playerVars.VarColorsVisor->ValueString.substr(1), 0, 16);
		if (boost::regex_match(playerVars.VarColorsHolo->ValueString.c_str(), what, expression))
			out->Colors[ColorIndices::Holo] = std::stoi(playerVars.VarColorsHolo->ValueString.substr(1), 0, 16);

		out->Armor[ArmorIndices::Helmet] = GetArmorIndex(playerVars.VarArmorHelmet->ValueString, helmetIndices);
		out->Armor[ArmorIndices::Chest] = GetArmorIndex(playerVars.VarArmorChest->ValueString, chestIndices);
		out->Armor[ArmorIndices::RightShoulder] = GetArmorIndex(playerVars.VarArmorRightShoulder->ValueString, rightShoulderIndices);
		out->Armor[ArmorIndices::LeftShoulder] = GetArmorIndex(playerVars.VarArmorLeftShoulder->ValueString, leftShoulderIndices);
	}

	uint8_t ValidateArmorPiece(const std::map<std::string, uint8_t> &indices, const uint8_t index)
	{
		// Just do a quick check to see if the index has a key associated with it,
		// and force it to 0 if not
		for (auto pair : indices)
			if (pair.second == index)
				return index;

		return 0;
	}
}

namespace Game::Armor
{
	void ArmorExtension::BuildData(int playerIndex, PlayerCustomization *out)
	{
		BuildPlayerCustomization(Modules::ModulePlayer::Instance(), out);
	}

	void ArmorExtension::ApplyData(int playerIndex, PlayerProperties *properties, const PlayerCustomization &data)
	{
		auto armorSessionData = &properties->Customization;
		armorSessionData->Armor[ArmorIndices::Helmet] = ValidateArmorPiece(helmetIndices, data.Armor[ArmorIndices::Helmet]);
		armorSessionData->Armor[ArmorIndices::Chest] = ValidateArmorPiece(chestIndices, data.Armor[ArmorIndices::Chest]);
		armorSessionData->Armor[ArmorIndices::RightShoulder] = ValidateArmorPiece(rightShoulderIndices, data.Armor[ArmorIndices::RightShoulder]);
		armorSessionData->Armor[ArmorIndices::LeftShoulder] = ValidateArmorPiece(leftShoulderIndices, data.Armor[ArmorIndices::LeftShoulder]);
		memcpy(armorSessionData->Colors, data.Colors, sizeof(data.Colors));
	}

	void ArmorExtension::Serialize(Blam::BitStream *stream, const PlayerCustomization &data)
	{
		// Colors
		for (int i = 0; i < ColorIndices::Count; i++)
			stream->WriteUnsigned<uint32_t>(data.Colors[i], 24);

		// Armor
		for (int i = 0; i < ArmorIndices::Count; i++)
			stream->WriteUnsigned<uint8_t>(data.Armor[i], 0, MaxArmorIndices[i]);

		// Unused
		for (int i = 0; i < 3; i++)
			stream->WriteUnsigned<uint8_t>(0, 8);

		stream->WriteUnsigned<uint32_t>(data.Unknown1C, 0, 0xFFFFFFFF);
	}

	void ArmorExtension::Deserialize(Blam::BitStream *stream, PlayerCustomization *out)
	{
		memset(out, 0, sizeof(PlayerCustomization));

		// Colors
		for (int i = 0; i < ColorIndices::Count; i++)
			out->Colors[i] = stream->ReadUnsigned<uint32_t>(24);

		// Armor
		for (int i = 0; i < ArmorIndices::Count; i++)
			out->Armor[i] = stream->ReadUnsigned<uint8_t>(0, MaxArmorIndices[i]);

		// Unused
		for (int i = 0; i < 3; i++)
			stream->ReadUnsigned<uint8_t>(8);

		out->Unknown1C = stream->ReadUnsigned<uint32_t>(0, 0xFFFFFFFF);
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
				continue;

			auto permName = std::string(Blam::Cache::StringIDCache::Instance.GetString(perm.Name));

			map.emplace(permName, i);
		}
	}

	void LoadArmorPermutations()
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
				AddArmorPermutations(element, helmetIndices);
			else if (string == "chest")
				AddArmorPermutations(element, chestIndices);
			else if (string == "rightshoulder")
				AddArmorPermutations(element, rightShoulderIndices);
			else if (string == "leftshoulder")
				AddArmorPermutations(element, leftShoulderIndices);
		}
		for (auto &element : mulg->Universal->GameVariantWeapons)
		{
			if (element.Weapon.TagIndex == -1)
				continue;

			weaponIndices.emplace(
				std::string(Blam::Cache::StringIDCache::Instance.GetString(element.Name)),
				(uint16_t)element.Weapon.TagIndex);
		}
	}

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

	void UpdateUiPlayerModelArmor()
	{
		using namespace Blam::Math;

		static auto Object_SetTransform = (void(*)(int objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up, int a5))(0x00B33530);
		static auto GetCharPlatformBiped = (int(*)(int playerRepresentationIndex))(0x00BB5BD0);

		// Try to get the UI player biped
		uint32_t uiPlayerBiped = GetCharPlatformBiped(2);
		if (uiPlayerBiped == 0xFFFFFFFF)
			return;

		if (s_UiPlayerModelState.Flags & UiPlayerModelState::eStateFlagsRotation)
		{
			*(float*)0x194A66C = s_UiPlayerModelState.RotationAngle;
			*(uint8_t*)0x5287C3C = 1; // mark dirty
			s_UiPlayerModelState.Flags &= ~UiPlayerModelState::eStateFlagsRotation;
		}

		if (s_UiPlayerModelState.Flags & UiPlayerModelState::eStateFlagsTranslation)
		{
			Object_SetTransform(uiPlayerBiped, &s_UiPlayerModelState.Position, nullptr, nullptr, 0);
			s_UiPlayerModelState.Flags &= ~UiPlayerModelState::eStateFlagsTranslation;
		}

		// This function runs every tick, so only update if necessary
		if (!updateUiPlayerArmor)
			return;

		CustomizeBiped(uiPlayerBiped);
		updateUiPlayerArmor = false;
	}

	void SetUiPlayerModelTransform(const Blam::Math::RealVector3D* newPosition, const float* rotationAngle)
	{
		if (newPosition)
		{
			s_UiPlayerModelState.Position = *newPosition;
			s_UiPlayerModelState.Flags |= UiPlayerModelState::eStateFlagsTranslation;
		}

		if (rotationAngle)
		{
			s_UiPlayerModelState.RotationAngle = *rotationAngle;
			s_UiPlayerModelState.Flags |= UiPlayerModelState::eStateFlagsRotation;
		}
	}
}