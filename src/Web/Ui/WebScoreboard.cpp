#include "WebScoreboard.hpp"
#include "ScreenLayer.hpp"
#include "../../Blam/BlamNetwork.hpp"
#include "../../Blam/BlamEvents.hpp"
#include "../../Patches/Events.hpp"
#include "../../Patches/Input.hpp"
#include "../../Pointer.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../Utils/String.hpp"

#include <iomanip>

using namespace Blam::Input;
using namespace Blam::Events;

namespace
{
	bool locked = false;
	bool postgame = false;
	time_t postgameDisplayed;
	const time_t postgameDisplayTime = 10;

	void OnEvent(Blam::DatumIndex player, const Event *event, const EventDefinition *definition);
	void OnGameInputUpdated();
}

namespace Web
{
	namespace Ui
	{
		namespace WebScoreboard
		{
			void Init()
			{
				Patches::Events::OnEvent(OnEvent);
				Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
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

			//Used to skip the 40 second wait at the end of a round.
			//It would probably be better to find the code that is causing the wait and fix it there.
			//This also fixes the black screen of death that happens sometimes at the end of a round. \o/
			void Tick()
			{
				if (postgame)
				{
					time_t curTime;
					time(&curTime);
					if (curTime - postgameDisplayed > postgameDisplayTime)
					{
						auto session = Blam::Network::GetActiveSession();
						if (session)
							session->Parameters.SetSessionMode(1);
						postgame = false;
					}
				}
			}

			std::string getScoreboard()
			{
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

				writer.StartObject();

				auto session = Blam::Network::GetActiveSession();
				if (!session || !session->IsEstablished())
				{
					writer.EndObject();
					return buffer.GetString();
				}

				if (session->HasTeams())
				{
					writer.Key("hasTeams");
					writer.Bool(true);
					writer.Key("redScore");
					writer.Int(Pointer(0x01879DA8).Read<uint32_t>());
					writer.Key("blueScore");
					writer.Int(Pointer(0x01879DAC).Read<uint32_t>());
				}
				else
				{
					writer.Key("hasTeams");
					writer.Bool(false);
					writer.Key("redScore");
					writer.Int(0);
					writer.Key("blueScore");
					writer.Int(0);
				}

				int32_t variantType = Pointer(0x023DAF18).Read<int32_t>();
				if (variantType >= 0 && variantType < Blam::GameTypeCount)
				{
					writer.Key("gameType");
					writer.String(Blam::GameTypeNames[variantType].c_str());
				}

				writer.Key("players");
				writer.StartArray();
				int peerIdx = session->MembershipInfo.FindFirstPeer();
				while (peerIdx != -1)
				{
					int playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
					if (playerIdx != -1)
					{
						auto player = session->MembershipInfo.PlayerSessions[playerIdx];
						auto playerStats = Blam::Players::GetStats(playerIdx);
						writer.StartObject();
						// Player information
						writer.Key("name");
						writer.String(Utils::String::ThinString(player.Properties.DisplayName).c_str());
						writer.Key("team");
						writer.Int(player.Properties.TeamIndex);
						std::stringstream color;
						color << "#" << std::setw(6) << std::setfill('0') << std::hex << player.Properties.Customization.Colors[Blam::Players::ColorIndices::Primary];
						writer.Key("color");
						writer.String(color.str().c_str());
						writer.Key("index");
						writer.Int(playerIdx);
						// Generic score information
						writer.Key("kills");
						writer.Int(playerStats.Kills);
						writer.Key("assists");
						writer.Int(playerStats.Assists);
						writer.Key("deaths");
						writer.Int(playerStats.Deaths);
						writer.Key("score");
						writer.Int(playerStats.Score);

						writer.EndObject();
					}
					peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
				}
				writer.EndArray();
				writer.EndObject();

				return buffer.GetString();
			}
		}
	}
}

namespace
{
	void OnEvent(Blam::DatumIndex player, const Event *event, const EventDefinition *definition)
	{
		//Update the scoreboard whenever an event occurs
		Web::Ui::ScreenLayer::Notify("scoreboard", Web::Ui::WebScoreboard::getScoreboard(), true);

		if (event->NameStringId == 0x4004D)// "general_event_game_over"
		{
			time(&postgameDisplayed);
			locked = true;
			postgame = true;
			Web::Ui::WebScoreboard::Show(locked, postgame);
		}
	}

	void OnGameInputUpdated()
	{
		BindingsTable bindings;
		GetBindings(0, &bindings);

		if (GetKeyTicks(bindings.PrimaryKeys[eGameActionUiSelect], eInputTypeUi) == 1 || GetKeyTicks(bindings.SecondaryKeys[eGameActionUiSelect], eInputTypeUi) == 1)
		{
			if (strcmp((char*)Pointer(0x22AB018)(0x1A4), "mainmenu") != 0)
			{
				//If shift is held down then lock the scoreboard
				if (GetKeyTicks(eKeyCodeShift, eInputTypeUi) == 0)
					locked = false;
				else
					locked = true;
				Web::Ui::WebScoreboard::Show(locked, postgame);
			}
		}

		if (!locked && !postgame)
		{
			//Hide the scoreboard when you release tab. Only check when the scoreboard isn't locked.
			if (GetKeyTicks(bindings.PrimaryKeys[eGameActionUiSelect], eInputTypeUi) == 0 && GetKeyTicks(bindings.SecondaryKeys[eGameActionUiSelect], eInputTypeUi) == 0)
			{
				Web::Ui::WebScoreboard::Hide();
				locked = false;
			}
		}
	}
}