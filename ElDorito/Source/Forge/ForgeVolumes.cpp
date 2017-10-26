#include "ForgeVolumes.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Modules/ModuleForge.hpp"
#include "ForgeUtil.hpp"
#include <unordered_map>

namespace
{
	using namespace Forge;
	using namespace Blam::Math;

	const auto MAX_VOLUMES = 64;
	const auto SCAN_INTERVAL = 1;
	const auto DAMAGE_EFFECT_TAG_INDEX = 0x000001ED;
	const auto DAMAGE_CAUSE_TYPE = 1; // guardians

	enum VolumeType : uint8_t
	{
		eVolumeType_Disabled = 0,
		eVolumeType_Kill,
		eVolumeType_GarbageCollection
	};

	struct ForgeVolume
	{
		VolumeType Type;
		int8_t TeamIndex;
		uint16_t Flags;
		uint32_t ObjectIndex;
		ZoneShape Zone;
	};

	struct DamageData
	{
		int DamageEffect;
		int Flags;
		int PlayerIndex;
		int ObjectIndex;
		int field_10;
		int field_14;
		int field_18;
		int field_1C;
		__int16 ClusterIndex;
		__int16 field_22;
		Blam::Math::RealVector3D ObjectPosition;
		Blam::Math::RealVector3D EpicenterDirection;
		Blam::Math::RealVector3D Direction;
		int Direction2;
		int field_4C;
		int field_50;
		int field_54;
		float field_58;
		float field_5C;
		float field_60;
		float field_64;
		float field_68;
		int field_6C;
		int field_70;
		int field_74;
		int field_78;
		int field_7C;
		int field_80;
		__int16 MaterialIndex;
		__int16 field_86;
		int field_88;
		int field_8C;
		int field_90;
		int field_94;
	};

	void FindVolumes();
	void RenderVolumes();
	void UpdateKillVolume(ForgeVolume &volume);
	void UpdateGarbageCollectionVolume(ForgeVolume &volume);

	int s_LastActiveVolumeScan = 0;
	int s_ActiveVolumeCount = 0;
	ForgeVolume s_Volumes[MAX_VOLUMES];
	std::unordered_map<uint32_t, int32_t> s_GarbageCollectionTimes;
}

namespace Forge::Volumes
{
	void Update()
	{
		const auto game_get_current_engine = (void*(*)())(0x005CE150);

		if (game_get_current_engine())
		{
			if (Blam::Time::TicksToSeconds(float(Blam::Time::GetGameTicks() - s_LastActiveVolumeScan)) > SCAN_INTERVAL)
			{
				FindVolumes();
				s_LastActiveVolumeScan = Blam::Time::GetGameTicks();
			}

			for (auto i = 0; i < s_ActiveVolumeCount; i++)
			{
				auto &volume = s_Volumes[i];
				switch (volume.Type)
				{
				case eVolumeType_Kill:
					UpdateKillVolume(volume);
					break;
				case eVolumeType_GarbageCollection:
					UpdateGarbageCollectionVolume(volume);
					break;
				}
			}

			RenderVolumes();
		}
		else
		{
			if (!s_GarbageCollectionTimes.empty())
				s_GarbageCollectionTimes.clear();
			s_LastActiveVolumeScan = 0;
		}
	}
}

namespace
{
	void ApplyUnitDamage(ForgeVolume &killVolume, uint32_t unitObjectIndex)
	{
		const auto InitDamageData = (int(*)(DamageData *damageData, int damageEffectTagIndex))(0x00B50330);
		const auto ApplyDamage = (void(*)(DamageData *a1, int unitObjectIndex, int a3))(0xB542A0);

		if (unitObjectIndex == -1)
			return;

		DamageData damage;
		InitDamageData(&damage, DAMAGE_EFFECT_TAG_INDEX);
		damage.field_8C = DAMAGE_CAUSE_TYPE;
		damage.Flags |= 4u;
		ApplyDamage(&damage, unitObjectIndex, 5);
	}


	void UpdateKillVolume(ForgeVolume &volume)
	{
		const auto ZoneShape__ContainsPlayer = (bool(__thiscall *)(void *thisptr, int playerIndex))(0x00765C80);
		auto players = Blam::Players::GetPlayers();

		auto objects = Blam::Objects::GetObjects();

		uint8_t playerIterator[0x10];
		const auto c_player_iterator__ctor = (void(__thiscall *)(void *thisptr))(0x00537400);
		const auto c_player_iterator__next_with_unit = (bool(__thiscall *)(void *thisptr))(0x00537950);
		c_player_iterator__ctor(playerIterator);

		while (c_player_iterator__next_with_unit(playerIterator))
		{
			auto playerIndex = *(uint32_t*)((uint8_t*)playerIterator + 0x8);
			auto datum = *(Blam::Players::PlayerDatum**)playerIterator;
			if (ZoneShape__ContainsPlayer(&volume.Zone, playerIndex))
			{
				if (volume.TeamIndex == datum->Properties.TeamIndex || volume.TeamIndex == 8)
					ApplyUnitDamage(volume, datum->SlaveUnit);
			}
		}
	}

	const auto multiplayer_globals_get_grenade_index = (int(*)(int tagIndex))(0x0052D1A0);
	const auto weapons_get_multiplayer_weapon_type = (int16_t(*)(uint32_t objectIndex))(0x00B62DB0);


	bool ShouldGarbageCollectObject(uint32_t objectIndex, ForgeVolume &volume)
	{
		auto volumeObject = Blam::Objects::Get(volume.ObjectIndex);
		if (!volumeObject)
			return false;

		auto garbageVolumeProperties = (Forge::ForgeGarbageVolumeProperties*)(&volumeObject->GetMultiplayerProperties()->TeleporterChannel);

		auto object = Blam::Objects::Get(objectIndex);
		auto objectType = *((uint8_t*)object + 0x9A);

		if (objectType == Blam::Objects::eObjectTypeVehicle)
		{
			if (!(garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectVehicles))
				return false;

			auto driverObjectIndex = *(uint32_t*)((uint8_t*)object + 0x32C);
			return driverObjectIndex == -1;
		}
		if (objectType == Blam::Objects::eObjectTypeWeapon)
		{
			auto mpWeaponType = weapons_get_multiplayer_weapon_type(objectIndex);

			auto ownerUnitObjectIndex = *(uint32_t*)((uint8_t*)object + 0x184);
			if (ownerUnitObjectIndex != -1)
				return false;

			if (mpWeaponType > 0)
			{
				return garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectObjectives;
			}
			else
			{
				return garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectWeapons;
			}
		}
		else if (objectType == Blam::Objects::eObjectTypeBiped)
		{
			if (!(garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectDeadBipeds))
				return false;

			for (auto player : Blam::Players::GetPlayers())
				if (player.SlaveUnit == Blam::DatumIndex(objectIndex))
					return false;
			return true;
		}
		else if (objectType == Blam::Objects::eObjectTypeEquipment)
		{
			auto isGrenade = multiplayer_globals_get_grenade_index(object->TagIndex) != -1;
			if (isGrenade)
			{
				if (!(garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectGrenades))
					return false;

				if (!*((uint8_t *)object + 0x179))
					return true;
			}
			else
			{
				if (!(garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectEquipment))
					return false;
				auto ownerUnitObjectIndex = *(uint32_t*)((uint8_t*)object + 0x184);
				return ownerUnitObjectIndex == -1;
			}
		}

		return false;
	}

	void UpdateGarbageCollectionVolume(ForgeVolume &volume)
	{
		const auto objects_get_in_cluster = (int16_t(*)(int a1, int objectTypeMask, int16_t *pClusterIndex,
			Blam::Math::RealVector3D *center, float radius, uint32_t *clusterObjects, int16_t maxObjects))(0x00B35B60);
		const auto objects_dispose = (void(*)(uint32_t objectIndex))(0x00B2CD10);
		const auto zone_intersect_point = (bool(*)(Blam::Math::RealVector3D *point, ZoneShape *zone))(0x00BA11F0);

		auto volumeObject = Blam::Objects::Get(volume.ObjectIndex);
		if (!volumeObject)
			return;

		auto garbageVolumeProperties = (Forge::ForgeGarbageVolumeProperties*)(&volumeObject->GetMultiplayerProperties()->TeleporterChannel);

		int collectionIntervals[] = { 0, 3, 15, 30 };
		auto interval = collectionIntervals[garbageVolumeProperties->Interval];

		if (Blam::Time::TicksToSeconds(float(Blam::Time::GetGameTicks() - s_GarbageCollectionTimes[volume.ObjectIndex])) < float(interval))
			return;

		s_GarbageCollectionTimes[volume.ObjectIndex] = Blam::Time::GetGameTicks();

		const auto collectionMask = (1 << Blam::Objects::eObjectTypeWeapon)
			| (1 << Blam::Objects::eObjectTypeVehicle)
			| (1 << Blam::Objects::eObjectTypeEquipment)
			| (1 << Blam::Objects::eObjectTypeBiped);

		uint32_t clusterObjects[128];

		auto clusterObjectCount = objects_get_in_cluster(0, collectionMask, &volumeObject->ClusterIndex,
			&volumeObject->Center, std::abs(volume.Zone.BoundingRadius), clusterObjects, 128);

		for (auto i = 0; i < clusterObjectCount; i++)
		{

			auto clusterObjectIndex = clusterObjects[i];
			auto clusterObject = Blam::Objects::Get(clusterObjectIndex);

			if (!zone_intersect_point(&clusterObject->Center, &volume.Zone))
				continue;

			if (ShouldGarbageCollectObject(clusterObjectIndex, volume))
				objects_dispose(clusterObjectIndex);
		}
	}

	void FindVolumes()
	{
		s_ActiveVolumeCount = 0;
		auto objects = Blam::Objects::GetObjects();
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->Type != Blam::Objects::eObjectTypeCrate || !it->Data)
				continue;

			auto mpProperties = it->Data->GetMultiplayerProperties();
			if (!mpProperties)
				continue;

			switch (it->Data->TagIndex)
			{
			case Forge::Volumes::KILL_VOLUME_TAG_INDEX:
			case Forge::Volumes::GARBAGE_VOLUME_TAG_INDEX:
				break;
			default:
				continue;
			}

			auto &volume = s_Volumes[s_ActiveVolumeCount++];
			GetObjectZoneShape(it.CurrentDatumIndex, &volume.Zone, 0);
			volume.ObjectIndex = it.CurrentDatumIndex;
			volume.TeamIndex = mpProperties->TeamIndex & 0xff;

			switch (it->Data->TagIndex)
			{
			case Forge::Volumes::KILL_VOLUME_TAG_INDEX:
				volume.Type = eVolumeType_Kill;
				break;
			case Forge::Volumes::GARBAGE_VOLUME_TAG_INDEX:
				volume.Type = eVolumeType_GarbageCollection;
				if (s_GarbageCollectionTimes.find(volume.ObjectIndex) == s_GarbageCollectionTimes.end())
					s_GarbageCollectionTimes[volume.ObjectIndex] = Blam::Time::GetGameTicks();
				break;
			}

			if (s_ActiveVolumeCount >= MAX_VOLUMES)
				break;
		}
	}

	inline bool SphereInFrustrum(RealVector3D &center, float radius)
	{
		const auto frustrum_sphere_intersect = (int(*)(void *frustrum, RealVector3D *center, float radius))(0x0075A860);
		const auto viewport = (uint8_t**)(0x24E0094);
		if (!viewport)
			return false;
		
		auto v = frustrum_sphere_intersect(*viewport + 0x94, &center, radius);
		return v > 0;
	}

	void RenderVolumes()
	{
		const auto zone_render = (void(*)(const ZoneShape *shape, float *color, uint32_t objectIndex))(0x00BA0FC0);
		

		if (!Forge::GetEditorModeState(Blam::Players::GetLocalPlayer(0), nullptr, nullptr)
			&& !Modules::ModuleForge::Instance().VarShowInvisibles->ValueInt)
			return;

		for (auto i = 0; i < s_ActiveVolumeCount; i++)
		{
			const auto &volume = s_Volumes[i];
			if (volume.Type == eVolumeType_Disabled)
				continue;

			Blam::Objects::ObjectBase *volumeObject;
			if (volume.ObjectIndex == -1 || !(volumeObject = Blam::Objects::Get(volume.ObjectIndex)))
				return;

			if (!SphereInFrustrum(volumeObject->Center, volume.Zone.BoundingRadius))
				continue;

			ZoneShape zone;
			GetObjectZoneShape(volume.ObjectIndex, &zone, 0);

			switch (volume.Type)
			{
			case eVolumeType_GarbageCollection:
			{
				float color[] = { 0.1f, 0, 0.5f, 0 };
				zone_render(&zone, color, volume.ObjectIndex);
			}
			break;
			case eVolumeType_Kill:
			{
				float color[] = { 0.1f, 0.5f, 0, 0 };
				zone_render(&zone, color, volume.ObjectIndex);
			}
			break;
			}
		}
	}
}
