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
#include "../Blam/BlamTime.hpp"
#include <cstdint>

namespace
{
	struct SpectateState
	{
		uint32_t Flags = 0;
		Blam::DatumIndex DirectedPlayerIndex;
	}
	s_SpectateState;

	void __fastcall GameDirectorUpdateHook(void* thisptr, void* unused, int a2);
	void __cdecl GetObserverCameraSensitivityHook(int localPlayerIndex, float* sensitivity);

	void NotifyEnded();
	void NotifyPlayerChanged(Blam::DatumIndex newPlayerIndex);
}

namespace Patches
{
	namespace Spectate
	{
		void ApplyAll()
		{
			// fix spectating
			Patch::NopFill(Pointer::Base(0x192FFD), 6);

			Pointer(0x01671F5C).Write((uint32_t)GameDirectorUpdateHook);
			Hook(0x32A8D6, GetObserverCameraSensitivityHook, HookFlags::IsCall).Apply();
		}

		void Tick()
		{
			auto gameGlobals = ElDorito::GetMainTls(0x3c)[0];
			if (!gameGlobals)
				return;

			auto isMapLoaded = gameGlobals(1).Read<uint8_t>();
			if (s_SpectateState.Flags & 1 && !isMapLoaded)
			{
				s_SpectateState.Flags = 0;
				NotifyEnded();
			}
		}
	}
}

namespace
{
	void NotifyPlayerChanged(Blam::DatumIndex playerDatumIndex)
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

	void __fastcall GameDirectorUpdateHook(void* thisptr, void* unused, int a2)
	{
		static auto GameDirectorUpdate = (void(__thiscall*)(void* thisptr, int a2))(0x007219A0);
		static auto GetLocalPlayerUnitObjectIndex = (uint32_t(__cdecl*)(int localPlayerIndex))(0x00589CC0);

		GameDirectorUpdate(thisptr, a2);

		auto localPlayerIndex = Pointer(thisptr)(0x140).Read<uint32_t>();
		auto directedPlayerIndex = Blam::DatumIndex(Pointer(thisptr)(0x144).Read<uint32_t>());
		auto playerIndex = Blam::Players::GetLocalPlayer(localPlayerIndex);
		auto unitObjectIndex = Blam::DatumIndex(GetLocalPlayerUnitObjectIndex(localPlayerIndex));

		auto& players = Blam::Players::GetPlayers();
		auto player = players.Get(playerIndex);

		auto engineGlobals = ElDorito::GetMainTls(0x48)[0];

		if (player && engineGlobals)
		{
			auto engineState = engineGlobals(0xE108).Read<uint8_t>();
			auto roundInProgress = engineState == 1;

			if (!s_SpectateState.Flags && unitObjectIndex == Blam::DatumIndex::Null
				&& player->DeadSlaveUnit != Blam::DatumIndex::Null)
			{
				s_SpectateState.Flags |= 1;
				NotifyPlayerChanged(directedPlayerIndex);
			}

			if (s_SpectateState.Flags & 1)
			{
				if (s_SpectateState.DirectedPlayerIndex != directedPlayerIndex)
				{
					NotifyPlayerChanged(directedPlayerIndex);
				}

				if (unitObjectIndex != Blam::DatumIndex::Null || !roundInProgress)
				{
					s_SpectateState.Flags = 0;
					NotifyEnded();
				}
			}
		}

		s_SpectateState.DirectedPlayerIndex = directedPlayerIndex;
	}

	void __cdecl GetObserverCameraSensitivityHook(int localPlayerIndex, float* sensitivity)
	{
		auto& moduleInput = Modules::ModuleInput::Instance();
		auto bindings = moduleInput.GetBindings();

		float sens = moduleInput.VarSpectateSensitivity->ValueFloat;

		// the controller defaults are unreasonably sensitive
		const auto isUsingController = bool(*(uint32_t*)0x0244DE98);
		if (isUsingController)
			sens *= 0.05f;

		sensitivity[0] = bindings->ControllerSensitivityX * 0.017453292f * sens;
		sensitivity[1] = bindings->ControllerSensitivityY * 0.017453292f * sens;
	}
}