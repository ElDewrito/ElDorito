#pragma once
#include "../WebRendererQuery.hpp"

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
					QueryError OnShow(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnHide(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnCommand(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnPing(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnCaptureInput(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnVersion(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnMapVariantInfo(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnGameVariantInfo(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnCommands(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnSendChat(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnSessionInfo(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnScoreboard(const rapidjson::Value &p_Args, std::string *p_Result);
					QueryError OnStats(const rapidjson::Value &p_Args, std::string *p_Result);
				}
			}
		}
	}
}
