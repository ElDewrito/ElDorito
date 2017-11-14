#include "Tweaks.hpp"

#include <vector>

#include "../Blam/Tags/TagInstance.hpp"

#include "../Blam/Tags/Camera/CameraFxSettings.hpp"

#include "../Blam/Tags/Game/Globals.hpp"
#include "../Blam/Tags/Game/MultiplayerGlobals.hpp"

#include "../Blam/Tags/Globals/CacheFileGlobalTags.hpp"

#include "../Blam/Tags/Models/Model.hpp"

#include "../Blam/Tags/Objects/Biped.hpp"
#include "../Blam/Tags/Objects/Projectile.hpp"

#include "../Blam/Tags/Sounds/SoundClasses.hpp"

#include "../Blam/Tags/Scenario/Scenario.hpp"

#include "../Blam/Tags/UI/ChudGlobalsDefinition.hpp"
#include "../Blam/Tags/UI/ChudDefinition.hpp"

#include "../Modules/ModuleTweaks.hpp"

#include "../Utils/Logger.hpp"

namespace Patches::Tweaks
{
	void ApplyAfterTagsLoaded()
	{
		using Blam::Tags::TagInstance;
		using Blam::Tags::Models::Model;
		using Blam::Tags::Game::Globals;
		using Blam::Tags::Game::MultiplayerGlobals;
		using Blam::Tags::Objects::Biped;
		using Blam::Tags::Objects::Projectile;
		using Blam::Tags::Scenario::Scenario;
		using Blam::Tags::UI::ChudDefinition;
		using Blam::Tags::UI::ChudGlobalsDefinition;
		using CameraFxSettings = Blam::Tags::Camera::FxSettings;
		using SoundClasses = Blam::Tags::Sounds::Classes;

		auto matgTags = TagInstance::GetInstancesInGroup('matg');

		if (matgTags.size() < 1)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "no 'globals' tags found!");
			return;
		}

		auto *matgDefinition = matgTags[0].GetDefinition<Globals>();

		if (!matgDefinition)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "tag 'globals\\globals.globals' not found!");
			return;
		}

		if (matgDefinition->SoundGlobals.Count < 1)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "no sound globals defined in tag 'globals\\globals.globals'!");
			return;
		}

		auto *snclDefinition = matgDefinition->SoundGlobals[0].SoundClasses.GetDefinition<SoundClasses>();

		if (!snclDefinition)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "tag 'sound\\sound_classes.sound_classes' not found!");
			return;
		}

		if (matgDefinition->InterfaceTags.Count < 1)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "no interface tags defined in tag 'globals\\globals.globals'!");
			return;
		}

		auto *chgdDefinition = matgDefinition->InterfaceTags[0].HudGlobals.GetDefinition<ChudGlobalsDefinition>();

		if (!chgdDefinition)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "tag 'ui\\chud\\globals.chud_globals_definition' not found!");
			return;
		}

		auto scnrTags = TagInstance::GetInstancesInGroup('scnr');

		if (Modules::ModuleTweaks::Instance().VarIntelBloomPatch->ValueInt)
		{
			for (auto &scnrTag : scnrTags)
			{
				auto *scnrDefinition = scnrTag.GetDefinition<Scenario>();

				if (!scnrDefinition)
					continue;

				for (auto &cameraFx : scnrDefinition->CameraFx)
				{
					if (cameraFx.Tag.TagIndex == -1)
						continue;

					auto *cfxsDefinition = cameraFx.Tag.GetDefinition<CameraFxSettings>();

					if (!cfxsDefinition)
						continue;

					cfxsDefinition->Flags = CameraFxSettings::FlagsValue::Bit4;
				}
			}
		}

		if (Modules::ModuleTweaks::Instance().VarAggressiveAudioDiscarding->ValueInt)
		{
			for (int i = 0; i <= 64; i++)
			{
				snclDefinition->Unknown2[i].CacheMissMode = SoundClasses::CacheMissMode::Discard;
				snclDefinition->Unknown2[i].Priority = 0;

				switch (i)
				{
				case 4:
					snclDefinition->Unknown2[i].MaxSoundsPerTag = 32;
					snclDefinition->Unknown2[i].MaxSoundsPerObject = 4;
					break;

				case 23:
					snclDefinition->Unknown2[i].MaxSoundsPerTag = 16;
					snclDefinition->Unknown2[i].MaxSoundsPerObject = 8;
					break;

				default:
					snclDefinition->Unknown2[i].MaxSoundsPerTag = 1;
					snclDefinition->Unknown2[i].MaxSoundsPerObject = 1;
					break;
				}
			}
		}

		if (Modules::ModuleTweaks::Instance().VarDisableReactorFog->ValueInt)
		{
			auto *scnrDefinition = Blam::Tags::Scenario::GetCurrentScenario();

			if (scnrDefinition && scnrDefinition->MapId == 700)
			{
				scnrDefinition->EffectScenery[01].PaletteIndex = -1;
				scnrDefinition->EffectScenery[02].PaletteIndex = -1;
				scnrDefinition->EffectScenery[61].PaletteIndex = -1;
			}
		}

		if (Modules::ModuleTweaks::Instance().VarDisableWeaponOutline->ValueInt)
		{
			if (chgdDefinition->HudGlobals.Count < 1)
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "no hud globals defined in tag 'ui\\chud\\globals.chud_globals_definition'!");
				return;
			}

			for (int c = 30; c < 37; c++)
			{
				chgdDefinition->HudGlobals[0].GlobalDynamicColors[c].Alpha = 0;
				chgdDefinition->HudGlobals[0].GlobalDynamicColors[c].R = 0;
				chgdDefinition->HudGlobals[0].GlobalDynamicColors[c].G = 0;
				chgdDefinition->HudGlobals[0].GlobalDynamicColors[c].B = 0;
			}
		}

		if (matgDefinition->PlayerRepresentation.Count < 1)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "no player representations found in tag 'globals\\globals.globals'!");
			return;
		}

		auto *bipdDefinition = matgDefinition->PlayerRepresentation[0].ThirdPersonUnit.GetDefinition<Biped>();

		if (!bipdDefinition)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "tag 'objects\\characters\\masterchief\\mp_masterchief\\mp_masterchief.biped' not found!");
			return;
		}

		auto *hlmtDefinition = bipdDefinition->Unit.Object.Model.GetDefinition<Model>();

		if (!hlmtDefinition)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "tag 'objects\\characters\\masterchief\\mp_masterchief\\mp_masterchief.model' not found!");
			return;
		}

		auto &transitionEffect = hlmtDefinition->NewDamageInfo[0].DamageSections[1].InstantResponses[0].SecondaryTransitionEffect;

		if (Modules::ModuleTweaks::Instance().VarDisableHeadshotEffect->ValueInt)
		{
			transitionEffect.TagIndex = -1;
		}
		else if (Modules::ModuleTweaks::Instance().VarGruntBirthdayParty->ValueInt)
		{
			transitionEffect.TagIndex = chgdDefinition->BirthdayPartyEffect.TagIndex;
		}

		if (Modules::ModuleTweaks::Instance().VarReachStyleFrags->ValueInt)
		{
			auto fragProjectile = TagInstance::Find('proj', "objects\\weapons\\grenade\\frag_grenade\\frag_grenade");
			auto bombrunProjectile = TagInstance::Find('proj', "objects\\equipment\\bombrun\\projectiles\\bombrun_grenade");

			if (fragProjectile.Index != 0xFFFF && bombrunProjectile.Index != 0xFFFF)
			{
				auto *fragDefinition = fragProjectile.GetDefinition<Projectile>();
				auto *bombrunDefinition = bombrunProjectile.GetDefinition<Projectile>();

				if (fragDefinition->Attachments.Count > 0 && bombrunDefinition->Attachments.Count > 0)
					fragDefinition->Attachments[0].Tag = bombrunDefinition->Attachments[0].Tag;
			}
		}

		if (Modules::ModuleTweaks::Instance().VarDisableHitMarkers->ValueInt)
		{
			if (bipdDefinition->Unit.HudInterfaces.Count < 1)
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "no hud interfaces defined in tag 'objects\\characters\\masterchief\\mp_masterchief\\mp_masterchief.biped'!");
				return;
			}

			auto *chudDefinition = bipdDefinition->Unit.HudInterfaces[0].UnitHudInterface.GetDefinition<ChudDefinition>();

			if (!chudDefinition)
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Debug, Utils::LogLevel::Warning, "tag 'ui\\chud\\spartan.chud_definition' not found!");
				return;
			}

			for (auto &hudWidget : chudDefinition->HudWidgets)
			{
				if (hudWidget.NameStringID == 0x2AAD) // hit_marker
				{
					hudWidget.PlacementData[0].ScaleX = 0;
					hudWidget.PlacementData[0].ScaleY = 0;
					break;
				}
			}
		}
	}
}