#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModulePlayer : public Utils::Singleton<ModulePlayer>, public ModuleBase
	{
	public:
		Command* VarArmorAccessory;
		Command* VarArmorArms;
		Command* VarArmorChest;
		Command* VarArmorHelmet;
		Command* VarArmorLegs;
		Command* VarArmorPelvis;
		Command* VarArmorShoulders;

		Command* VarColorsPrimary;
		Command* VarColorsSecondary;
		Command* VarColorsVisor;
		Command* VarColorsLights;
		Command* VarColorsHolo;

		Command* VarRenderWeapon;

		Command* VarPlayerName;
		Command* VarPlayerPrivKey;
		Command* VarPlayerPubKey;

		wchar_t UserName[17];

		ModulePlayer();
	};
}