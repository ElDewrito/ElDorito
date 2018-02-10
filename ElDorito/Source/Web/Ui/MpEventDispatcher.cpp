#include "MpEventDispatcher.hpp"
#include "../../Blam/BlamEvents.hpp"
#include "../../Patches/Events.hpp"
#include "ScreenLayer.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include <unordered_map>
#include <sstream>

using namespace Blam::Events;

namespace
{
	void OnEvent(Blam::DatumHandle player, const Event *event, const EventDefinition *definition);
	std::string GetEventName(uint32_t stringId);

	extern std::unordered_map<uint32_t, std::string> EventNames;
}

namespace Web::Ui::MpEventDispatcher
{
	void Init()
	{
		Patches::Events::OnEvent(OnEvent);
	}
}

namespace
{
	void OnEvent(Blam::DatumHandle player, const Event *event, const EventDefinition *definition)
	{
		// Ignore the event if it's not targeted at the local player
		typedef uint32_t(*GetLocalPlayerPtr)(int index);
		auto GetLocalPlayer = reinterpret_cast<GetLocalPlayerPtr>(0x589C30);
		if (player.Handle != GetLocalPlayer(0))
			return;

		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
		jsonWriter.StartObject();

		// name
		auto eventName = GetEventName(event->NameStringId);
		jsonWriter.Key("name");
		jsonWriter.String(eventName.c_str());

		// category
		jsonWriter.Key("category");
		jsonWriter.Int(event->Type);

		// audience
		jsonWriter.Key("audience");
		jsonWriter.Int(definition->Audience);

		jsonWriter.EndObject();

		// Broadcast a "mpevent" event to all screens
		Web::Ui::ScreenLayer::Notify("mpevent", jsonBuffer.GetString(), true);
	}

	std::string GetEventName(uint32_t stringId)
	{
		// Try to find the stringID in the event names list
		std::string name;
		auto it = EventNames.find(stringId);
		if (it != EventNames.end())
			return it->second;

		// If it wasn't found, then just convert it to a hex number
		std::ostringstream ss;
		ss << "0x" << std::hex << stringId;
		return ss.str();
	}

	std::unordered_map<uint32_t, std::string> EventNames =
	{
		{ 0x40010, "earn_wp_event_kill" },
		{ 0x40011, "earn_wp_event_assist" },
		{ 0x40012, "earn_wp_event_perfection" },
		{ 0x40013, "earn_wp_event_extermination" },
		{ 0x40014, "earn_wp_event_multikill_2" },
		{ 0x40015, "earn_wp_event_multikill_3" },
		{ 0x40016, "earn_wp_event_multikill_4" },
		{ 0x40017, "earn_wp_event_multikill_5" },
		{ 0x40018, "earn_wp_event_multikill_6" },
		{ 0x40019, "earn_wp_event_multikill_7" },
		{ 0x4001A, "earn_wp_event_multikill_8" },
		{ 0x4001B, "earn_wp_event_multikill_9" },
		{ 0x4001C, "earn_wp_event_multikill_10" },
		{ 0x4001D, "earn_wp_event_bash_kill" },
		{ 0x4001E, "earn_wp_event_bashbehind_kill" },
		{ 0x4001F, "earn_wp_event_kill_sniper" },
		{ 0x40020, "earn_wp_event_stickygrenade_kill" },
		{ 0x40021, "earn_wp_event_spartanlaser_kill" },
		{ 0x40022, "earn_wp_event_oddball_carrier_kill_player" },
		{ 0x40023, "earn_wp_event_ctf_flag_carrier_kill_player" },
		{ 0x40024, "earn_wp_event_flame_kill" },
		{ 0x40025, "earn_wp_event_player_kill_spreeplayer" },
		{ 0x40026, "earn_wp_event_deadplayer_kill" },
		{ 0x40027, "earn_wp_event_vehicle_impact_kill" },
		{ 0x40028, "earn_wp_event_vehicle_hijack" },
		{ 0x40029, "earn_wp_event_shotgun_kill_sword" },
		{ 0x4002A, "earn_wp_event_driver_assist_gunner" },
		{ 0x4002B, "earn_wp_event_aircraft_hijack" },
		{ 0x4002C, "earn_wp_event_ctf_flag_player_kill_carrier" },
		{ 0x4002D, "earn_wp_event_ctf_flag_captured" },
		{ 0x4002E, "earn_wp_event_koth_5" },
		{ 0x4002F, "earn_wp_event_revenge" },
		{ 0x40030, "earn_wp_event_flag_grabbed_from_stand" },
		{ 0x40031, "earn_wp_event_flag_returned_by_player" },
		{ 0x40032, "earn_wp_event_5_kills_in_a_row" },
		{ 0x40033, "earn_wp_event_10_kills_in_a_row" },
		{ 0x40034, "earn_wp_event_15_kills_in_a_row" },
		{ 0x40035, "earn_wp_event_20_kills_in_a_row" },
		{ 0x40036, "earn_wp_event_25_kills_in_a_row" },
		{ 0x40037, "earn_wp_event_30_kills_in_a_row" },
		{ 0x40038, "earn_wp_event_vehicle_5" },
		{ 0x40039, "earn_wp_event_headshot" },
		{ 0x4003A, "general_event_kill" },
		{ 0x4003B, "general_event_suicide" },
		{ 0x4003C, "general_event_kill_teammate" },
		{ 0x4003D, "general_event_victory" },
		{ 0x4003E, "general_event_team_victory" },
		{ 0x40045, "general_event_player_quit" },
		{ 0x40046, "general_event_player_joined" },
		{ 0x40047, "general_event_killed_by_unknown" },
		{ 0x40048, "general_event_30_minutes_left" },
		{ 0x40049, "general_event_15_minutes_left" },
		{ 0x4004A, "general_event_5_minutes_left" },
		{ 0x4004B, "general_event_1_minute_left" },
		{ 0x4004D, "general_event_game_over" },
		{ 0x4004E, "general_event_respawn_tick" },
		{ 0x4004F, "general_event_respawn_final_tick" },
		{ 0x40050, "general_event_teleporter_used" },
		{ 0x40051, "general_event_teleporter_blocked" },
		{ 0x40052, "general_event_player_switched_team" },
		{ 0x40053, "general_event_player_rejoined" },
		{ 0x40054, "general_event_gained_lead" },
		{ 0x40055, "general_event_gained_team_lead" },
		{ 0x40056, "general_event_lost_lead" },
		{ 0x40057, "general_event_lost_team_lead" },
		{ 0x40058, "general_event_tied_leader" },
		{ 0x40059, "general_event_tied_team_leader" },
		{ 0x4005A, "general_event_round_over" },
		{ 0x4005B, "general_event_30_seconds_left" },
		{ 0x4005C, "general_event_10_seconds_left" },
		{ 0x4005D, "general_event_killed_by_falling" },
		{ 0x4005E, "general_event_kill_collision" },
		{ 0x4005F, "general_event_kill_melee" },
		{ 0x40060, "general_event_sudden_death" },
		{ 0x40061, "general_event_player_booted_player" },
		{ 0x40062, "general_event_kill_flag_carrier" },
		{ 0x40063, "general_event_kill_bomb_carrier" },
		{ 0x40064, "general_event_kill_sticky_grenade" },
		{ 0x40065, "general_event_kill_sniper" },
		{ 0x40066, "general_event_kill_stealth_melee" },
		{ 0x40067, "general_event_boarded_vehicle" },
		{ 0x40068, "general_event_game_start_team_notification" },
		{ 0x40069, "general_event_teleporter_telefrag" },
		{ 0x4006C, "general_event_shotgun_kill_sword" },
		{ 0x4006D, "general_event_kill_deadplayer" },
		{ 0x4006E, "general_event_vehicle_hijack" },
		{ 0x4006F, "general_event_aircraft_hijack" },
		{ 0x40070, "general_event_kill_spartanlaser" },
		{ 0x40071, "general_event_kill_flame" },
		{ 0x40072, "general_event_driver_assist_gunner" },
		{ 0x40073, "flavor_event_extermination" },
		{ 0x40074, "flavor_event_perfection" },
		{ 0x40075, "flavor_event_multikill_2" },
		{ 0x40076, "flavor_event_multikill_3" },
		{ 0x40077, "flavor_event_multikill_4" },
		{ 0x40078, "flavor_event_multikill_5" },
		{ 0x40079, "flavor_event_multikill_6" },
		{ 0x4007A, "flavor_event_multikill_7" },
		{ 0x4007B, "flavor_event_multikill_8" },
		{ 0x4007C, "flavor_event_multikill_9" },
		{ 0x4007D, "flavor_event_multikill_10" },
		{ 0x4007E, "flavor_event_5_kills_in_a_row" },
		{ 0x4007F, "flavor_event_10_kills_in_a_row" },
		{ 0x40080, "flavor_event_15_kills_in_a_row" },
		{ 0x40081, "flavor_event_20_kills_in_a_row" },
		{ 0x40082, "flavor_event_25_kills_in_a_row" },
		{ 0x40083, "flavor_event_30_kills_in_a_row" },
		{ 0x40084, "flavor_event_sniper_5" },
		{ 0x40085, "flavor_event_sniper_10" },
		{ 0x40086, "flavor_event_shotgun_5" },
		{ 0x40087, "flavor_event_shotgun_10" },
		{ 0x40088, "flavor_event_vehicle_5" },
		{ 0x40089, "flavor_event_vehicle_10" },
		{ 0x4008A, "flavor_event_sword_5" },
		{ 0x4008B, "flavor_event_sword_10" },
		{ 0x4008C, "flavor_event_juggernaut_5" },
		{ 0x4008D, "flavor_event_juggernaut_10" },
		{ 0x4008E, "flavor_event_zombie_5" },
		{ 0x4008F, "flavor_event_zombie_10" },
		{ 0x40090, "flavor_event_human_5" },
		{ 0x40091, "flavor_event_human_10" },
		{ 0x40092, "flavor_event_human_15" },
		{ 0x40093, "flavor_event_koth_5" },
		{ 0x40094, "flavor_event_shotgun_kill_sword" },
		{ 0x40095, "flavor_event_vehicle_impact_kill" },
		{ 0x40096, "flavor_event_vehicle_hijack" },
		{ 0x40097, "flavor_event_aircraft_hijack" },
		{ 0x40098, "flavor_event_deadplayer_kill" },
		{ 0x40099, "flavor_event_player_kill_spreeplayer" },
		{ 0x4009A, "flavor_event_spartanlaser_kill" },
		{ 0x4009B, "flavor_event_stickygrenade_kill" },
		{ 0x4009C, "flavor_event_sniper_kill" },
		{ 0x4009D, "flavor_event_bashbehind_kill" },
		{ 0x4009E, "flavor_event_bash_kill" },
		{ 0x4009F, "flavor_event_flame_kill" },
		{ 0x400A0, "flavor_event_driver_assist_gunner" },
		{ 0x400A1, "flavor_event_assault_bomb_planted" },
		{ 0x400A2, "flavor_event_assault_player_kill_carrier" },
		{ 0x400A3, "flavor_event_vip_player_kill_vip" },
		{ 0x400A4, "flavor_event_juggernaut_player_kill_juggernaut" },
		{ 0x400A5, "flavor_event_oddball_carrier_kill_player" },
		{ 0x400A6, "flavor_event_ctf_flag_captured" },
		{ 0x400A7, "flavor_event_ctf_flag_player_kill_carrier" },
		{ 0x400A8, "flavor_event_ctf_flag_carrier_kill_player" },
		{ 0x400A9, "flavor_event_infection_survive" },
		{ 0x400AA, "flavor_event_nemesis" },
		{ 0x400AB, "flavor_event_avenger" },
		{ 0x400AC, "flavor_forge_cannot_place_object" },
		{ 0x400AD, "flavor_forge_theoretical_object_limit_reached" },
		{ 0x400AE, "slayer_event_game_start" },
		{ 0x400AF, "slayer_event_new_target" },
		{ 0x400B0, "ctf_event_game_start" },
		{ 0x400B1, "ctf_event_flag_taken" },
		{ 0x400B2, "ctf_event_flag_grabbed_from_stand" },
		{ 0x400B3, "ctf_event_flag_dropped" },
		{ 0x400B4, "ctf_event_flag_returned_by_player" },
		{ 0x400B5, "ctf_event_flag_returned_by_timeout" },
		{ 0x400B6, "ctf_event_flag_captured" },
		{ 0x400B7, "ctf_event_flag_new_defensive_team" },
		{ 0x400BE, "ctf_event_flag_capture_faliure" },
		{ 0x400BF, "oddball_event_game_start" },
		{ 0x400C0, "oddball_event_ball_spawned" },
		{ 0x400C1, "oddball_event_ball_picked_up" },
		{ 0x400C2, "oddball_event_ball_dropped" },
		{ 0x400C3, "oddball_event_ball_reset" },
		{ 0x400C5, "oddball_event_10_points_remaining" },
		{ 0x400C6, "oddball_event_25_points_remaining" },
		{ 0x400C8, "king_event_game_start" },
		{ 0x400C9, "king_event_hill_controlled" },
		{ 0x400CA, "king_event_hill_contested" },
		{ 0x400CC, "king_event_hill_move" },
		{ 0x400CD, "king_event_hill_controlled_team" },
		{ 0x400CE, "king_event_hill_contested_team" },
		{ 0x400CF, "vip_event_game_start" },
		{ 0x400D0, "vip_event_new_vip" },
		{ 0x400D1, "vip_event_vip_killed" },
		{ 0x400D2, "vip_event_vip_died" },
		{ 0x400D3, "vip_event_side_switch" },
		{ 0x400D4, "vip_event_reached_destination_area" },
		{ 0x400D5, "vip_event_destination_area_moved" },
		{ 0x400D6, "juggernaut_event_game_start" },
		{ 0x400D7, "juggernaut_event_new_juggernaut" },
		{ 0x400D8, "juggernaut_event_zone_moved" },
		{ 0x400D9, "juggernaut_event_player_kill_juggernaut" },
		{ 0x400DA, "territories_event_game_start" },
		{ 0x400DB, "territories_event_territory_captured" },
		{ 0x400DE, "territories_event_side_switch" },
		{ 0x400DF, "assault_event_game_start" },
		{ 0x400E0, "assault_event_bomb_taken" },
		{ 0x400E1, "assault_event_bomb_dropped" },
		{ 0x400E2, "assault_event_bomb_returned_by_player" },
		{ 0x400E3, "assault_event_bomb_returned_by_timeout" },
		{ 0x400E4, "assault_event_bomb_captured" },
		{ 0x400EB, "assault_event_bomb_disarmed" },
		{ 0x400EC, "assault_event_bomb_placed_on_enemy_post" },
		{ 0x400F0, "assault_event_neutral_bomb_returned" },
		{ 0x400F1, "assault_event_last_player_on_team" },
		{ 0x400F2, "infection_event_game_start" },
		{ 0x400F3, "infection_event_new_infection" },
		{ 0x400F4, "infection_event_zombie_spawn" },
		{ 0x400F5, "infection_event_alpha_zombie_spawn" },
		{ 0x400F6, "infection_event_survive" },
	};
}