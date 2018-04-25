#pragma once
#include "ClientFunctions.hpp"
#include "../../Ui/ScreenLayer.hpp"
#include "../../Ui/WebVirtualKeyboard.hpp"
#include "../../Ui/WebForge.hpp"
#include "../../Ui/WebScoreboard.hpp"
#include "../../../CommandMap.hpp"
#include "../../../Blam/BlamNetwork.hpp"
#include "../../../Discord/DiscordRPC.h"
#include "../../../Patches/Network.hpp"
#include "../../../Patches/Input.hpp"
#include "../../../Patches/Ui.hpp"
#include "../../../Modules/ModuleVoIP.hpp"
#include "../../../Modules/ModulePlayer.hpp"
#include "../../../Pointer.hpp"
#include "../../../Server/ServerChat.hpp"
#include "../../../Utils/VersionInfo.hpp"
#include "../../../Utils/String.hpp"
#include "../../../ThirdParty/rapidjson/writer.h"
#include "../../../ThirdParty/rapidjson/stringbuffer.h"

namespace
{
	uint16_t PingId;
	bool PingHandlerRegistered;

	void PongReceived(const Blam::Network::NetworkAddress &from, uint32_t timestamp, uint16_t id, uint32_t latency);
}

namespace Anvil::Client::Rendering::Bridge::ClientFunctions
{
	QueryError OnShow(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		Web::Ui::ScreenLayer::Show(true);
		return QueryError_Ok;
	}

	QueryError OnHide(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		Web::Ui::ScreenLayer::Show(false);
		return QueryError_Ok;
	}

	QueryError OnCommand(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		// Get the "command" argument
		auto s_CommandValue = p_Args.FindMember("command");
		if (s_CommandValue == p_Args.MemberEnd() || !s_CommandValue->value.IsString())
		{
			*p_Result = "Bad query: A \"command\" argument is required and must be a string";
			return QueryError_BadQuery;
		}

		// Get the "internal" argument (defaults to false)
		auto s_InternalValue = p_Args.FindMember("internal");
		auto s_IsInternal = (s_InternalValue != p_Args.MemberEnd() && s_InternalValue->value.IsBool() && s_InternalValue->value.GetBool());

		// Run the command
		if (!Modules::CommandMap::Instance().ExecuteCommandWithStatus(s_CommandValue->value.GetString(), !s_IsInternal, p_Result))
			return QueryError_CommandFailed;
		return QueryError_Ok;
	}

	QueryError OnPing(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		// Get the "address" argument
		auto s_AddressValue = p_Args.FindMember("address");
		auto s_port = p_Args.FindMember("port");

		if (s_AddressValue == p_Args.MemberEnd() || !s_AddressValue->value.IsString() || s_port == p_Args.MemberEnd() || !s_port->value.IsInt())
		{
			*p_Result = "Bad query: The \"address\" argument must be a string and the \"port\" argument must be an int";
			return QueryError_BadQuery;
		}

		// Parse it
		Blam::Network::NetworkAddress blamAddress;
		if (!Blam::Network::NetworkAddress::Parse(s_AddressValue->value.GetString(), s_port->value.GetInt(), &blamAddress))
		{
			*p_Result = "Invalid argument: The \"address\" argument is not a valid IP address.";
			return QueryError_InvalidArgument;
		}

		// Register our ping handler if it isn't already
		if (!PingHandlerRegistered)
		{
			PingId = Patches::Network::OnPong(PongReceived);
			PingHandlerRegistered = true;
		}

		// Send a ping message
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->Gateway->Ping(blamAddress, PingId))
		{
			*p_Result = "Network error: Failed to send ping";
			return QueryError_NetworkError;
		}
		return QueryError_Ok;
	}

	QueryError OnCaptureInput(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		// Get the "capture" argument
		auto s_CaptureValue = p_Args.FindMember("capture");
		auto s_CapturePointerValue = p_Args.FindMember("capturePointer");
		bool capturePointer = false;
		if (s_CaptureValue == p_Args.MemberEnd() || !s_CaptureValue->value.IsBool())
		{
			*p_Result = "Bad query: A \"capture\" argument is required and must be a boolean";
			return QueryError_BadQuery;
		}

		if (s_CapturePointerValue != p_Args.MemberEnd() && s_CapturePointerValue->value.IsBool())
		{
			capturePointer = s_CapturePointerValue->value.GetBool();
		}

		// Toggle input capture
		Web::Ui::ScreenLayer::CaptureInput(s_CaptureValue->value.GetBool(), capturePointer);
		return QueryError_Ok;
	}

	QueryError OnVersion(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		*p_Result = Utils::Version::GetVersionString();
		return QueryError_Ok;
	}

	QueryError OnMapVariantInfo(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		// Get the map variant session parameter
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			*p_Result = "Not available: A game session is not active";
			return QueryError_NotAvailable;
		}
		auto mapVariant = reinterpret_cast<uint8_t*>(session->Parameters.MapVariant.Get());
		if (!mapVariant)
		{
			*p_Result = "Not available: Map variant data is not available";
			return QueryError_NotAvailable;
		}

		// Get values out of it
		auto name = Utils::String::ThinString(reinterpret_cast<const wchar_t*>(mapVariant + 0x8));
		auto description = reinterpret_cast<const char*>(mapVariant + 0x28);
		auto author = reinterpret_cast<const char*>(mapVariant + 0xA8);
		auto mapId = *reinterpret_cast<uint32_t*>(mapVariant + 0x100);

		// Build a JSON response
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		writer.StartObject();
		writer.Key("name");
		writer.String(name.c_str());
		writer.Key("description");
		writer.String(description);
		writer.Key("author");
		writer.String(author);
		writer.Key("mapId");
		writer.Int64(mapId);
		writer.EndObject();
		*p_Result = buffer.GetString();
		return QueryError_Ok;
	}

	QueryError OnScoreboard(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			*p_Result = "Cannot get scoreboard data when there is not an active session.";
			return QueryError_BadQuery;
		}

		*p_Result = Web::Ui::WebScoreboard::getScoreboard();
		return QueryError_Ok;
	}

	QueryError OnScoreboardHide(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		Web::Ui::WebScoreboard::Hide();
		return QueryError_Ok;
	}

	QueryError OnGameVariantInfo(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		// Get the game variant session parameter
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			*p_Result = "Not available: A game session is not active";
			return QueryError_NotAvailable;
		}
		auto gameVariant = session->Parameters.GameVariant.Get();
		if (!gameVariant)
		{
			*p_Result = "Not available: Game variant data is not available";
			return QueryError_NotAvailable;
		}

		// Get values out of it
		auto mode = gameVariant->GameType;
		auto name = Utils::String::ThinString(gameVariant->Name);
		auto description = gameVariant->Description;
		auto author = gameVariant->Author;
		auto teams = (gameVariant->TeamGame & 1) != 0;
		auto timeLimit = gameVariant->RoundTimeLimit;
		auto rounds = gameVariant->NumberOfRounds;

		// The score-to-win value location varies depending on gametype
		auto scoreToWin = -1;
		auto rawGameVariant = reinterpret_cast<uint8_t*>(gameVariant);
		switch (gameVariant->GameType)
		{
		case Blam::GameType::Slayer: scoreToWin = *reinterpret_cast<int16_t*>(rawGameVariant + 0x1D4); break;
		case Blam::GameType::Oddball: scoreToWin = *reinterpret_cast<int16_t*>(rawGameVariant + 0x1D8); break;
		case Blam::GameType::KOTH: scoreToWin = *reinterpret_cast<int16_t*>(rawGameVariant + 0x1D8); break;
		case Blam::GameType::CTF: scoreToWin = *reinterpret_cast<int16_t*>(rawGameVariant + 0x1DC); break;
		case Blam::GameType::Assault: scoreToWin = *reinterpret_cast<int16_t*>(rawGameVariant + 0x1DC); break;
		case Blam::GameType::Juggernaut: scoreToWin = *reinterpret_cast<int16_t*>(rawGameVariant + 0x1D4); break;
		case Blam::GameType::VIP: scoreToWin = *reinterpret_cast<int16_t*>(rawGameVariant + 0x1D4); break;
		}

		// Build a JSON response
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		writer.StartObject();
		writer.Key("mode");
		writer.Int(mode);
		writer.Key("name");
		writer.String(name.c_str());
		writer.Key("description");
		writer.String(description);
		writer.Key("author");
		writer.String(author);
		writer.Key("teams");
		writer.Bool(teams);
		writer.Key("timeLimit");
		writer.Int(timeLimit);
		writer.Key("rounds");
		writer.Int(rounds);
		writer.Key("scoreToWin");
		writer.Int(scoreToWin);
		writer.EndObject();
		*p_Result = buffer.GetString();
		return QueryError_Ok;
	}

	QueryError OnCommands(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		const auto &commandMap = Modules::CommandMap::Instance();
						
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		writer.StartArray();
		for (auto &&command : commandMap.Commands)
		{
			writer.StartObject();
			writer.Key("type");
			writer.Int(command.Type);
			writer.Key("module");
			writer.String(command.ModuleName.c_str());
			writer.Key("name");
			writer.String(command.Name.c_str());
			writer.Key("shortName");
			writer.String(command.ShortName.c_str());
			writer.Key("description");
			writer.String(command.Description.c_str());
			switch (command.Type)
			{
			case Modules::eCommandTypeVariableInt:
				writer.Key("value");
				writer.Int(command.ValueInt);
				writer.Key("defaultValue");
				writer.Int(command.DefaultValueInt);
				writer.Key("minValue");
				writer.Int(command.ValueIntMin);
				writer.Key("maxValue");
				writer.Int(command.ValueIntMax);
				break;
			case Modules::eCommandTypeVariableInt64:
				writer.Key("value");
				writer.Int64(command.ValueInt64);
				writer.Key("defaultValue");
				writer.Int64(command.DefaultValueInt64);
				writer.Key("minValue");
				writer.Int64(command.ValueInt64Min);
				writer.Key("maxValue");
				writer.Int64(command.ValueInt64Max);
				break;
			case Modules::eCommandTypeVariableFloat:
				writer.Key("value");
				writer.Double(command.ValueFloat);
				writer.Key("defaultValue");
				writer.Double(command.DefaultValueFloat);
				writer.Key("minValue");
				writer.Double(command.ValueFloatMin);
				writer.Key("maxValue");
				writer.Double(command.ValueFloatMax);
				break;
			case Modules::eCommandTypeVariableString:
				writer.Key("value");
				writer.String(command.ValueString.c_str());
				writer.Key("defaultValue");
				writer.String(command.DefaultValueString.c_str());
				writer.Key("minValue");
				writer.Null();
				writer.Key("maxValue");
				writer.Null();
				break;
			default:
				writer.Key("value");
				writer.Null();
				writer.Key("defaultValue");
				writer.Null();
				writer.Key("minValue");
				writer.Null();
				writer.Key("maxValue");
				writer.Null();
				break;
			}
			writer.Key("replicated");
			writer.Bool((command.Flags & eCommandFlagsReplicated) != 0);
			writer.Key("archived");
			writer.Bool((command.Flags & eCommandFlagsArchived) != 0);
			writer.Key("hidden");
			writer.Bool((command.Flags & eCommandFlagsHidden) != 0);
			writer.Key("hostOnly");
			writer.Bool((command.Flags & eCommandFlagsHostOnly) != 0);
			writer.Key("hideValue");
			writer.Bool((command.Flags & eCommandFlagsOmitValueInList) != 0);
			writer.Key("internal");
			writer.Bool((command.Flags & eCommandFlagsInternal) != 0);
			writer.Key("arguments");
			writer.StartArray();
			for (auto &&arg : command.CommandArgs)
				writer.String(arg.c_str());
			writer.EndArray();
			writer.EndObject();
		}
		writer.EndArray();
		*p_Result = buffer.GetString();
		return QueryError_Ok;
	}

	QueryError OnSendChat(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto message = p_Args.FindMember("message");
		auto teamChat = p_Args.FindMember("teamChat");

		if (message == p_Args.MemberEnd() || !message->value.IsString())
		{
			*p_Result = "Bad query: A \"message\" argument is required and must be a string";
			return QueryError_BadQuery;
		}
		if (teamChat == p_Args.MemberEnd() || !teamChat->value.IsBool())
		{
			*p_Result = "Bad query: A \"teamChat\" argument is required and must be a boolean";
			return QueryError_BadQuery;
		}

		if(teamChat->value.GetBool())
			*p_Result = Server::Chat::SendTeamMessage(message->value.GetString());
		else
			*p_Result = Server::Chat::SendGlobalMessage(message->value.GetString());

		return QueryError_Ok;
	}

	QueryError OnSessionInfo(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			writer.Key("established");
			writer.Bool(false);

			writer.Key("hasTeams");
			writer.Bool(false);

			writer.Key("isHost");
			writer.Bool(false);

			writer.Key("playerInfo");
			writer.StartObject();
			writer.Key("Name");
			writer.String("");
			writer.Key("Uid");
			writer.String("");
			writer.EndObject();
		}
		else
		{
			writer.Key("established");
			writer.Bool(true);

			writer.Key("hasTeams");
			writer.Bool(session->HasTeams());

			writer.Key("isHost");
			writer.Bool(session->IsHost());

			writer.Key("playerInfo");
			writer.StartObject();
			writer.Key("Name");
			writer.String(Utils::String::ThinString(session->MembershipInfo.GetLocalPlayerSession().Properties.DisplayName).c_str());
			writer.Key("Uid");
			char uid[17];
			Blam::Players::FormatUid(uid, session->MembershipInfo.GetLocalPlayerSession().Properties.Uid);
			writer.String(uid);
			writer.EndObject();
		}
		writer.Key("mapName");
		writer.String((char*)Pointer(0x22AB018)(0x1A4));

		writer.EndObject();

		*p_Result = buffer.GetString();
		return QueryError_Ok;
	}

	QueryError OnStats(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto name = p_Args.FindMember("playerName");
		if (name == p_Args.MemberEnd() || !name->value.IsString())
		{
			*p_Result = "Bad query: A \"playerName\" argument is required and must be a string";
			return QueryError_BadQuery;
		}

		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
		{
			*p_Result = "Cannot get stat data when there is not an active session.";
			return QueryError_BadQuery;
		}

		int playerIdx = session->MembershipInfo.FindFirstPlayer();
		while (playerIdx != -1)
		{
			auto player = session->MembershipInfo.PlayerSessions[playerIdx];
			if (strcmp(Utils::String::ThinString(player.Properties.DisplayName).c_str(), name->value.GetString()) == 0)
			{
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				auto playerStats = Blam::Players::GetStats(playerIdx);
				auto pvpStats = Blam::Players::GetPVPStats(playerIdx);
				writer.StartObject();

				writer.Key("playerkilledplayer");
				writer.StartArray();
				for (int i = 0; i < 16; i++)
				{
					if (pvpStats.StatsAgainstEachPlayer[i].Kills > 0)
					{
						auto p = session->MembershipInfo.PlayerSessions[i];
						writer.StartObject();
						writer.Key("PlayerName");
						writer.String(Utils::String::ThinString(p.Properties.DisplayName).c_str());
						writer.Key("Kills");
						writer.Int(pvpStats.StatsAgainstEachPlayer[i].Kills);
						writer.EndObject();
					}
									

				}
				writer.EndArray();

				writer.Key("playerkilledby");
				writer.StartArray();
				for (int i = 0; i < 16; i++)
				{
					if (pvpStats.StatsAgainstEachPlayer[i].KilledBy > 0)
					{
						auto p = session->MembershipInfo.PlayerSessions[i];
						writer.StartObject();
						writer.Key("PlayerName");
						writer.String(Utils::String::ThinString(p.Properties.DisplayName).c_str());
						writer.Key("Kills");
						writer.Int(pvpStats.StatsAgainstEachPlayer[i].KilledBy);
						writer.EndObject();
					}
									

				}
				writer.EndArray();


				writer.Key("medals");
				writer.StartObject();
				for (int medal = 0; medal < Blam::Tags::Objects::MedalType::MedalCount; medal++)
				{
					writer.Key(Blam::Tags::Objects::MedalTypeNames[medal].c_str());
					writer.Int(playerStats.Medals[medal]);
				}
				writer.EndObject();
				writer.Key("weapons");
				writer.StartObject();
				for (int weapon = 0; weapon < Blam::Tags::Objects::DamageReportingType::DamageCount; weapon++)
				{
					writer.Key(Blam::Tags::Objects::DamageReportingTypeNames[weapon].c_str());
					writer.StartObject();

					auto wep = playerStats.WeaponStats[weapon];
					writer.Key("BetrayalsWith");
					writer.Int(wep.BetrayalsWith);
					writer.Key("HeadshotsWith");
					writer.Int(wep.HeadshotsWith);
					writer.Key("KilledBy");
					writer.Int(wep.KilledBy);
					writer.Key("Kills");
					writer.Int(wep.Kills);
					writer.Key("SuicidesWith");
					writer.Int(wep.SuicidesWith);

					writer.EndObject();
				}
				writer.EndObject();

				writer.EndObject();

				*p_Result = buffer.GetString();
				return QueryError_Ok;
			}
			playerIdx = session->MembershipInfo.FindNextPlayer(playerIdx);
		}

		*p_Result = "Could not find player.";
		return QueryError_BadQuery;
	}

	QueryError OnSubmitVirtualKeyboard(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto value = p_Args.FindMember("value");
		if (value == p_Args.MemberEnd() || !value->value.IsString())
		{
			*p_Result = "Bad query: A \"message\" argument is required and must be a string";
			return QueryError_BadQuery;
		}
		Web::Ui::WebVirtualKeyboard::Submit(Utils::String::WidenString(value->value.GetString()));
		return QueryError_Ok;
	}

	QueryError OnCancelVirtualKeyboard(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		Web::Ui::WebVirtualKeyboard::Cancel();
		return QueryError_Ok;
	}

	QueryError OnGameAction(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto value = p_Args.FindMember("key");
		if (value == p_Args.MemberEnd() || !value->value.IsNumber())
		{
			*p_Result = "Bad query : A \"key\" argument is required and must be a number";
			return QueryError_BadQuery;
		}
		if (!Patches::Input::QueueGameAction(value->value.GetInt()))
		{
			*p_Result = "Bad query : \"key\" argument was out of bounds.";
			return QueryError_BadQuery;
		}
		return QueryError_Ok;
	}

	QueryError OnVoIPSpeakingChanged(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto value = p_Args.FindMember("value");
		if (value == p_Args.MemberEnd() || !value->value.IsBool())
		{
			*p_Result = "Bad query : A \"value\" argument is required and must be a bool";
			return QueryError_BadQuery;
		}

		Modules::ModuleVoIP::Instance().voiceDetected = value->value.GetBool();

		Patches::Ui::ToggleSpeakingPlayerName(Modules::ModulePlayer::Instance().VarPlayerName->ValueString , value->value.GetBool());

		return QueryError_Ok;
	}

	QueryError OnVoIPConnectedChanged(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto value = p_Args.FindMember("value");
		if (value == p_Args.MemberEnd() || !value->value.IsBool())
		{
			*p_Result = "Bad query : A \"value\" argument is required and must be a bool";
			return QueryError_BadQuery;
		}

		Modules::ModuleVoIP::Instance().voipConnected = value->value.GetBool();

		return QueryError_Ok;
	}

	QueryError OnVoIPPlayerSpeakingChanged(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto name = p_Args.FindMember("name");
		auto value = p_Args.FindMember("value");
		if (name == p_Args.MemberEnd() || !name->value.IsString())
		{
			*p_Result = "Bad query : A \"name\" argument is required and must be a string";
			return QueryError_BadQuery;
		}
		else if (value == p_Args.MemberEnd() || !value->value.IsBool())
		{
			*p_Result = "Bad query : A \"value\" argument is required and must be a bool";
			return QueryError_BadQuery;
		}

		Patches::Ui::ToggleSpeakingPlayerName(name->value.GetString(), value->value.GetBool());

		return QueryError_Ok;
	}

	QueryError OnIsMapLoading(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		static auto IsMapLoading = (bool(*)())(0x005670E0);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		writer.StartObject();

		writer.Key("loading");
		writer.Bool(IsMapLoading());
		writer.EndObject();

		*p_Result = buffer.GetString();
		return QueryError_Ok;
	}

	QueryError OnForgeAction(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		Web::Ui::WebForge::ProcessAction(p_Args, p_Result);
		return QueryError_Ok;
	}
	
	QueryError OnShowLan(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		Patches::Ui::ShowLanBrowser();
		return QueryError_Ok;
	}

	QueryError OnDiscordReply(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto userId = p_Args.FindMember("userId");
		auto replyValue = p_Args.FindMember("reply");
		if (userId == p_Args.MemberEnd() || !userId->value.IsString())
		{
			*p_Result = "Bad query : A \"userId\" argument is required and must be a string";
			return QueryError_BadQuery;
		}
		else if (replyValue == p_Args.MemberEnd() || !replyValue->value.IsNumber())
		{
			*p_Result = "Bad query : A \"reply\" argument is required and must be a number";
			return QueryError_BadQuery;
		}

		Discord::DiscordRPC::Instance().ReplyToJoinRequest(userId->value.GetString(), replyValue->value.GetInt());
		return QueryError_Ok;
	}
}

namespace
{
	void PongReceived(const Blam::Network::NetworkAddress &from, uint32_t timestamp, uint16_t id, uint32_t latency)
	{
		if (id != PingId)
			return;
		std::string data = "{";
		data += "address: '" + from.ToString() + "'";
		data += ",latency: " + std::to_string(latency);
		data += ",timestamp: " + std::to_string(timestamp);
		data += "}";
		Web::Ui::ScreenLayer::Notify("pong", data, true);
	}
}