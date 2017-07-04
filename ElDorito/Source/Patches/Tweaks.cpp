#include "Tweaks.hpp"
#include <vector>
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Camera/CameraFxSettings.hpp"
#include "../Patch.hpp"
#include "Core.hpp"
#include "../Blam/Tags/Sounds/SoundClasses.hpp"
#include "../Blam/Tags/Scenario/Scenario.hpp"
#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"
#include "../Blam/Tags/UI/ChudGlobalsDefinition.hpp"
#include "../Blam/Tags/UI/ChudDefinition.hpp"
#include "../Blam/Tags/Objects/Projectile.hpp"
#include "../Blam/Tags/Objects/Model.hpp"
#include "../Modules/ModuleTweaks.hpp"

namespace Patches
{
	namespace Tweaks
	{

		void ApplyAfterTagsLoaded()
		{
			using namespace Blam::Tags;

			if (Modules::ModuleTweaks::Instance().VarAggressiveAudioDiscarding->ValueInt)
			{
				auto *sounds = TagInstance(0x019F).GetDefinition<Sounds::Classes>();
				for (int i = 0; i <= 64; i++) {
					sounds->Unknown2[i].CacheMissMode = Sounds::Classes::CacheMissMode::Discard;
					sounds->Unknown2[i].Priority = 0;
					switch (i)
					{
					case 4:
						sounds->Unknown2[i].MaxSoundsPerTag = 32;
						sounds->Unknown2[i].MaxSoundsPerObject = 4;
						break;
					case 23:
						sounds->Unknown2[i].MaxSoundsPerTag = 16;
						sounds->Unknown2[i].MaxSoundsPerObject = 8;
						break;
					default:
						sounds->Unknown2[i].MaxSoundsPerTag = 1;
						sounds->Unknown2[i].MaxSoundsPerObject = 1;
						break;
					}
				}
			}

			if (Modules::ModuleTweaks::Instance().VarDisableReactorFog->ValueInt)
			{
				// Reactor scnr is only loaded when playing on that map
				auto scenario = TagInstance(0x3F83).GetDefinition<Scenario::Scenario>('scnr');
				if (scenario)
				{
					scenario->EffectScenery[01].PaletteIndex = -1;
					scenario->EffectScenery[02].PaletteIndex = -1;
					scenario->EffectScenery[61].PaletteIndex = -1;
				}
			}

			if (Modules::ModuleTweaks::Instance().VarDisableWeaponOutline->ValueInt)
			{
				auto *chgd = TagInstance(0x1BD).GetDefinition<UI::ChudGlobalsDefinition>();
				for (int c = 30; c < 37; c++)
				{
					chgd->HudGlobals[0].GlobalDynamicColors[c].Alpha = 0;
					chgd->HudGlobals[0].GlobalDynamicColors[c].R = 0;
					chgd->HudGlobals[0].GlobalDynamicColors[c].G = 0;
					chgd->HudGlobals[0].GlobalDynamicColors[c].B = 0;
				}
			}

			if (Modules::ModuleTweaks::Instance().VarReachStyleFrags->ValueInt)
			{
				auto *projectile = TagInstance(0x01AD).GetDefinition<Blam::Tags::Objects::Projectile>();
				projectile->Attachments[0].Tag.TagIndex = 0x1B3A;
			}

			auto *model = TagInstance(0x1348).GetDefinition<Blam::Tags::Objects::Model>();
			if (Modules::ModuleTweaks::Instance().VarDisableHeadshotEffect->ValueInt)
				model->NewDamageInfo[0].DamageSections[1].InstantResponses[0].SecondaryTransitionEffect.TagIndex = 0;
			else if (Modules::ModuleTweaks::Instance().VarGruntBirthdayParty->ValueInt)
				model->NewDamageInfo[0].DamageSections[1].InstantResponses[0].SecondaryTransitionEffect.TagIndex = 0x12FE;

			if (Modules::ModuleTweaks::Instance().VarDisableHitMarkers->ValueInt)
			{
				auto *chud = Blam::Tags::TagInstance(0x0C1E).GetDefinition<Blam::Tags::UI::ChudDefinition>();
				for (auto &widget : chud->HudWidgets)
				{
					if (widget.NameStringID == 0x2AAD) // hit_marker
					{
						widget.PlacementData[0].ScaleX = 0;
						widget.PlacementData[0].ScaleY = 0;
						break;
					}
				}
			}
		}
	}
}