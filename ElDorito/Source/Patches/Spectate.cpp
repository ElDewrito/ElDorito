#include "Spectate.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Pointer.hpp"
#include "../Patch.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../Utils/String.hpp"
#include "../ElDorito.hpp"
#include <cstdint>

namespace
{
	struct SpectateState
	{
		uint32_t Flags = 0;
		Blam::DatumIndex LocalUnitObjectIndex;
		Blam::DatumIndex DirectedPlayerIndex;
	}
	s_SpectateState;

	void __fastcall GameDirectorUpdateHook(void* thisptr, void* unused, int a2);

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
		}

		void Tick()
		{
			auto isMapLoaded = ElDorito::GetMainTls(0x3c)[0](1).Read<uint8_t>();
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
	void NotifyPlayerChanged(Blam::DatumIndex newPlayerIndex)
	{
		auto& players = Blam::Players::GetPlayers();
		auto player = players.Get(newPlayerIndex);
		if (!player)
			return;

		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		const auto playerDisplayName = Utils::String::ThinString(player->Properties.DisplayName);
		const auto teamIndex = player->Properties.TeamIndex;

		jsonWriter.StartObject();
		jsonWriter.Key("playerIndex");
		jsonWriter.Uint(newPlayerIndex);
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
		static auto GameGetEngine = (void*(__cdecl*)())(0x005CE150);

		GameDirectorUpdate(thisptr, a2);

		auto localPlayerIndex = Pointer(thisptr)(0x140).Read<uint32_t>();
		auto directedPlayerIndex = Blam::DatumIndex(Pointer(thisptr)(0x144).Read<uint32_t>());
		auto playerIndex = Blam::Players::GetLocalPlayer(localPlayerIndex);
		auto unitObjectIndex = Blam::DatumIndex(GetLocalPlayerUnitObjectIndex(localPlayerIndex));

		if (GameGetEngine())
		{
			if (s_SpectateState.LocalUnitObjectIndex != unitObjectIndex)
			{
				if (unitObjectIndex == Blam::DatumIndex::Null)
				{
					s_SpectateState.Flags |= 1;
					NotifyPlayerChanged(directedPlayerIndex);
				}
				else
				{
					s_SpectateState.Flags = 0;
					NotifyEnded();
				}
			}

			if (s_SpectateState.Flags & 1)
			{
				if (s_SpectateState.DirectedPlayerIndex != directedPlayerIndex)
					NotifyPlayerChanged(directedPlayerIndex);
			}

			s_SpectateState.LocalUnitObjectIndex = unitObjectIndex;
			s_SpectateState.DirectedPlayerIndex = directedPlayerIndex;
		}
	}
}