#pragma once
#include "ClientFunctions.hpp"
#include "../../Ui/ScreenLayer.hpp"
#include "../../../CommandMap.hpp"
#include "../../../Blam/BlamNetwork.hpp"
#include "../../../Patches/Network.hpp"
#include "../../../Utils/VersionInfo.hpp"

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