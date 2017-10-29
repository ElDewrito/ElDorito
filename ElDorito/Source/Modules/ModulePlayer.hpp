#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModulePlayer : public Utils::Singleton<ModulePlayer>, public ModuleBase
	{
	public:
		Command* VarArmorHelmet;
		Command* VarArmorChest;
		Command* VarArmorRightShoulder;
		Command* VarArmorLeftShoulder;

		Command* VarColorsPrimary;
		Command* VarColorsSecondary;
		Command* VarColorsVisor;
		Command* VarColorsLights;

		Command *VarRepresentation;

		Command* VarPlayerName;
		Command* VarPlayerGender;
		Command* VarPlayerServiceTag;
		Command* VarPlayerPrivKey;
		Command* VarPlayerPubKey;

		wchar_t UserName[17];

		ModulePlayer();

		static bool ValidServiceTag(const std::string &serviceTag);
	};
}
