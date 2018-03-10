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
	uint32_t postgameDisplayed;
	const float postgameDelayTime = 2;
	time_t scoreboardSentTime = 0;


	void OnEvent(Blam::DatumHandle player, const Event *event, const EventDefinition *definition);
	void OnGameInputUpdated();
}

namespace Web::Ui::WebScoreboard
{
	void Init()
	{
		Patches::Events::OnEvent(OnEvent);
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
		time(&scoreboardSentTime);
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
			if (scoreboardSentTime != 0 && curTime1 - scoreboardSentTime > 1)
			{
				Web::Ui::ScreenLayer::Notify("scoreboard", getScoreboard(), true);
				time(&scoreboardSentTime);
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
			if (playerIndex == Blam::DatumHandle::Null || !(player = Blam::Players::GetPlayers().Get(playerIndex)))
				return;

			auto secondsUntilSpawn = Pointer(player)(0x2CBC).Read<int>();
			auto firstTimeSpawning = Pointer(player)(0x4).Read<uint32_t>() & 8;

			if (player->SlaveUnit != Blam::DatumHandle::Null)
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

			previousHasUnit = player->SlaveUnit != Blam::DatumHandle::Null;
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

		auto hasTeams = session->HasTeams();
		writer.Key("hasTeams");
		writer.Bool(hasTeams);

		auto get_number_of_rounds = (int(*)())(0x005504C0);
		auto get_current_round = (int(*)())(0x00550AD0);

		writer.Key("numberOfRounds");
		writer.Int(get_number_of_rounds());

		writer.Key("currentRound");
		writer.Int(get_current_round());

		writer.Key("teamScores");
		writer.StartArray();
		for (int t = 0; t < 8; t++)
		{
			const auto &teamScore = scoreboard->TeamScores[t];
			writer.Int((uint32_t(teamScore.TotalScore) << 16) | (teamScore.Score & 0xffff));
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

		int playerIdx = session->MembershipInfo.FindFirstPlayer();
		uint16_t teamObjectiveSet = 0;

		writer.Key("players");
		writer.StartArray();

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

			char buff[17];
			sprintf_s(buff, "#%x", player.Properties.Customization.Colors[Blam::Players::ColorIndices::Primary]);
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
			if (playerDatum.GetSalt() && UnitHasObjective(playerDatum.SlaveUnit))
			{
				hasObjective = true;
				if (hasTeams) 
				{
					auto localPeerTeamIndex = session->MembershipInfo.GetPeerTeam(session->MembershipInfo.LocalPeerIndex);
					teamObjectiveSet |= (1 << player.Properties.TeamIndex);
					hasObjective = localPeerTeamIndex != player.Properties.TeamIndex;
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

		if (hasTeams) {
			writer.Key("teamHasObjective");
			writer.Int(teamObjectiveSet);
		}
		
		writer.EndObject();

		return buffer.GetString();
	}
}

namespace
{
	void OnEvent(Blam::DatumHandle player, const Event *event, const EventDefinition *definition)
	{
		if (event->NameStringId == 0x4004D || event->NameStringId == 0x4005A) // "general_event_game_over" / "general_event_round_over"
		{
			Web::Ui::ScreenLayer::Notify("scoreboard", Web::Ui::WebScoreboard::getScoreboard(), true);

			postgameDisplayed = Blam::Time::GetGameTicks();
			postgame = true;
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
}