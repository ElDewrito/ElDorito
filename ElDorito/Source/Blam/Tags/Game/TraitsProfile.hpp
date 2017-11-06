#pragma once
#include <cstdint>
#include "../Tags.hpp"
#include "../../Text/StringID.hpp"

namespace Blam::Tags::Game
{
	using Blam::Tags::TagBlock;
	using Blam::Text::StringID;

	struct TraitsProfile
	{
		struct ShieldsAndHealth;
		struct WeaponsAndDamage;
		struct Movement;
		struct Appearance;
		struct Sensors;

		StringID Name;
		TagBlock<TraitsProfile::ShieldsAndHealth> ShieldsAndHealth;
		TagBlock<TraitsProfile::WeaponsAndDamage> WeaponsAndDamage;
		TagBlock<TraitsProfile::Movement> Movement;
		TagBlock<TraitsProfile::Appearance> Appearance;
		TagBlock<TraitsProfile::Sensors> Sensors;

		struct TraitsProfile::ShieldsAndHealth
		{
			enum class DamageResistance : int8_t;
			enum class ShieldMultiplier : int8_t;
			enum class ShieldRechargeRate : int8_t;
			enum class HeadshotImmunity : int8_t;
			enum class ShieldVampirism : int8_t;

			TraitsProfile::ShieldsAndHealth::DamageResistance DamageResistance : 8;
			TraitsProfile::ShieldsAndHealth::ShieldMultiplier ShieldMultiplier : 8;
			TraitsProfile::ShieldsAndHealth::ShieldRechargeRate ShieldRechargeRate : 8;
			TraitsProfile::ShieldsAndHealth::HeadshotImmunity HeadshotImmunity : 8;
			TraitsProfile::ShieldsAndHealth::ShieldVampirism ShieldVampirism : 8;
			int8_t Unknown1;
			int8_t Unknown2;
			int8_t Unknown3;

			enum class TraitsProfile::ShieldsAndHealth::DamageResistance : int8_t
			{
				Unchanged,
				_10Percent,
				_50Percent,
				_90Percent,
				_100Percent,
				_110Percent,
				_150Percent,
				_200Percent,
				_300Percent,
				_500Percent,
				_1000Percent,
				_2000Percent,
				Invulnerable
			};

			enum class TraitsProfile::ShieldsAndHealth::ShieldMultiplier : int8_t
			{
				Unchanged,
				_Minus25Percent,
				_Minus10Percent,
				_Minus5Percent,
				_0Percent,
				_50Percent,
				_90Percent,
				_100Percent,
				_110Percent,
				_200Percent
			};

			enum class TraitsProfile::ShieldsAndHealth::ShieldRechargeRate : int8_t
			{
				Unchanged,
				_Minus25Percent,
				_Minus10Percent,
				_Minus5Percent,
				_0Percent,
				_50Percent,
				_90Percent,
				_100Percent,
				_110Percent,
				_200Percent
			};

			enum class TraitsProfile::ShieldsAndHealth::HeadshotImmunity : int8_t
			{
				Unchanged,
				Enabled,
				Disabled
			};

			enum class TraitsProfile::ShieldsAndHealth::ShieldVampirism : int8_t
			{
				Unchanged,
				Disabled,
				_10Percent,
				_25Percent,
				_50Percent,
				_100Percent
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct TraitsProfile::ShieldsAndHealth, 0x8);

		struct TraitsProfile::WeaponsAndDamage
		{
			enum class DamageModifier : int8_t;
			enum class GrenadeRegeneration : int8_t;
			enum class WeaponPickup : int8_t;
			enum class InfiniteAmmo : int8_t;
			enum class GrenadeCount : int16_t;

			TraitsProfile::WeaponsAndDamage::DamageModifier DamageModifier : 8;
			TraitsProfile::WeaponsAndDamage::GrenadeRegeneration GrenadeRegeneration : 8;
			TraitsProfile::WeaponsAndDamage::WeaponPickup WeaponPickup : 8;
			TraitsProfile::WeaponsAndDamage::InfiniteAmmo InfiniteAmmo : 8;
			StringID PrimaryWeapon;
			StringID SecondaryWeapon;
			TraitsProfile::WeaponsAndDamage::GrenadeCount GrenadeCount : 16;
			int8_t Unknown1;
			int8_t Unknown2;

			enum class TraitsProfile::WeaponsAndDamage::DamageModifier : int8_t
			{
				Unchanged,
				_0Percent,
				_25Percent,
				_50Percent,
				_75Percent,
				_90Percent,
				_100Percent,
				_110Percent,
				_125Percent,
				_150Percent,
				_200Percent,
				_300Percent,
				InstantKill
			};

			enum class TraitsProfile::WeaponsAndDamage::GrenadeRegeneration : int8_t
			{
				Unchanged,
				Enabled,
				Disabled
			};

			enum class TraitsProfile::WeaponsAndDamage::WeaponPickup : int8_t
			{
				Unchanged,
				Enabled,
				Disabled
			};

			enum class TraitsProfile::WeaponsAndDamage::InfiniteAmmo : int8_t
			{
				Unchanged,
				Enabled,
				Disabled
			};

			enum class TraitsProfile::WeaponsAndDamage::GrenadeCount : int16_t
			{
				Unchanged,
				MapDefault,
				None
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct TraitsProfile::WeaponsAndDamage, 0x10);

		struct TraitsProfile::Movement
		{
			enum class PlayerSpeed : int8_t;
			enum class PlayerGravity : int8_t;
			enum class VehicleUse : int8_t;

			TraitsProfile::Movement::PlayerSpeed PlayerSpeed : 8;
			TraitsProfile::Movement::PlayerGravity PlayerGravity : 8;
			TraitsProfile::Movement::VehicleUse VehicleUse : 8;
			int8_t Unknown;

			enum class TraitsProfile::Movement::PlayerSpeed : int8_t
			{
				Unchanged,
				_0Percent,
				_25Percent,
				_50Percent,
				_75Percent,
				_90Percent,
				_100Percent,
				_110Percent,
				_125Percent,
				_150Percent,
				_200Percent,
				_300Percent
			};

			enum class TraitsProfile::Movement::PlayerGravity : int8_t
			{
				Unchanged,
				_50Percent,
				_75Percent,
				_100Percent,
				_150Percent,
				_200Percent
			};

			enum class TraitsProfile::Movement::VehicleUse : int8_t
			{
				Unchanged,
				None,
				PassengerOnly,
				FullUse
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct TraitsProfile::Movement, 0x4);

		struct TraitsProfile::Appearance
		{
			enum class ActiveCamo : int8_t;
			enum class Waypoint : int8_t;
			enum class Aura : int8_t;
			enum class ForcedColor : int8_t;

			TraitsProfile::Appearance::ActiveCamo ActiveCamo : 8;
			TraitsProfile::Appearance::Waypoint Waypoint : 8;
			TraitsProfile::Appearance::Aura Aura : 8;
			TraitsProfile::Appearance::ForcedColor ForcedColor : 8;

			enum class TraitsProfile::Appearance::ActiveCamo : int8_t
			{
				Unchanged,
				Disabled,
				BadCamo,
				PoorCamo,
				GoodCamo
			};

			enum class TraitsProfile::Appearance::Waypoint : int8_t
			{
				Unchanged,
				None,
				VisibleToAllies,
				VisibleToEveryone
			};

			enum class TraitsProfile::Appearance::Aura : int8_t
			{
				Unchanged,
				Disabled,
				Team,
				Black,
				White
			};

			enum class TraitsProfile::Appearance::ForcedColor : int8_t
			{
				Unchanged,
				Off,
				Red,
				Blue,
				Green,
				Orange,
				Purple,
				Gold,
				Brown,
				Pink,
				White,
				Black,
				Zombie
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct TraitsProfile::Appearance, 0x4);

		struct TraitsProfile::Sensors
		{
			enum class MotionTrackerMode : int32_t;
			enum class MotionTrackerRange : int32_t;

			TraitsProfile::Sensors::MotionTrackerMode MotionTrackerMode : 32;
			TraitsProfile::Sensors::MotionTrackerRange MotionTrackerRange : 32;

			enum class TraitsProfile::Sensors::MotionTrackerMode : int32_t
			{
				Unchanged,
				Disabled,
				AllyMovement,
				PlayerMovement,
				PlayerLocations
			};

			enum class TraitsProfile::Sensors::MotionTrackerRange : int32_t
			{
				Unchanged,
				_10Meters,
				_15Meters,
				_25Meters,
				_50Meters,
				_75Meters,
				_100Meters,
				_150Meters
			};
		};
		TAG_STRUCT_SIZE_ASSERT(struct TraitsProfile::Sensors, 0x8);
	};
	TAG_STRUCT_SIZE_ASSERT(struct TraitsProfile, 0x40);
}
