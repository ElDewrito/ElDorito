#pragma once
#include "ClientFunctions.hpp"
#include "../../Ui/ScreenLayer.hpp"
#include "../../../CommandMap.hpp"
#include "../../../Blam/BlamNetwork.hpp"
#include "../../../Patches/Network.hpp"
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

namespace Anvil
{
	namespace Client
	{
		namespace Rendering
		{
			namespace Bridge
			{
				namespace ClientFunctions
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
						*p_Result = Modules::CommandMap::Instance().ExecuteCommand(s_CommandValue->value.GetString(), !s_IsInternal);
						return QueryError_Ok;
					}

					QueryError OnPing(const rapidjson::Value &p_Args, std::string *p_Result)
					{
						// Get the "address" argument
						auto s_AddressValue = p_Args.FindMember("address");
						if (s_AddressValue == p_Args.MemberEnd() || !s_AddressValue->value.IsString())
						{
							*p_Result = "Bad query: An \"address\" argument is required and must be a string";
							return QueryError_BadQuery;
						}

						// Parse it
						Blam::Network::NetworkAddress blamAddress;
						if (!Blam::Network::NetworkAddress::Parse(s_AddressValue->value.GetString(), 11774, &blamAddress))
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
						if (s_CaptureValue == p_Args.MemberEnd() || !s_CaptureValue->value.IsBool())
						{
							*p_Result = "Bad query: A \"capture\" argument is required and must be a boolean";
							return QueryError_BadQuery;
						}

						// Toggle input capture
						Web::Ui::ScreenLayer::CaptureInput(s_CaptureValue->value.GetBool());
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
				}
			}
		}
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