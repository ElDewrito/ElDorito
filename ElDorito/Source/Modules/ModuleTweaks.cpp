#include "ModuleTweaks.hpp"

namespace Modules
{
	ModuleTweaks::ModuleTweaks() : ModuleBase("Tweaks")
	{
		VarIntelBloomPatch = AddVariableInt("IntelBloomPatch", "intelbloompatch", "Fixes excessive bloom when using Intel integrated graphics", eCommandFlagsArchived, 0);
		VarIntelBloomPatch->ValueIntMin = 0;
		VarIntelBloomPatch->ValueIntMax = 1;

		VarAggressiveAudioDiscarding = AddVariableInt("AggressiveAudioDiscarding", "aggressiveaudiodiscarding", "Discard sounds more aggressively to prevent audio latency. This is a workaround, not a fix.", eCommandFlagsArchived, 0);
		VarAggressiveAudioDiscarding->ValueIntMin = 0;
		VarAggressiveAudioDiscarding->ValueIntMax = 1;;

		VarDisableReactorFog = AddVariableInt("DisableReactorFog", "disablereactorfog", "Disable the fog effects around the moving platforms on the Reactor map to increase performance.", eCommandFlagsArchived, 0);
		VarDisableReactorFog->ValueIntMin = 0;
		VarDisableReactorFog->ValueIntMax = 1;;

		VarDisableWeaponOutline = AddVariableInt("DisableWeaponOutline", "disableweaponoutline", "Disables the outline effect on weapons and equipment.", eCommandFlagsArchived, 0);
		VarDisableWeaponOutline->ValueIntMin = 0;
		VarDisableWeaponOutline->ValueIntMax = 1;;

		VarDisableHeadshotEffect = AddVariableInt("DisableHeadshotEffect", "disableheadshoteffect", "Disables the *ping* effect and sound on headshots.", eCommandFlagsArchived, 0);
		VarDisableHeadshotEffect->ValueIntMin = 0;
		VarDisableHeadshotEffect->ValueIntMax = 1;;

		VarDisableHitMarkers = AddVariableInt("DisableHitMarkers", "disablehitmarkers", "Disables weapon hitmarkers.", eCommandFlagsArchived, 0);
		VarDisableHitMarkers->ValueIntMin = 0;
		VarDisableHitMarkers->ValueIntMax = 1;;

		VarGruntBirthdayParty = AddVariableInt("GruntBirthdayParty", "gruntbirthdayparty", "Hooray! Tweaks.DisableHeadshotEffect will disable this.", eCommandFlagsArchived, 0);
		VarGruntBirthdayParty->ValueIntMin = 0;
		VarGruntBirthdayParty->ValueIntMax = 1;;

		VarReachStyleFrags = AddVariableInt("ReachStyleFrags", "reachstylefrags", "Adds a Halo: Reach style trail effect to frag grenades.", eCommandFlagsArchived, 0);
		VarReachStyleFrags->ValueIntMin = 0;
		VarReachStyleFrags->ValueIntMax = 1;;

		VarMemoryExpansion = AddVariableInt("MemoryExpansion", "memoryexpansion", "Number of bytes to expand the global memory map by. Applies on startup.", eCommandFlagsHidden, 104857600);
	}
}
