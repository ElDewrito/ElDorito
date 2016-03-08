#pragma once
#include "../WebRendererQuery.hpp"
#include "../../../Patches/WebOverlay.hpp"
#include "../../../CommandMap.hpp"
#include "../../../Blam/BlamNetwork.hpp"

namespace Anvil
{
	namespace Client
	{
		namespace Rendering
		{
			namespace Bridge
			{
				class ClientFunctions
				{
				public:
					static QueryError OnShow(const rapidjson::Value &p_Args, std::string *p_Result)
					{
						Patches::WebOverlay::Show(true);
						return QueryError_Ok;
					}

					static QueryError OnHide(const rapidjson::Value &p_Args, std::string *p_Result)
					{
						Patches::WebOverlay::Show(false);
						return QueryError_Ok;
					}

					static QueryError OnCommand(const rapidjson::Value &p_Args, std::string *p_Result)
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

					static QueryError OnPing(const rapidjson::Value &p_Args, std::string *p_Result)
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
						
						// Send a ping message
						auto session = Blam::Network::GetActiveSession();
						if (!session || !session->Gateway->Ping(blamAddress, Patches::WebOverlay::GetPingId()))
						{
							*p_Result = "Network error: Failed to send ping";
							return QueryError_NetworkError;
						}
						return QueryError_Ok;
					}

					static QueryError OnCaptureInput(const rapidjson::Value &p_Args, std::string *p_Result)
					{
						// Get the "capture" argument
						auto s_CaptureValue = p_Args.FindMember("capture");
						if (s_CaptureValue == p_Args.MemberEnd() || !s_CaptureValue->value.IsBool())
						{
							*p_Result = "Bad query: A \"capture\" argument is required and must be a boolean";
							return QueryError_BadQuery;
						}

						// Toggle input capture
						Patches::WebOverlay::CaptureInput(s_CaptureValue->value.GetBool());
						return QueryError_Ok;
					}
				};
			}
		}
	}
}
