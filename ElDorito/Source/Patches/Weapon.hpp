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

		namespace Get
		{
			uint16_t Index(std::string &weaponName);
			std::map<std::string, uint16_t> Indices();

			RealVector3D Offset(std::string mapped, std::string &weaponName);
		}

		namespace Set
		{
			void OffsetModified(std::string &weaponName, RealVector3D &weaponOffset);
		}

		namespace Check
		{
			bool OffsetModified(const std::string &weapon);
		}

		namespace Config
		{
			void Load(std::string configPath);
			void Save(std::string configPath);
		}
	}
}