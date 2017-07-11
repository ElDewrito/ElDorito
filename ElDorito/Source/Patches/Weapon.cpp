#include <iomanip>
#include <fstream>
#include <map>

#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include "../Console.hpp"

#include "../Modules/ModuleWeapon.hpp"
#include "../Modules/ModuleServer.hpp"

#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"

#include "../Blam/Cache/StringIdCache.hpp"

#include "../Blam/Math/RealVector3D.hpp"

#include "../Blam/Tags/Tags.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"

#include "../Patches/Core.hpp"
#include "../Patches/Weapon.hpp"

#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"

namespace
{
	int __cdecl DualWieldHook(unsigned short objectIndex);
	void DualWieldSprintInputHook();
	void DualWieldScopeLevelHook();
	int DualWieldEquipmentCountHook(uint32_t unitIndex, short equipmentIndex);
}

namespace Patches
{
	namespace Weapon
	{
		using Blam::Math::RealVector3D;
		using Blam::Tags::TagInstance;

		bool IsNotMainMenu;
		std::string ConfigPath;
		std::map<std::string, uint16_t> weaponIndices;
		std::map<std::string, RealVector3D> weaponOffsetsDefault;
		std::map<std::string, RealVector3D> weaponOffsetsModified;

		//Callback for when the loading screen back to the main menu finishes. We use this to determine when to start a new vote.
		void MapLoadedCallback(const char *mapPath)
		{
			std::string currentMap = mapPath;
			auto separatorIndex = currentMap.find_first_of("\\/");
			auto mapName = currentMap.substr(separatorIndex + 1);

			if (mapName == "mainmenu")
			{
				IsNotMainMenu = false;
			}
			else
			{
				IsNotMainMenu = true;

				if (!weaponOffsetsDefault.empty())
				{
					for (auto &element : weaponIndices)
					{
						std::string selected = element.first;
						auto *weapon = TagInstance(Patches::Weapon::GetIndex(selected)).GetDefinition<Blam::Tags::Items::Weapon>();
						weaponOffsetsDefault.emplace(selected, weapon->FirstPersonWeaponOffset);
					}
				}

				if (!weaponOffsetsModified.empty())
				{
					Config::Save(ConfigPath);
				}
				else
				{
					for (auto &weaponParams : weaponOffsetsModified)
					{
						std::string weaponName = weaponParams.first;
						RealVector3D weaponOffset = weaponParams.second;
						ApplyOffset(weaponName, weaponOffset);
					}
				}
			}
		}

		void ApplyAll()
		{
			ConfigPath = Modules::ModuleWeapon::Instance().VarWeaponConfig->ValueString;
			Config::Load(ConfigPath);
			Patches::Core::OnMapLoaded(MapLoadedCallback);

			// Force descoping for all weapons
			Pointer::Base(0x73F1E6).Write<uint8_t>(0);

			Hook(0x761550, DualWieldHook).Apply();
			Hook(0x06DFBB, DualWieldSprintInputHook).Apply();
			Hook(0x1D50CB, DualWieldScopeLevelHook).Apply();
			Hook(0x7A21D4, DualWieldEquipmentCountHook, HookFlags::IsCall).Apply();
		}

		void ApplyAfterTagsLoaded()
		{
			using Blam::Tags::Game::Globals;
			using Blam::Tags::Game::MultiplayerGlobals;

			auto *matg = TagInstance(0x0016).GetDefinition<Globals>();
			auto *mulg = TagInstance(matg->MultiplayerGlobals.TagIndex).GetDefinition<MultiplayerGlobals>();

			for (auto &element : mulg->Universal->GameVariantWeapons)
			{
				auto string = std::string(Blam::Cache::StringIDCache::Instance.GetString(element.Name));
				auto index = (uint16_t)element.Weapon.TagIndex;

				if (index != 0xFFFF)
					weaponIndices.emplace(string, index);
			}
		}

		std::map<std::string, uint16_t> GetIndices()
		{
			return weaponIndices;
		}

		uint16_t GetIndex(std::string &weaponName)
		{
			if (weaponIndices.find(weaponName) == weaponIndices.end())
				return 0xFFFF;

			return weaponIndices.find(weaponName)->second;
		}

		std::string GetName(uint16_t & weaponIndex)
		{
			std::string weaponName;

			bool indexExists = false;

			for (auto &element : weaponIndices)
			{
				auto name = element.first;
				auto index = element.second;

				if (weaponIndex == index) {
					weaponName = name;
					indexExists = true;
				}
			}
			if (indexExists == false)
				return "NotFoundInMULG";

			return weaponName;
		}

		uint16_t GetEquippedWeaponIndex()
		{
			uint16_t equippedIndex = 0xFFFF;

			auto session = Blam::Network::GetActiveSession();
			if (!session || !session->IsEstablished())
			{
				return equippedIndex;
			}

			int playerIdx = session->MembershipInfo.FindFirstPlayer();
			auto playerDatum = &Blam::Players::GetPlayers()[playerIdx];
			auto playerObject = Pointer(Blam::Objects::GetObjects()[playerDatum->SlaveUnit].Data);
			if (playerObject)
			{
				auto equippedWeaponIndex = playerObject(0x2CA).Read<uint8_t>();
				if (equippedWeaponIndex != -1)
				{
					auto equippedWeaponObjectIndex = playerObject(0x2D0 + 4 * equippedWeaponIndex).Read<uint32_t>();
					auto equippedWeaponObjectPtr = Pointer(Blam::Objects::GetObjects()[equippedWeaponObjectIndex].Data);
					if (equippedWeaponObjectPtr)
					{
						auto weap = Blam::Tags::TagInstance(Pointer(equippedWeaponObjectPtr).Read<uint32_t>()).GetDefinition<Blam::Tags::Items::Weapon>();
						equippedIndex = Pointer(equippedWeaponObjectPtr).Read<uint32_t>();
					}
				}
			}
			return equippedIndex;
		}

		std::string GetEquippedWeaponName()
		{
			auto Index = GetEquippedWeaponIndex();

			return GetName(Index);
		}

		RealVector3D GetOffsetByIndex(bool isDefault, uint16_t &weaponIndex)
		{
			for each (auto element in weaponIndices)
			{
				std::string weaponName = element.first;

				if (weaponIndex == element.second)
				{
					if (isDefault)
						return weaponOffsetsDefault.find(weaponName)->second;
					else
						return weaponOffsetsModified.find(weaponName)->second;
				}
			}

			return { -0xFFFFFFFF, -0xFFFFFFFF, -0xFFFFFFFF };
		}

		RealVector3D GetOffsetByName(bool isDefault, std::string &weaponName)
		{
			if (isDefault)
				return weaponOffsetsDefault.find(weaponName)->second;
			else
				return weaponOffsetsModified.find(weaponName)->second;

			return { -0xFFFFFFFF, -0xFFFFFFFF, -0xFFFFFFFF };
		}

		void SetOffsetModified(std::string &weaponName, RealVector3D &weaponOffset)
		{
			if (weaponOffsetsDefault.find(weaponName)->second == weaponOffset)
			{
				weaponOffsetsModified.erase(weaponName);
				ApplyOffset(weaponName, weaponOffset);
			}
			else
			{
				weaponOffsetsModified.try_emplace(weaponName, weaponOffset);
				ApplyOffset(weaponName, weaponOffset);
			}
		}

		void ApplyOffset(std::string &weaponName, RealVector3D &weaponOffset)
		{
			if (IsNotMainMenu)
			{
				auto *weapon = TagInstance(Patches::Weapon::GetIndex(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
				weapon->FirstPersonWeaponOffset = weaponOffset;
			}
		}

		bool IsOffsetModified(const std::string &weapon)
		{
			return weaponOffsetsModified.find(weapon) != weaponOffsetsModified.end();
		}

		namespace Config
		{
			void Load(std::string configPath)
			{
				std::ifstream inFile(configPath);
				std::vector <std::string> lines;
				std::string str;

				while (std::getline(inFile, str))
					lines.push_back(str);

				weaponOffsetsModified.clear();

				for (std::string line : lines)
				{
					auto weaponParams = Utils::String::SplitString(line, ' ');

					if ((weaponParams.size() < 4 || weaponParams.size() > 4))
					{
#if _DEBUG
						Console::WriteLine(line);
#endif
					}
					else if (line[0] != '#')
					{
						std::string weaponName = weaponParams[0];
						RealVector3D offset = { stof(weaponParams[1]), stof(weaponParams[2]), stof(weaponParams[3]) };

						SetOffsetModified(weaponName, offset);
					}
					else
					{
#if _DEBUG
						Console::WriteLine(line);
#endif
					}
				}
			}

			void Save(std::string configPath)
			{
				std::ofstream outFile(configPath, std::ios::trunc);

				for (auto &weaponParams : weaponOffsetsModified)
				{
					std::string weaponName = weaponParams.first;
					auto *weapon = TagInstance(Patches::Weapon::GetIndex(weaponName)).GetDefinition<Blam::Tags::Items::Weapon>();
					weapon->FirstPersonWeaponOffset = weaponParams.second;
					outFile << weaponName << " " << weaponParams.second.I << " " << weaponParams.second.J << " " << weaponParams.second.K << "\n";
				}
			}

			/* This is copypasta from voting json, going to model the newer weapon config on it
			bool LoadJson(std::string filename)
			{
				std::ifstream in(filename, std::ios::in | std::ios::binary);
				if (!in || !in.is_open())
					return false;

				std::string contents;
				in.seekg(0, std::ios::end);
				contents.resize((unsigned int)in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&contents[0], contents.size());
				in.close();

				rapidjson::Document document;
				if (!document.Parse<0>(contents.c_str()).HasParseError() && document.IsObject())
				{
					if (!document.HasMember("Types") || !document.HasMember("Maps"))
						return false;

					const rapidjson::Value& maps = document["Maps"];
					// rapidjson uses SizeType instead of size_t :/
					for (rapidjson::SizeType i = 0; i < maps.Size(); i++)
					{
						const rapidjson::Value& mapObject = maps[i];
						if (!mapObject.HasMember("mapName") || !mapObject.HasMember("displayName"))
							continue;

						//Check to make sure that the map exists
						std::string mapName = mapObject["mapName"].GetString();
						if (std::find(defaultMaps.begin(), defaultMaps.end(), mapName) != defaultMaps.end())
							haloMaps.push_back(HaloMap(mapName, mapObject["displayName"].GetString(), getDefaultMapId(mapName)));

						else if (std::find(customMaps.begin(), customMaps.end(), mapName) != customMaps.end())
							haloMaps.push_back(HaloMap(mapName, mapObject["displayName"].GetString(), getCustomMapID(mapName)));

						else
							Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Invalid Map: " + mapName + ", skipping..");


					}

					const rapidjson::Value& types = document["Types"];
					for (rapidjson::SizeType i = 0; i < types.Size(); i++)
					{
						const rapidjson::Value& c = types[i];
						if (!c.HasMember("typeName") || !c.HasMember("displayName"))
							continue;

						//TODO verify the gametypes
						HaloType ht(c["typeName"].GetString(), c["displayName"].GetString());


						if (c.HasMember("commands"))
						{
							const rapidjson::Value& commands = c["commands"];
							for (rapidjson::SizeType i = 0; i < commands.Size(); i++)
							{
								ht.commands.push_back(commands[i].GetString());
							}
						}

						if (c.HasMember("SpecificMaps"))
						{
							const rapidjson::Value& smaps = c["SpecificMaps"];
							if (!smaps.IsNull())
							{
								for (rapidjson::SizeType i = 0; i < smaps.Size(); i++)
								{
									const rapidjson::Value& map = smaps[i];
									if (!map.HasMember("mapName") || !map.HasMember("displayName"))
										continue;

									std::string mapName = map["mapName"].GetString();
									if (std::find(defaultMaps.begin(), defaultMaps.end(), mapName) != defaultMaps.end())
										ht.specificMaps.push_back(HaloMap(mapName, map["displayName"].GetString(), getDefaultMapId(mapName)));

									else if (std::find(customMaps.begin(), customMaps.end(), mapName) != customMaps.end())
										ht.specificMaps.push_back(HaloMap(mapName, map["displayName"].GetString(), getCustomMapID(mapName)));
									else
										Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Invalid Map: " + mapName + ", skipping..");
								}
							}
						}
						gameTypes.push_back(ht);

					}
				}

				if (gameTypes.size() < 2 || haloMaps.size() < 2)
					return false;


				return true;
			}
			*/
		}
	}
}

namespace
{
	bool UnitIsDualWielding(Blam::DatumIndex unitIndex)
	{
		if (!unitIndex)
			return false;
		auto objectHeaderArrayPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		auto unitDatumPtr = objectHeaderArrayPtr(0x44)[0](unitIndex.Index() * 0x10)(0xC)[0];

		if (!unitDatumPtr)
			return false;

		auto dualWieldWeaponIndex = unitDatumPtr(0x2CB).Read<int8_t>();

		if (dualWieldWeaponIndex < 0 || dualWieldWeaponIndex >= 4)
			return false;

		typedef uint32_t(*UnitGetWeaponPtr)(uint32_t unitObject, short weaponIndex);
		auto UnitGetWeapon = reinterpret_cast<UnitGetWeaponPtr>(0xB454D0);

		return UnitGetWeapon(unitIndex, dualWieldWeaponIndex) != 0xFFFFFFFF;
	}

	bool PlayerIsDualWielding(Blam::DatumIndex playerIndex)
	{
		auto &players = Blam::Players::GetPlayers();
		return UnitIsDualWielding(players[playerIndex].SlaveUnit);
	}

	bool LocalPlayerIsDualWielding()
	{
		auto localPlayer = Blam::Players::GetLocalPlayer(0);
		if (localPlayer == Blam::DatumIndex::Null)
			return false;
		return PlayerIsDualWielding(localPlayer);
	}

	uint32_t GetObjectDataAddress(uint32_t objectDatum)
	{
		uint32_t objectIndex = objectDatum & UINT16_MAX;
		Pointer objectHeaderPtr = ElDorito::GetMainTls(GameGlobals::ObjectHeader::TLSOffset)[0];
		uint32_t objectAddress = objectHeaderPtr(0x44).Read<uint32_t>() + 0xC + objectIndex * 0x10;
		return *(uint32_t*)(objectAddress);
	}

	int __cdecl DualWieldHook(unsigned short objectIndex)
	{
		using Blam::Tags::TagInstance;
		using Blam::Tags::Items::Weapon;

		if (!Modules::ModuleServer::Instance().VarServerDualWieldEnabledClient->ValueInt)
			return 0;

		auto index = *(uint32_t*)GetObjectDataAddress(objectIndex);
		auto *weapon = TagInstance(index).GetDefinition<Weapon>();

		return ((int32_t)weapon->WeaponFlags1 & (int32_t)Weapon::Flags1::CanBeDualWielded) != 0;
	}

	__declspec(naked) void DualWieldSprintInputHook()
	{
		__asm
		{
			push	eax
			call	LocalPlayerIsDualWielding
			test	al, al
			pop		eax
			jz		enable // leave sprint enabled(for now) if not dual wielding
			and		ax, 0FEFFh // disable by removing the 8th bit indicating no sprint input press
		enable:
			mov		dword ptr ds : [esi + 8], eax
				mov		ecx, edi
				push	046DFC0h
				ret
		}
	}

	// scope level is an int16 with -1 indicating no scope, 0 indicating first level, 1 indicating second level etc.
	__declspec(naked) void DualWieldScopeLevelHook()
	{
		__asm
		{
			mov		word ptr ds : [edi + esi + 32Ah], 0FFFFh // no scope by default
			push	eax
			push	ecx
			call	LocalPlayerIsDualWielding
			test	al, al
			pop		ecx
			pop		eax
			jnz		noscope // prevent scoping when dual wielding
			mov		word ptr ds : [edi + esi + 32Ah], ax // otherwise use intended scope level
		noscope:
			push	05D50D3h
				ret
		}
	}

	int DualWieldEquipmentCountHook(uint32_t unitIndex, short equipmentIndex)
	{
		typedef int(__cdecl* GetEquipmentCountFunc)(uint32_t unitIndex, short equipmentIndex);
		GetEquipmentCountFunc GetEquipmentCount = reinterpret_cast<GetEquipmentCountFunc>(0xB440F0);
		return GetEquipmentCount(unitIndex, equipmentIndex);
	}
}