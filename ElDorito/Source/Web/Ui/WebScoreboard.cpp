#include "WebScoreboard.hpp"
#include "ScreenLayer.hpp"
#include "../../Blam/BlamNetwork.hpp"
#include "../../Blam/BlamEvents.hpp"
#include "../../Blam/BlamTypes.hpp"
#include "../../Blam/Tags/Objects/Damage.hpp"
#include "../../Patches/Events.hpp"
#include "../../Patches/Scoreboard.hpp"
#include "../../Patches/Input.hpp"
#include "../../Pointer.hpp"
#include "../../Modules/ModuleInput.hpp"
#include "../../Modules/ModuleServer.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../Utils/String.hpp"
#include "../../ElDorito.hpp"

#include <iomanip>
#include "../../Blam/BlamObjects.hpp"
#include "../../Blam/Tags/Items/DefinitionWeapon.hpp"
#include "../../Blam/BlamTime.hpp"

using namespace Blam::Input;
using namespace Blam::Events;

namespace
{
	bool locked = false;
	bool postgame = false;
	bool returningToLobby = false;
	bool acceptsInput = true;
	bool pressedLastTick = false;
	bool spawningSoon = false;
	int lastPressedTime = 0;
	int postgameDisplayed;
	const float postgameDelayTime = 2;

	void OnEvent(Blam::DatumIndex player, const Event *event, const EventDefinition *definition);
	void OnGameInputUpdated();
	void OnScoreUpdate();
}

namespace Web::Ui::WebScoreboard
{
	void Init()
	{
		Patches::Events::OnEvent(OnEvent);
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
		Patches::Scoreboard::OnScoreUpdate(OnScoreUpdate);
	}

	void Show(bool locked, bool postgame)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		jsonWriter.StartObject();
		jsonWriter.Key("locked");
		jsonWriter.Bool(locked);
		jsonWriter.Key("postgame");
		jsonWriter.Bool(postgame);
		jsonWriter.EndObject();

		ScreenLayer::Show("scoreboard", jsonBuffer.GetString());
	}

	void Hide()
	{
		ScreenLayer::Hide("scoreboard");
	}

	void Tick()
	{
		const auto game_engine_round_in_progress = (bool(*)())(0x00550F90);
		const auto is_main_menu = (bool(*)())(0x00531E90);
		const auto is_multiplayer = (bool(*)())(0x00531C00);
		static auto previousMapLoadingState = 0;
		static auto previousEngineState = 0;
		static bool previousHasUnit = false;

		if (!is_multiplayer() && !is_main_menu())
			return;

		if (postgame)
		{
			if (Blam::Time::TicksToSeconds((Blam::Time::GetGameTicks() - postgameDisplayed)) > postgameDelayTime)
			{
				Web::Ui::WebScoreboard::Show(locked, postgame);
				acceptsInput = false;
				postgame = false;
				returningToLobby = true;
			}
		}
		auto isMainMenu = is_main_menu();

		auto currentMapLoadingState = *(bool*)0x023917F0;
		if (previousMapLoadingState && !currentMapLoadingState)
		{
			if (isMainMenu)
			{
				returningToLobby = false;
				acceptsInput = true;
				Web::Ui::WebScoreboard::Hide();
			}
			else
			{
				locked = false;
				postgame = false;
				acceptsInput = false;
				Web::Ui::WebScoreboard::Show(locked, postgame);
			}
		}
		else if (!previousMapLoadingState && currentMapLoadingState && isMainMenu && !returningToLobby)
		{
			locked = false;
			postgame = false;
			Web::Ui::WebScoreboard::Hide();
		}
		previousMapLoadingState = currentMapLoadingState;

		auto engineGlobals = ElDorito::GetMainTls(0x48)[0];
		if (!engineGlobals)
			return;

		auto currentEngineState = engineGlobals(0xE110).Read<uint8_t>();	
		if (previousEngineState & 8 && !(currentEngineState & 8)) // summary ui
		{
			locked = false;
			postgame = false;
			acceptsInput = true;
			Web::Ui::WebScoreboard::Hide();
		}
		previousEngineState = currentEngineState;

		if (game_engine_round_in_progress())
		{
			Blam::Players::PlayerDatum *player{ nullptr };
			auto playerIndex = Blam::Players::GetLocalPlayer(0);
			if (playerIndex == Blam::DatumIndex::Null || !(player = Blam::Players::GetPlayers().Get(playerIndex)))
				return;

			auto secondsUntilSpawn = Pointer(player)(0x2CBC).Read<int>();
			auto firstTimeSpawning = Pointer(player)(0x4).Read<uint32_t>() & 8;

			if (player->SlaveUnit != Blam::DatumIndex::Null)
			{
				spawningSoon = false;

				if (!previousHasUnit)
				{
					acceptsInput = true;
					Web::Ui::WebScoreboard::Hide();
				}
			}
			else
			{
				if (!spawningSoon && !firstTimeSpawning && secondsUntilSpawn > 0 && secondsUntilSpawn < 2)
				{
					spawningSoon = true;
					locked = false;
					postgame = false;
					acceptsInput = false;
					Web::Ui::WebScoreboard::Show(locked, postgame);
				}
			}

			previousHasUnit = player->SlaveUnit != Blam::DatumIndex::Null;
		}
	}

	std::string getScoreboard()
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();

		auto session = Blam::Network::GetActiveSession();
		auto get_multiplayer_scoreboard = (Blam::MutiplayerScoreboard*(*)())(0x00550B80);
		auto* scoreboard = get_multiplayer_scoreboard();
		if (!session || !session->IsEstablished() || !scoreboard)
		{
			writer.EndObject();
			return buffer.GetString();
		}
		
		writer.Key("playersInfo");
		writer.String(Modules::ModuleServer::Instance().VarPlayersInfoClient->ValueString.c_str());

		writer.Key("hasTeams");
		writer.Bool(session->HasTeams());

		auto get_number_of_rounds = (int(*)())(0x005504C0);
		auto get_current_round = (int(*)())(0x00550AD0);

		writer.Key("numberOfRounds");
		writer.Int(get_number_of_rounds());

		writer.Key("currentRound");
		writer.Int(get_current_round());

		
		writer.Key("totalScores");
		writer.StartArray();
		for (int t = 0; t < 8; t++)
		{
			writer.Int(scoreboard->TeamScores[t].TotalScore);
		}
		writer.EndArray();

		writer.Key("teamScores");
		writer.StartArray();
		for (int t = 0; t < 8; t++)
		{
			writer.Int(scoreboard->TeamScores[t].Score);
		}
		writer.EndArray();
		
		
		int32_t variantType = Pointer(0x023DAF18).Read<int32_t>();

		if (variantType >= 0 && variantType < Blam::GameTypeCount)
		{
			writer.Key("gameType");
			writer.String(Blam::GameTypeNames[variantType].c_str());
		}

		auto& playersGlobal = ElDorito::GetMainTls(0x40)[0];
		uint32_t playerStatusBase = 0x2161808;

		writer.Key("players");
		writer.StartArray();
		int playerIdx = session->MembershipInfo.FindFirstPlayer();
		bool teamObjective[10] = { 0 };
		while (playerIdx != -1)
		{
			auto player = session->MembershipInfo.PlayerSessions[playerIdx];
			auto playerStats = Blam::Players::GetStats(playerIdx);

			writer.StartObject();
			// Player information
			writer.Key("name");
			writer.String(Utils::String::ThinString(player.Properties.DisplayName).c_str());
			writer.Key("serviceTag");
			writer.String(Utils::String::ThinString(player.Properties.ServiceTag).c_str());
			writer.Key("team");
			writer.Int(player.Properties.TeamIndex);
			std::stringstream color;
			color << "#" << std::setw(6) << std::setfill('0') << std::hex << player.Properties.Customization.Colors[Blam::Players::ColorIndices::Primary];
			writer.Key("color");
			writer.String(color.str().c_str());
			char uid[17];
			Blam::Players::FormatUid(uid, player.Properties.Uid);
			writer.Key("UID");
			writer.String(uid);
			writer.Key("isHost");
			if (Modules::ModuleServer::Instance().VarServerDedicatedClient->ValueInt == 1)
				writer.Bool(false);
			else {
				writer.Bool(playerIdx == session->MembershipInfo.HostPeerIndex);
			}
			uint8_t alive = Pointer(playerStatusBase + (176 * playerIdx)).Read<uint8_t>();
			writer.Key("isAlive");
			writer.Bool(alive == 1);
			// Generic score information
			writer.Key("kills");
			writer.Int(scoreboard->PlayerScores[playerIdx].Kills);
			writer.Key("assists");
			writer.Int(scoreboard->PlayerScores[playerIdx].Assists);
			writer.Key("deaths");
			writer.Int(scoreboard->PlayerScores[playerIdx].Deaths);
			writer.Key("score");
			writer.Int(scoreboard->PlayerScores[playerIdx].Score);
			writer.Key("totalScore");
			writer.Int(scoreboard->PlayerScores[playerIdx].TotalScore);
			writer.Key("playerIndex");
			writer.Int(playerIdx);
			writer.Key("bestStreak");
			writer.Int(scoreboard->PlayerScores[playerIdx].HighestSpree);

			bool hasObjective = false;
			const auto& playerDatum = Blam::Players::GetPlayers()[playerIdx];
			if (playerDatum.GetSalt() && playerDatum.SlaveUnit != Blam::DatumIndex::Null)
			{
				auto unitObjectPtr = Pointer(Blam::Objects::Get(playerDatum.SlaveUnit));
				if (unitObjectPtr)
				{
					auto rightWeaponIndex = unitObjectPtr(0x2Ca).Read<uint8_t>();
					if (rightWeaponIndex != 0xFF)
					{
						auto rightWeaponObject = Blam::Objects::Get(unitObjectPtr(0x2D0 + 4 * rightWeaponIndex).Read<uint32_t>());
						if (rightWeaponObject)
						{
							auto weap = Blam::Tags::TagInstance(rightWeaponObject->TagIndex).GetDefinition<Blam::Tags::Items::Weapon>();
							hasObjective = weap->MultiplayerWeaponType != Blam::Tags::Items::Weapon::MultiplayerType::None;
							if (hasObjective)
								teamObjective[player.Properties.TeamIndex] = true;
								if (hasObjective && session->HasTeams() && session->MembershipInfo.GetPeerTeam(session->MembershipInfo.LocalPeerIndex) != player.Properties.TeamIndex)
								hasObjective = false;
						}
					}
				}
			}

			writer.Key("hasObjective");
			writer.Bool(hasObjective);

			//gametype specific stats
			writer.Key("flagKills");
			writer.Int(playerStats.WeaponStats[Blam::Tags::Objects::DamageReportingType::Flag].Kills);
			writer.Key("ballKills");
			writer.Int(playerStats.WeaponStats[Blam::Tags::Objects::DamageReportingType::Ball].Kills);

			writer.Key("kingsKilled");
			writer.Int(playerStats.KingsKilled);
			writer.Key("timeInHill");
			writer.Int(playerStats.TimeInHill);
			writer.Key("timeControllingHill");
			writer.Int(playerStats.TimeControllingHill);

			writer.Key("humansInfected");
			writer.Int(playerStats.HumansInfected);
			writer.Key("zombiesKilled");
			writer.Int(playerStats.ZombiesKilled);

			writer.EndObject();
			playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
		}
		writer.EndArray();

		writer.Key("teamHasObjective");
		writer.StartArray();
		for (int i = 0; i < 10; i++)
		{
			writer.Bool(teamObjective[i]);
		}
		writer.EndArray();

		writer.EndObject();

		return buffer.GetString();
	}
}

namespace
{
	void OnEvent(Blam::DatumIndex player, const Event *event, const EventDefinition *definition)
	{
		//Update the scoreboard whenever an event occurs
		Web::Ui::ScreenLayer::Notify("scoreboard", Web::Ui::WebScoreboard::getScoreboard(), true);

		if (event->NameStringId == 0x4004D || event->NameStringId == 0x4005A) // "general_event_game_over" / "general_event_round_over"
		{
			postgameDisplayed = Blam::Time::GetGameTicks();
			postgame = true;
		}
	}

	void OnScoreUpdate()
	{
		//Update the scoreboard whenever an event occurs
		Web::Ui::ScreenLayer::Notify("scoreboard", Web::Ui::WebScoreboard::getScoreboard(), true);
	}

	void OnGameInputUpdated()
	{
		if (!acceptsInput)
			return;

		auto uiSelect = GetActionState(eGameActionUiSelect);

		if (!(uiSelect->Flags & eActionStateFlagsHandled) && uiSelect->Ticks == 1)
		{
			uiSelect->Flags |= eActionStateFlagsHandled;

			if (strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") != 0)
			{
				//If shift is held down or was pressed again within the repeat delay, lock the scoreboard
				locked = GetKeyTicks(eKeyCodeShift, eInputTypeUi) || ((GetTickCount() - lastPressedTime) < 250
					&& Modules::ModuleInput::Instance().VarTapScoreboard->ValueInt == 1);

				Web::Ui::WebScoreboard::Show(locked, postgame);

				lastPressedTime = GetTickCount();
				pressedLastTick = true;	
			}
			else
			{
				Web::Ui::WebScoreboard::Show(true, false);
			}
		}
		//Hide the scoreboard when you release tab. Only check when the scoreboard isn't locked.
		else if(!locked && !postgame && pressedLastTick && uiSelect->Ticks == 0)
		{		
			Web::Ui::WebScoreboard::Hide();
			pressedLastTick = false;		
		}
	}
}