#pragma once
#include <string>
#include <vector>

#include "../Modules/ModuleWeapon.hpp"
#include "../Blam/Math/RealVector3D.hpp"

namespace Patches
{
	using namespace Blam::Math;

	namespace Weapon
	{
		void Init();
		void ApplyAfterTagsLoaded();
		uint16_t Get_WeaponIndice(std::string &weaponName);
		RealVector3D Get_WeaponOffset(std::string &weaponName, bool isDefault);
		void Update_WeaponOffsetsModified(std::string &weaponName, RealVector3D &weaponOffset);

		const std::string ConfigPath = "weapons.txt";
	}
}