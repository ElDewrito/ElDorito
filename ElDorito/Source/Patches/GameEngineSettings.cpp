#include "GameEngineSettings.hpp"
#include "../Blam/BitStream.hpp"
#include "../Patch.hpp"

namespace
{
	struct c_player_health_traits
	{
		uint8_t DamageResistance;
		uint8_t ShieldRechargeRate;
		uint8_t ShieldMultiplier;
		uint8_t HeadshotImmunity;
		uint8_t ShieldVampirism;
		uint8_t field_5;
		uint8_t field_6;
		uint8_t field_7;

		void Serialize(Blam::BitStream &stream)
		{
			stream.WriteUnsigned(DamageResistance, 4);
			stream.WriteUnsigned(ShieldRechargeRate, 4);
			stream.WriteUnsigned(ShieldMultiplier, 3);
			stream.WriteUnsigned(HeadshotImmunity, 2);
			stream.WriteUnsigned(ShieldVampirism, 3);
		}

		void Deserialize(Blam::BitStream &stream)
		{
			DamageResistance = stream.ReadUnsigned<uint8_t>(4);
			if (DamageResistance > 15)
				DamageResistance = 0;
			ShieldRechargeRate = stream.ReadUnsigned<uint8_t>(4);
			if (ShieldRechargeRate > 9)
				ShieldRechargeRate = 0;
			ShieldMultiplier = stream.ReadUnsigned<uint8_t>(3);
			if (ShieldMultiplier > 5)
				ShieldMultiplier = 0;
			HeadshotImmunity = stream.ReadUnsigned<uint8_t>(2);
			if (HeadshotImmunity > 2)
				HeadshotImmunity = 0;
			ShieldVampirism = stream.ReadUnsigned<uint8_t>(3);
			if (ShieldVampirism > 5)
				ShieldVampirism = 0;
		}
	};
	static_assert(sizeof(c_player_health_traits) == 0x8, "c_player_health_traits invalid");

	struct c_player_weapon_traits
	{
		uint16_t field_0;
		uint8_t PrimaryWeaponIndex;
		uint8_t SecondaryWeaponIndex;
		uint8_t DamageModifier;
		uint8_t GrenadeRegeeration;
		uint8_t InfiniteAmmo;
		uint8_t WeaponPickup;

		void Serialize(Blam::BitStream &stream)
		{
			stream.WriteUnsigned(DamageModifier, 4);
			stream.WriteUnsigned(PrimaryWeaponIndex, 8);
			stream.WriteUnsigned(SecondaryWeaponIndex, 8);
			stream.WriteUnsigned(field_0, 2);
			stream.WriteUnsigned(InfiniteAmmo, 3);
			stream.WriteUnsigned(GrenadeRegeeration, 2);
			stream.WriteUnsigned(WeaponPickup, 2);
		}

		void Deserialize(Blam::BitStream &stream)
		{
			DamageModifier = stream.ReadUnsigned<uint8_t>(4);
			if (DamageModifier > 12)
				DamageModifier = 0;
			PrimaryWeaponIndex = ValidateWeaponIndex(stream.ReadUnsigned<uint8_t>(8));
			SecondaryWeaponIndex = ValidateWeaponIndex(stream.ReadUnsigned<uint8_t>(8));
			field_0 = stream.ReadUnsigned<uint8_t>(2);
			if (field_0 > 2)
				field_0 = 0;
			InfiniteAmmo = stream.ReadUnsigned<uint8_t>(3);
			if (InfiniteAmmo > 3)
				InfiniteAmmo = 0;
			GrenadeRegeeration = stream.ReadUnsigned<uint8_t>(2);
			if (GrenadeRegeeration > 2)
				GrenadeRegeeration = 0;
			WeaponPickup = stream.ReadUnsigned<uint8_t>(2);
			if (WeaponPickup > 2)
				WeaponPickup = 0;
		}

		uint8_t ValidateWeaponIndex(uint8_t index)
		{
			auto v5 = index;
			auto v6 = v5 < 0;
			if (v5 < 0)
				v6 = v5 < 0;
			auto v7 = v5 == -1 || v5 == -3 || v5 == -2;
			auto v8 = (!v6 || v7) == 0;
			auto v9 = -2;
			auto v10 = -2;
			if (!v8)
				v10 = v5;
			return v10;
		}
	};
	static_assert(sizeof(c_player_health_traits) == 0x8, "c_player_weapon_traits invalid");

	struct c_player_movement_traits
	{
		uint8_t PlayerSpeed;
		uint8_t PlayerGravity;
		uint8_t VehicleUse;
		uint8_t field_3;

		void Serialize(Blam::BitStream &stream)
		{
			stream.WriteUnsigned(PlayerSpeed, 4);
			stream.WriteUnsigned(PlayerGravity, 4);
			stream.WriteUnsigned(VehicleUse, 2);
		}

		void Deserialize(Blam::BitStream &stream)
		{
			PlayerSpeed = stream.ReadUnsigned<uint8_t>(4);
			if (PlayerSpeed > 15)
				PlayerSpeed = 0;
			PlayerGravity = stream.ReadUnsigned<uint8_t>(4);
			if (PlayerGravity > 10)
				PlayerGravity = 0;
			VehicleUse = stream.ReadUnsigned<uint8_t>(2);
			if (VehicleUse > 3)
				VehicleUse = 0;
		}
	};
	static_assert(sizeof(c_player_movement_traits) == 0x4, "c_player_movement_traits invalid");

	struct c_player_appearance_traits
	{
		uint8_t ActiveCamo;
		uint8_t Waypoint;
		uint8_t Aura;  // now player size
		uint8_t ForcedColor;

		void Serialize(Blam::BitStream &stream)
		{
			stream.WriteUnsigned(ActiveCamo, 3);
			stream.WriteUnsigned(Waypoint, 3);
			stream.WriteUnsigned(Aura, 5);
			stream.WriteUnsigned(ForcedColor, 4);
		}

		void Deserialize(Blam::BitStream &stream)
		{
			ActiveCamo = stream.ReadUnsigned<uint8_t>(3);
			if (ActiveCamo > 4)
				ActiveCamo = 0;
			Waypoint = stream.ReadUnsigned<uint8_t>(3);
			if (Waypoint > 5)
				Waypoint = 0;
			Aura = stream.ReadUnsigned<uint8_t>(5);
			if (Aura > 18)
				Aura = 0;
			ForcedColor = stream.ReadUnsigned<uint8_t>(4);
			if (ForcedColor > 13)
				ForcedColor = 0;
		}
	};
	static_assert(sizeof(c_player_appearance_traits) == 0x4, "c_player_appearance_traits invalid");

	struct c_player_sensor_traits
	{
		uint16_t MotionTrackerMode;
		uint16_t MotionTrackerRange;

		void Serialize(Blam::BitStream &stream)
		{
			stream.WriteUnsigned(MotionTrackerMode, 3);
			stream.WriteUnsigned(MotionTrackerRange, 3);
		}

		void Deserialize(Blam::BitStream &stream)
		{
			MotionTrackerMode = stream.ReadUnsigned<uint8_t>(3);
			if (MotionTrackerMode > 4)
				MotionTrackerMode = 0;
			MotionTrackerRange = stream.ReadUnsigned<uint8_t>(3);
			if (MotionTrackerRange > 7)
				MotionTrackerRange = 0;
		}
	};
	static_assert(sizeof(c_player_sensor_traits) == 0x4, "c_player_sensor_traits invalid");

	struct c_player_trait_profile
	{
		c_player_health_traits Health;
		c_player_weapon_traits Weapon;
		c_player_movement_traits Movement;
		c_player_appearance_traits Appearance;
		c_player_sensor_traits Sensory;

		void Serialize(Blam::BitStream &stream)
		{
			Health.Serialize(stream);
			Weapon.Serialize(stream);
			Movement.Serialize(stream);
			Appearance.Serialize(stream);
			Sensory.Serialize(stream);
		}

		void Deserialize(Blam::BitStream &stream)
		{
			Health.Deserialize(stream);
			Weapon.Deserialize(stream);
			Movement.Deserialize(stream);
			Appearance.Deserialize(stream);
			Sensory.Deserialize(stream);
		}
	};
	static_assert(sizeof(c_player_trait_profile) == 0x1C, "c_player_trait_profile invalid");

	float __fastcall c_player_health_traits__get_damage_resistance_hook(uint8_t *thisptr, void *unused);
	float __fastcall c_player_movement_traits__get_walking_speed_hook(uint8_t *thisptr, void *unused);
	float __fastcall c_player_movement_traits__get_personal_gravity_hook(c_player_movement_traits *thisptr, void *unused);

	void __fastcall c_player_trait_profile__serialize_hook(c_player_trait_profile &profile, void *unused, Blam::BitStream &stream) { profile.Serialize(stream); }
	void __fastcall c_player_trait_profile__deserialize_hook(c_player_trait_profile &profile, void *unused, Blam::BitStream &stream) { profile.Deserialize(stream); }
	void __fastcall c_player_health_traits__serialize_hook(c_player_health_traits &traits, void *unused, Blam::BitStream &stream) { traits.Serialize(stream); }
	void __fastcall c_player_weapon_traits__serialize_hook(c_player_weapon_traits &traits, void *unused, Blam::BitStream &stream) { traits.Serialize(stream); }
	void __fastcall c_player_movement_traits__serialize_hook(c_player_movement_traits &traits, void *unused, Blam::BitStream &stream) { traits.Serialize(stream); }
	void __fastcall c_player_appearance_traits__serialize_hook(c_player_appearance_traits &traits, void *unused, Blam::BitStream &stream) { traits.Serialize(stream); }
	void __fastcall c_player_sensor_traits__serialize_hook(c_player_sensor_traits &traits, void *unused, Blam::BitStream &stream) { traits.Serialize(stream); }
	void __fastcall c_player_health_traits__deserialize_hook(c_player_health_traits &traits, void *unused, Blam::BitStream &stream) { traits.Deserialize(stream); }
	void __fastcall c_player_weapon_traits__deserialize_hook(c_player_weapon_traits &traits, void *unused, Blam::BitStream &stream) { traits.Deserialize(stream); }
	void __fastcall c_player_movement_traits__deserialize_hook(c_player_movement_traits &traits, void *unused, Blam::BitStream &stream) { traits.Deserialize(stream); }
	void __fastcall c_player_appearance_traits__deserialize_hook(c_player_appearance_traits &traits, void *unused, Blam::BitStream &stream) { traits.Deserialize(stream); }
	void __fastcall c_player_sensor_traits__deserialize_hook(c_player_sensor_traits &traits, void *unused, Blam::BitStream &stream) { traits.Deserialize(stream); }

	void AuraTraitDefaultFixHook();
}

namespace Patches::GameEngineSettings
{
	void ApplyAll()
	{
		// bottomless clip
		Patch(0x1CB9A1 + 2, { 3 }).Apply();
		Patch(0x1CBF32 + 2, { 3 }).Apply();
		Patch(0x1CE01A + 2, { 3 }).Apply();
		Patch(0x1CD586 + 2, { 3 }).Apply();
		Patch(0x1CCBED + 2, { 3 }).Apply();

		// waypoint
		Patch(0x1CB59A + 2, { 5 }).Apply();
		Patch(0x1CDEF6 + 2, { 5 }).Apply();
		Patch(0x1CBA15 + 2, { 5 }).Apply();
		Patch(0x1CC699 + 2, { 5 }).Apply();
		Patch(0x1CCE53 + 2, { 5 }).Apply();

		// immune to assasinations
		Patch(0x1CB8CE + 2, { 15 }).Apply();
		Patch(0x1CDB74 + 2, { 15 }).Apply();
		Patch(0x1CD2C6 + 2, { 15 }).Apply();
		Patch(0x1CC94D + 2, { 15 }).Apply();
		Patch(0x1CBD6E + 2, { 15 }).Apply();
		Hook(0x1CC110, c_player_health_traits__get_damage_resistance_hook).Apply();

		//walking speed
		Patch(0x1CB9D1 + 2, { 15 }).Apply();
		Patch(0x1CD7B2 + 2, { 15 }).Apply();
		Patch(0x1CDD86 + 2, { 15 }).Apply();
		Patch(0x1CC77D + 2, { 15 }).Apply();
		Patch(0x1CB731 + 2, { 15 }).Apply();
		Hook(0x1CC480, c_player_movement_traits__get_walking_speed_hook).Apply();
		// gravity
		Patch(0x1CB9E1 + 2, { 10 }).Apply();
		Patch(0x1CD7C5 + 2, { 10 }).Apply();
		Patch(0x1CD4C6 + 2, { 10 }).Apply();
		Patch(0x1CC799 + 2, { 10 }).Apply();
		Patch(0x1CB765 + 2, { 10 }).Apply();
		Hook(0x1CC200, c_player_movement_traits__get_personal_gravity_hook).Apply();

		// player size
		Patch(0x1CB5B9 + 2, { 18 }).Apply();
		Patch(0x1CBA25 + 2, { 18 }).Apply();
		Patch(0x1CCEB6 + 2, { 18 }).Apply();
		Patch(0x1CC6B7 + 2, { 18 }).Apply();
		Patch(0x1CCE66 + 2, { 18 }).Apply();
		Patch(0x14F3B6, 0x90, 6).Apply();
		Patch(0x14F3BD, 0x90, 2).Apply();
		// disable appearance trait caching
		Patch(0x349781, { 0x90, 0x90 }).Apply();
		Patch(0x34978E, { 0xEB }).Apply();

		// serialization
		Hook(0x1CB4F0, c_player_trait_profile__serialize_hook).Apply();
		Hook(0x1CAEB0, c_player_trait_profile__deserialize_hook).Apply();
		Hook(0x1CAD20, c_player_health_traits__deserialize_hook).Apply();
		Hook(0x1CADB0, c_player_weapon_traits__deserialize_hook).Apply();
		Hook(0x1CAC80, c_player_movement_traits__deserialize_hook).Apply();
		Hook(0x1CAC10, c_player_appearance_traits__deserialize_hook).Apply();
		Hook(0x1CACE0, c_player_sensor_traits__deserialize_hook).Apply();
		Hook(0x1CB1D0, c_player_health_traits__serialize_hook).Apply();
		Hook(0x1CB320, c_player_weapon_traits__serialize_hook).Apply();
		Hook(0x1CB060, c_player_movement_traits__serialize_hook).Apply();
		Hook(0x1CAF50, c_player_appearance_traits__serialize_hook).Apply();
		Hook(0x1CB130, c_player_sensor_traits__serialize_hook).Apply();

		Hook(0x174D5D, AuraTraitDefaultFixHook).Apply();
	}
}

namespace
{
	float __fastcall c_player_health_traits__get_damage_resistance_hook(uint8_t* thisptr, void *unused)
	{
		switch (*(uint8_t*)thisptr)
		{
		case 1: return 0.1f;
		case 2: return 0.5f;
		case 3: return 0.89999998f;
		case 5: return 1.1f;
		case 6: return 1.5f;
		case 7: return 2.0f;
		case 8: return 3.0f;
		case 9: return 5.0f;
		case 10: return 10.0f;
		case 11: return 20.0f;
		case 12:
		case 15: // invincible
			return  1000.0;
		case 13: return 15;
		case 14: return 30;
		}
		return 1.0f;
	}

	float __fastcall c_player_movement_traits__get_walking_speed_hook(uint8_t* thisptr, void *unused)
	{
		float result = 0.0f;

		switch (*thisptr)
		{
		case 1:
			result = 0.25f;
			break;
		case 2:
			result = 0.5f;
			break;
		case 3:
			result = 0.75f;
			break;
		case 4:
			result = 0.89999998f;
			break;
		case 6:
			result = 1.1f;
			break;
		case 7:
			result = 1.25f;
			break;
		case 8:
			result = 1.5f;
			break;
		case 9:
			result = 2.0f;
			break;
		case 10:
			result = 3.0f;
			break;
		default:
			result = 1.0f;
			break;
		}
		return result;
	}

	float __fastcall c_player_movement_traits__get_personal_gravity_hook(c_player_movement_traits* thisptr, void *unused)
	{
		auto result = 0.0f;
		switch (thisptr->PlayerGravity)
		{
		case 1:
			result = 0.5f;
			break;
		case 2:
			result = 0.75f;
			break;
		case 4:
			result = 1.5f;
			break;
		case 5:
			result = 2.0f;
			break;
		default:
			result = 1.0f;
			break;
		case 6:
			result = 0.0f;
			break;
		case 7:
			result = 0.15f;
			break;
		case 8:
			result = 0.25f;
			break;
		case 9:
			result = 0.35f;
			break;
		case 10:
			result = 1.25f;
			break;
		}
		return result;
	}

	__declspec(naked) void AuraTraitDefaultFixHook()
	{
		__asm
		{
			movzx eax, byte ptr[ecx + 0x16E]
			cmp eax, 1
			jnz skip
			xor eax, eax
			skip :
			mov ecx, [ebp + 0x10]
			mov dl, 1
			mov[ecx], eax
			mov al, dl
			pop ebp
			retn 0x0C
		}
	}
}
