#include "ModuleTweaks.hpp"
#include "../Patches/Ui.hpp"

namespace
{
	bool VariableFlatHUDUpdated(const std::vector<std::string>& arguments, std::string& returnInfo)
	{
		Patches::Ui::UpdateHUDDistortion();
		return true;
	}
}

namespace Modules
{
	ModuleTweaks::ModuleTweaks() : ModuleBase("Tweaks")
	{
		VarIntelBloomPatch = AddVariableInt("IntelBloomPatch", "intelbloompatch", "Fixes excessive bloom when using Intel integrated graphics", eCommandFlagsArchived, 0);
		VarIntelBloomPatch->ValueIntMin = 0;
		VarIntelBloomPatch->ValueIntMax = 1;

		VarAggressiveAudioDiscarding = AddVariableInt("AggressiveAudioDiscarding", "aggressiveaudiodiscarding", "Discard sounds more aggressively to prevent audio latency. This is a workaround, not a fix.", eCommandFlagsArchived, 0);
		VarAggressiveAudioDiscarding->ValueIntMin = 0;
		VarAggressiveAudioDiscarding->ValueIntMax = 1;

		VarDisableReactorFog = AddVariableInt("DisableReactorFog", "disablereactorfog", "Disable the fog effects around the moving platforms on the Reactor map to increase performance.", eCommandFlagsArchived, 0);
		VarDisableReactorFog->ValueIntMin = 0;
		VarDisableReactorFog->ValueIntMax = 1;

		VarDisableWeaponOutline = AddVariableInt("DisableWeaponOutline", "disableweaponoutline", "Disables the outline effect on weapons and equipment.", eCommandFlagsArchived, 0);
		VarDisableWeaponOutline->ValueIntMin = 0;
		VarDisableWeaponOutline->ValueIntMax = 1;

		VarDisableHeadshotEffect = AddVariableInt("DisableHeadshotEffect", "disableheadshoteffect", "Disables the *ping* effect and sound on headshots.", eCommandFlagsArchived, 0);
		VarDisableHeadshotEffect->ValueIntMin = 0;
		VarDisableHeadshotEffect->ValueIntMax = 1;

		VarGruntBirthdayParty = AddVariableInt("GruntBirthdayParty", "gruntbirthdayparty", "Hooray! Tweaks.DisableHeadshotEffect will disable this.", eCommandFlagsArchived, 0);
		VarGruntBirthdayParty->ValueIntMin = 0;
		VarGruntBirthdayParty->ValueIntMax = 1;

		VarReachStyleFrags = AddVariableInt("ReachStyleFrags", "reachstylefrags", "Adds a Halo: Reach style trail effect to frag grenades.", eCommandFlagsArchived, 0);
		VarReachStyleFrags->ValueIntMin = 0;
		VarReachStyleFrags->ValueIntMax = 1;

		VarFlatHUD = AddVariableInt("FlatHUD", "flathud", "Removes distortion from the HUD.", eCommandFlagsArchived, 0, VariableFlatHUDUpdated);
		VarFlatHUD->ValueIntMin = 0;
		VarFlatHUD->ValueIntMax = 1;
	}
}
