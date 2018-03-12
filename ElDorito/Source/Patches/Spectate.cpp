#include "Spectate.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Pointer.hpp"
#include "../Patch.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Web/Ui/WebTimer.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../Utils/String.hpp"
#include "../ElDorito.hpp"
#include "../Modules/ModuleInput.hpp"
#include "../Modules/ModuleSettings.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Blam/BlamObjects.hpp"
#include <cstdint>

namespace
{
	struct SpectateState
	{
		bool IsSpectating;
		Blam::DatumHandle DirectedPlayerIndex;
	}
	s_SpectateState;

	void __fastcall GameDirectorUpdateHook(void* thisptr, void* unused, int a2);
	void __cdecl GetObserverCameraSensitivityHook(int localPlayerIndex, float* sensitivity);
}

namespace Patches::Spectate
{
	void ApplyAll()
	{
		Pointer(0x01671F5C).Write((uint32_t)GameDirectorUpdateHook);
		Hook(0x32A8D6, GetObserverCameraSensitivityHook, HookFlags::IsCall).Apply();
	}
}

namespace
{
	void NotifyPlayerChanged(Blam::DatumHandle playerDatumIndex)
	{
		auto& players = Blam::Players::GetPlayers();
		auto player = players.Get(playerDatumIndex);
		if (!player)
			return;

		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		const auto playerIndex = Pointer(player)(0x24).Read<uint16_t>();
		const auto playerDisplayName = Utils::String::ThinString(player->Properties.DisplayName);
		const auto teamIndex = player->Properties.TeamIndex;

		jsonWriter.StartObject();
		jsonWriter.Key("playerIndex");
		jsonWriter.Uint(playerIndex);
		jsonWriter.Key("teamIndex");
		jsonWriter.Uint(teamIndex);
		jsonWriter.Key("displayName");
		jsonWriter.String(playerDisplayName.c_str());
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("spectate_change", jsonBuffer.GetString(), true);
	}

	void NotifyEnded()
	{
		Web::Ui::ScreenLayer::Notify("spectate_end", "{}", true);
	}

	bool IsSpectating(int localPlayerIndex)
	{
		static auto game_engine_in_state = (bool(*)(char state))(0x005523A0);
		const auto game_engine_round_in_progress = (bool(*)())(0x00550F90);

		if (!game_engine_round_in_progress() || game_engine_in_state(4))
			return false;

		auto playerIndex = Blam::Players::GetLocalPlayer(localPlayerIndex);
		if (playerIndex == Blam::DatumHandle::Null)
			return false;

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (player->SlaveUnit != Blam::DatumHandle::Null)
			return false;

		auto secondsUntilSpawn = Pointer(player)(0x2CBC).Read<int>();

		return player->SlaveUnit == Blam::DatumHandle::Null && secondsUntilSpawn > 1 
			&& !(Pointer(player)(0x4).Read<uint32_t>() & 8u);
	}

	void __fastcall GameDirectorUpdateHook(void* thisptr, void* unused, int a2)
	{
		static auto GameDirectorUpdate = (void(__thiscall*)(void* thisptr, int a2))(0x007219A0);
		
		auto localPlayerIndex = Pointer(thisptr)(0x140).Read<uint32_t>();
		if (IsSpectating(localPlayerIndex))
		{
			if (!s_SpectateState.IsSpectating)
			{
				s_SpectateState.IsSpectating = true;
				s_SpectateState.DirectedPlayerIndex = -1;

				// prevent switching players instantly
				for (int i = Blam::Input::eGameActionNextPlayer; i <= Blam::Input::eGameActionPrevPlayer; i++)
				{
					auto action = Blam::Input::GetActionState(Blam::Input::GameAction(i));
					action->Ticks = 0;
					action->Flags |= Blam::Input::eActionStateFlagsHandled;
				}
			}

			uint32_t playerIndex = -1;
			auto unitObjectIndex = *(uint32_t*)((uint8_t*)thisptr + 0x8);
			auto unitObject = (uint8_t*)Blam::Objects::Get(unitObjectIndex);
			if (unitObject)
			{
				playerIndex = *(uint32_t*)(unitObject + 0x198);
				if(playerIndex == -1)
					playerIndex = *(uint32_t*)(unitObject + 0x19C);
			}

			if (s_SpectateState.DirectedPlayerIndex.Handle != playerIndex)
				NotifyPlayerChanged(playerIndex);
			s_SpectateState.DirectedPlayerIndex = playerIndex;
		}
		else if (s_SpectateState.IsSpectating)
		{
			s_SpectateState.IsSpectating = false;
			s_SpectateState.DirectedPlayerIndex = -1;
			NotifyEnded();
		}

		GameDirectorUpdate(thisptr, a2);
	}

	void __cdecl GetObserverCameraSensitivityHook(int localPlayerIndex, float* sensitivity)
	{
		auto& moduleInput = Modules::ModuleInput::Instance();
		auto& moduleSettings = Modules::ModuleSettings::Instance();
		auto bindings = moduleInput.GetBindings();

		float sens = moduleInput.VarSpectateSensitivity->ValueFloat;

		// the controller defaults are unreasonably sensitive
		const auto isUsingController = *(bool*)0x0244DE98;
		if (isUsingController)
		{
			sens *= 0.015f;
			sensitivity[0] = bindings->ControllerSensitivityX * 0.017453292f * sens;
			sensitivity[1] = bindings->ControllerSensitivityY * 0.017453292f * sens;
		}
		else
		{
			sensitivity[0] = (moduleSettings.VarMouseSensitivityHorizontal->ValueInt / 100.0f * 360.0f) * 0.017453294f * sens;
			sensitivity[1] = (moduleSettings.VarMouseSensitivityVertical->ValueInt / 100.0f * 360.0f) * 0.017453294f * sens;
		}
	}
}