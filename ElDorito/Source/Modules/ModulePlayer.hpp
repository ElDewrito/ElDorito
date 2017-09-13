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
		Command* VarColorsHolo;

		Command *VarRepresentation;

		Command* VarRenderWeapon;

		Command* VarPlayerName;
		Command* VarPlayerPrivKey;
		Command* VarPlayerPubKey;

		wchar_t UserName[17];

		ModulePlayer();
	};
}
