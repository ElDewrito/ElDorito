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
#include "../../Modules/ModuleGame.hpp"

using namespace Blam::Input;
using namespace Blam::Events;

namespace
{
	const float kScoreboardUpdateRateSeconds = 0.25f;

	bool scoreboardShown = false;
	bool locked = false;
	bool postgame = false;
	bool pregame = false;
	bool returningToLobby = false;
	bool acceptsInput = true;
	bool pressedLastTick = false;
	bool spawningSoon = false;
	bool roundInProgress = false;
	int lastPressedTime = 0;
	uint32_t postgameDisplayed;
	const float postgameDelayTime = 2;
	uint32_t scoreboardSentTime = 0;

	void OnEvent(Blam::DatumHandle player, const Event *event, const EventDefinition *definition);
	void OnGameInputUpdated();
	void getScoreboardInternal(rapidjson::Writer<rapidjson::StringBuffer>& writer);
}

namespace Web::Ui::WebScoreboard
{
	void Init()
	{
		Patches::Events::OnEvent(OnEvent);
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
		scoreboardSentTime = 0;
	}

	void Show(bool locked, bool postgame)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		jsonWriter.StartObject();
		jsonWriter.Key("expanded");
		jsonWriter.Bool(Modules::ModuleGame::Instance().VarExpandedScoreboard->ValueInt != 0);
		jsonWriter.Key("locked");
		jsonWriter.Bool(locked);
		jsonWriter.Key("postgame");
		jsonWriter.Bool(postgame);
		jsonWriter.Key("scoreboardData");
		getScoreboardInternal(jsonWriter);
		jsonWriter.EndObject();

		scoreboardShown = true;
		ScreenLayer::Show("scoreboard", jsonBuffer.GetString());
	}

	void Hide()
	{
		scoreboardShown = false;
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
			if (Blam::Time::TicksToSeconds(Blam::Time::GetGameTicks() - postgameDisplayed) > postgameDelayTime)
			{
				Web::Ui::WebScoreboard::Show(locked, postgame);
				acceptsInput = false;
				postgame = false;
				returningToLobby = true;
			}
		}
		auto isMainMenu = is_main_menu();

		if (!postgame && !isMainMenu) {
			time_t curTime1;
			time(&curTime1);

			if (scoreboardShown && Blam::Time::TicksToSeconds(scoreboardSentTime++) > kScoreboardUpdateRateSeconds)
			{
				scoreboardSentTime = 0;
				Web::Ui::ScreenLayer::NotifyScreen("scoreboard", "scoreboard", getScoreboard());
				
			}
		}

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
				pregame = true;
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

		roundInProgress = game_engine_round_in_progress();
		if (roundInProgress)
		{
			Blam::Players::PlayerDatum *player{ nullptr };
			auto playerIndex = Blam::Players::GetLocalPlayer(0);
			if (playerIndex == Blam::DatumHandle::Null || !(player = Blam::Players::GetPlayers().Get(playerIndex)))
				return;

			auto secondsUntilSpawn = Pointer(player)(0x2CBC).Read<int>();
			auto firstTimeSpawning = Pointer(player)(0x4).Read<uint32_t>() & 8;

			if (player->SlaveUnit != Blam::DatumHandle::Null)
			{
				spawningSoon = false;
				pregame = false;
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

			previousHasUnit = player->SlaveUnit != Blam::DatumHandle::Null;
		}
	}

	std::string getScoreboard()
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		getScoreboardInternal(jsonWriter);
		return jsonBuffer.GetString();
	}
}

namespace
{
	void OnEvent(Blam::DatumHandle player, const Event *event, const EventDefinition *definition)
	{
		if (event->NameStringId == 0x4004D || event->NameStringId == 0x4005A) // "general_event_game_over" / "general_event_round_over"
		{
			postgameDisplayed = Blam::Time::GetGameTicks();
			postgame = true;

			Web::Ui::ScreenLayer::NotifyScreen("scoreboard", "scoreboard", Web::Ui::WebScoreboard::getScoreboard());
		}
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

	bool UnitHasObjective(uint32_t unitObjectIndex)
	{
		auto unitObject = (uint8_t*)Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return false;
		auto rightWeaponIndex = *(unitObject + 0x2Ca);
		if (rightWeaponIndex == 0xFF)
			return false;

		auto weaponObjectIndex = *(uint32_t*)(unitObject + 0x2D0 + 4 * rightWeaponIndex);
		auto rightWeaponObject = Blam::Objects::Get(weaponObjectIndex);
		if (!rightWeaponObject)
			return false;

		auto weap = Blam::Tags::TagInstance(rightWeaponObject->TagIndex).GetDefinition<Blam::Tags::Items::Weapon>();
		return weap->MultiplayerWeaponType != Blam::Tags::Items::Weapon::MultiplayerType::None;
	}

	void getScoreboardInternal(rapidjson::Writer<rapidjson::StringBuffer> &writer)
	{
		writer.StartObject();

		auto session = Blam::Network::GetActiveSession();
		auto get_multiplayer_scoreboard = (Blam::MutiplayerScoreboard*(*)())(0x00550B80);
		auto* scoreboard = get_multiplayer_scoreboard();
		if (!session || !session->IsEstablished() || !scoreboard)
		{
			writer.EndObject();
			return;
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
		uint16_t teamHasObjectiveSet = 0;
		while (playerIdx != -1)
		{
			auto player = session->MembershipInfo.PlayerSessions[playerIdx];
			auto playerStats = Blam::Players::GetStats(playerIdx);

			bool isAlive = !roundInProgress;
			bool hasObjective = false;
			const auto& playerDatum = Blam::Players::GetPlayers()[playerIdx];
			if (playerDatum.GetSalt())
			{
				if (roundInProgress)
					isAlive = playerDatum.SlaveUnit != Blam::DatumHandle::Null || pregame || postgame;

				if (UnitHasObjective(playerDatum.SlaveUnit))
				{
					hasObjective = true;
					if (hasObjective && session->HasTeams()) {
						teamHasObjectiveSet |= (1 << player.Properties.TeamIndex);
						if (session->MembershipInfo.GetPeerTeam(session->MembershipInfo.LocalPeerIndex) != player.Properties.TeamIndex)
							hasObjective = false;
					}
				}
			}

			writer.StartObject();
			// Player information
			writer.Key("name");
			writer.String(Utils::String::ThinString(player.Properties.DisplayName).c_str());
			writer.Key("serviceTag");
			writer.String(Utils::String::ThinString(player.Properties.ServiceTag).c_str());
			writer.Key("team");
			writer.Int(player.Properties.TeamIndex);
			char buff[17];
			sprintf_s(buff, "#%06X", player.Properties.Customization.Colors[Blam::Players::ColorIndices::Primary]);
			writer.Key("color");
			writer.String(buff);
			Blam::Players::FormatUid(buff, player.Properties.Uid);
			writer.Key("UID");
			writer.String(buff);
			writer.Key("isHost");
			if (Modules::ModuleServer::Instance().VarServerDedicatedClient->ValueInt == 1)
				writer.Bool(false);
			else {
				writer.Bool(playerIdx == session->MembershipInfo.HostPeerIndex);
			}

			//uint8_t alive = Pointer(playerStatusBase + (176 * playerIdx)).Read<uint8_t>();
			writer.Key("isAlive");
			//writer.Bool(alive == 1);
			writer.Bool(isAlive);
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
			writer.Int(playerStats.BestStreak);

			

			writer.Key("hasObjective");
			writer.Bool(hasObjective);

			//gametype specific stats
			if (variantType == Blam::GameType::CTF || variantType == Blam::GameType::Assault || variantType == Blam::GameType::Oddball)
			{
				writer.Key("flagKills");
				writer.Int(playerStats.WeaponStats[Blam::Tags::Objects::DamageReportingType::Flag].Kills);
				writer.Key("ballKills");
				writer.Int(playerStats.WeaponStats[Blam::Tags::Objects::DamageReportingType::Ball].Kills);
			}
			else if (variantType == Blam::GameType::KOTH)
			{
				writer.Key("kingsKilled");
				writer.Int(playerStats.KingsKilled);
				writer.Key("timeInHill");
				writer.Int(playerStats.TimeInHill);
				writer.Key("timeControllingHill");
				writer.Int(playerStats.TimeControllingHill);
			}
			else if (variantType == Blam::GameType::Infection)
			{
				writer.Key("humansInfected");
				writer.Int(playerStats.HumansInfected);
				writer.Key("zombiesKilled");
				writer.Int(playerStats.ZombiesKilled);
			}

			writer.EndObject();
			playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
		}
		writer.EndArray();

		if (session->HasTeams()) {
			writer.Key("teamHasObjective");
			writer.Int(teamHasObjectiveSet);
		}

		writer.EndObject();
	}
}