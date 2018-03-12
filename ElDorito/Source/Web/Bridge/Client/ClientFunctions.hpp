#pragma once
#include "../WebRendererQuery.hpp"

namespace Anvil::Client::Rendering::Bridge::ClientFunctions
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
	QueryError OnScoreboardHide(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnStats(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnSubmitVirtualKeyboard(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnCancelVirtualKeyboard(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnGameAction(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnVoIPSpeakingChanged(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnVoIPConnectedChanged(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnVoIPPlayerSpeakingChanged(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnIsMapLoading(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnForgeAction(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnShowLan(const rapidjson::Value &p_Args, std::string *p_Result);
	QueryError OnDiscordReply(const rapidjson::Value &p_Args, std::string *p_Result);
}
