#include "Medals.hpp"
#include "Events.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Patch.hpp"
#include "../Modules/ModuleGame.hpp"

namespace
{
	void OnEvent(Blam::DatumHandle player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition);
	void __fastcall chud_update_h3_medal_animation_hook(int thisptr, void* unused, int a2);
	void __fastcall chud_update_saber_medal_animation_hook(int thisptr, void* unused, int a2);
	void sound_enqueue_announcer_sound_hook(uint32_t sndTagIndex, int delayTicks);
	void HeadshotHook();
}

namespace Patches::Medals
{
	void ApplyAll()
	{
		Events::OnEvent(OnEvent);
		Patch::NopFill(Pointer::Base(0x697680), 5);
		Patch::NopFill(Pointer::Base(0x697687), 4);
		Hook(0x6970D6, chud_update_h3_medal_animation_hook, HookFlags::IsCall).Apply();
		Hook(0x6970DE, chud_update_saber_medal_animation_hook, HookFlags::IsCall).Apply();
		Hook(0x2E41D1, sound_enqueue_announcer_sound_hook, HookFlags::IsCall).Apply();
		// disable headshot medal for non-sniper kills
		Hook(0x1A1E70, HeadshotHook).Apply();
	}
}

namespace
{
	void OnEvent(Blam::DatumHandle player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)
	{
		if (Modules::ModuleGame::Instance().VarCefMedals->ValueInt)
			return;

		if (definition->MedalStringId && definition->MedalStringId != -1)
		{
			if (player != Blam::Players::GetLocalPlayer(0))
				return;

			const auto chud_show_medal = (void(*)(int playerMappingIndex, int medal))(0x00A95A40);
			chud_show_medal(0, definition->MedalStringId);
		}
	}


	void __fastcall chud_update_saber_medal_animation_hook(int thisptr, void* unused, int a2)
	{

	}

	void __fastcall chud_update_h3_medal_animation_hook(int thisptr, void* unused, int a2)
	{
		struct s_chud_h3_medal_state
		{
			char Active;
			int field_4;
			int StartTime;
			int InEndTime;
			int Duration;
			char InAnimationFinished;
			int OutPercent;
			int field_1C;
			int field_20;
			float Opacity;
		};

		const auto chud_update_h3_medal_animation = (void(__thiscall*)(int thisptr, int a2))(0x00A973F0);

		chud_update_h3_medal_animation(thisptr, a2);

		auto medals = (s_chud_h3_medal_state*)thisptr;

		const auto fadeOutDuration = Blam::Time::SecondsToTicks(1);
		for (auto i = 0; i < 4; i++)
		{
			if (medals[i].Active && medals[i].InAnimationFinished)
			{
				auto dt = Blam::Time::GetGameTicks() - medals[i].StartTime;
				if (dt > 250)
				{
					
					auto fadeOutDelta = dt - 250;
					if (fadeOutDelta > fadeOutDuration)
					{
						medals[i].Active = 0;
					}

					auto &opacity = medals[i].Opacity;
					opacity = 1.0f - fadeOutDelta / float(fadeOutDuration);
					if (opacity < 0.0f)
						opacity = 0.0f;
				}
			}
		}
	}

	void sound_enqueue_announcer_sound_hook(uint32_t sndTagIndex, int delayTicks)
	{
		const auto sound_enqueue_announcer_sound = (void(*)(uint32_t sndTagIndex, int a2))(0x006E4630);

		if (!Modules::ModuleGame::Instance().VarCefMedals->ValueInt)
			sound_enqueue_announcer_sound(sndTagIndex, delayTicks);
	}

	__declspec(naked) void HeadshotHook()
	{
		__asm
		{
			cmp eax, 1
			jnz NO_MEDAL
			mov eax, [ebp + 0x18]
			mov ax, word ptr[eax]
			cmp ax, 0x10
			jz DEFAULT
			cmp ax, 0x11
			jz DEFAULT
			NO_MEDAL:
			push 0x005A1ED5
			retn
			DEFAULT:
			push 0x005A1E75
			retn
		}
	}
}
