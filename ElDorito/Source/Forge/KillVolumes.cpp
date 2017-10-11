#include "KillVolumes.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "ForgeUtil.hpp"

namespace
{
	using namespace Forge;
	using namespace Blam::Math;

	const auto MAX_KILL_VOLUMES = 64;
	const auto SCAN_INTERVAL = 1;
	const auto DAMAGE_EFFECT_TAG_INDEX = 0x000001ED;
	const auto DAMAGE_CAUSE_TYPE = 1; // guardians

	struct ForgeKillVolume
	{
		uint32_t ObjectIndex;
		ZoneShape Zone;
		int16_t TeamIndex;
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

	void FindKillVolumes();
	void CheckAndKillPlayer(uint32_t playerIndex);

	int s_LastActiveVolumeScan = 0;
	int s_ActiveKillVolumeCount = 0;
	ForgeKillVolume s_KillVolumes[MAX_KILL_VOLUMES];
}

namespace Forge::KillVolumes
{
	void Update()
	{
		const auto game_get_current_engine = (void*(*)())(0x005CE150);

		if (game_get_current_engine())
		{
			if (Blam::Time::TicksToSeconds(float(Blam::Time::GetGameTicks() - s_LastActiveVolumeScan)) > SCAN_INTERVAL)
			{
				FindKillVolumes();
				s_LastActiveVolumeScan = Blam::Time::GetGameTicks();
			}

			auto players = Blam::Players::GetPlayers();
			for (auto it = players.begin(); it != players.end(); ++it)
				CheckAndKillPlayer(it.CurrentDatumIndex);
		}
		else
		{
			s_LastActiveVolumeScan = 0;
		}
	}
}

namespace
{
	void ApplyUnitDamage(ForgeKillVolume &killVolume, uint32_t unitObjectIndex)
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

	void CheckAndKillPlayer(uint32_t playerIndex)
	{
		const auto ZoneShape__ContainsPlayer = (bool(__thiscall *)(void *thisptr, int playerIndex))(0x00765C80);

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player)
			return;

		for (auto i = 0; i < s_ActiveKillVolumeCount; i++)
		{
			auto &killVolume = s_KillVolumes[i];
			if (ZoneShape__ContainsPlayer(&killVolume.Zone, playerIndex))
			{
				if (killVolume.TeamIndex == player->Properties.TeamIndex || killVolume.TeamIndex == 8)
					ApplyUnitDamage(killVolume, player->SlaveUnit);
			}
		}
	}

	void FindKillVolumes()
	{
		s_ActiveKillVolumeCount = 0;
		auto objects = Blam::Objects::GetObjects();
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->Type != Blam::Objects::eObjectTypeCrate || !it->Data)
				continue;

			auto mpProperties = it->Data->GetMultiplayerProperties();
			if (!mpProperties)
				continue;

			const auto isTeleporter = mpProperties->ObjectType == 0x9 || mpProperties->ObjectType == 0xA || mpProperties->ObjectType == 0xB;
			if (!isTeleporter || !mpProperties->Shape || mpProperties->Shape > 3)
				continue;

			if (mpProperties->TeleporterChannel == int8_t(0xff))
			{
				auto &volume = s_KillVolumes[s_ActiveKillVolumeCount++];
				volume.TeamIndex = mpProperties->TeamIndex;
				volume.ObjectIndex = it.CurrentDatumIndex;
				GetObjectZoneShape(it.CurrentDatumIndex, &volume.Zone, 0);
			}

			if (s_ActiveKillVolumeCount >= MAX_KILL_VOLUMES)
				break;
		}
	}
}
