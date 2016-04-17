#include "ModuleVoIP.hpp"
#include <sstream>
#include "../ElDorito.hpp"
#include "../VoIP/TeamspeakClient.hpp"
#include "../VoIP/TeamspeakServer.hpp"
#include <teamspeak/public_definitions.h>
#include <teamspeak/public_errors.h>
#include <teamspeak/clientlib_publicdefinitions.h>
#include <teamspeak/clientlib.h>
#include "../Blam/BlamNetwork.hpp"

bool VariablePushToTalkUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	unsigned int error;
	uint64 scHandlerID = VoIPGetscHandlerID();
	if (scHandlerID != NULL){
		if (Modules::ModuleVoIP::Instance().VarVoIPPushToTalk->ValueInt == 0){
			if ((error = ts3client_setPreProcessorConfigValue(scHandlerID, "vad", "true")) != ERROR_ok) {
				returnInfo = "Error setting voice activation detection. Error: " + std::to_string(error);
				return false;
			}
			if ((error = ts3client_setPreProcessorConfigValue(scHandlerID, "voiceactivation_level", Modules::ModuleVoIP::Instance().VarVoIPVADLevel->ValueString.c_str())) != ERROR_ok) {
				returnInfo = "Error setting voice activation level. Error: " + std::to_string(error);
				return false;
			}
			if ((error = ts3client_setClientSelfVariableAsInt(scHandlerID, CLIENT_INPUT_DEACTIVATED, INPUT_ACTIVE)) != ERROR_ok) {
				char* errorMsg;
				if (ts3client_getErrorMessage(error, &errorMsg) != ERROR_ok) {
					ts3client_freeMemory(errorMsg);
				}
			}

			if (ts3client_flushClientSelfUpdates(scHandlerID, NULL) != ERROR_ok) {
				char* errorMsg;
				if (ts3client_getErrorMessage(error, &errorMsg) != ERROR_ok) {
					ts3client_freeMemory(errorMsg);
				}
			}
		}
		else
		{
			if ((error = ts3client_setPreProcessorConfigValue(scHandlerID, "vad", "false")) != ERROR_ok) {
				returnInfo = "Error setting voice activation detection. Error: " + std::to_string(error);
				return false;
			}
			if ((error = ts3client_setPreProcessorConfigValue(scHandlerID, "voiceactivation_level", "-50")) != ERROR_ok) {
				returnInfo = "Error setting voice activation level. Error: " + std::to_string(error);
				return false;
			}
		}
	}
	returnInfo = Modules::ModuleVoIP::Instance().VarVoIPPushToTalk->ValueInt ? "Enabled VoIP PushToTalk and Disabled Voice Activation Detection" : "Disabled VoIP PushToTalk and Enabled Voice Acitivation Detection.";
	return true;
}

bool VariableVolumeModifierUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	unsigned int error;
	uint64 scHandlerID = VoIPGetscHandlerID();
	if (scHandlerID != NULL){
		if ((error = ts3client_setPlaybackConfigValue(scHandlerID, "volume_modifier", Modules::ModuleVoIP::Instance().VarVoIPVolumeModifier->ValueString.c_str())) != ERROR_ok) {
			returnInfo = "Unable to set volume modifier, are you connected to a server? Error: " + std::to_string(error);
			return false;
		}
	}
	returnInfo = "Set VoIP Volume Modifier to " + Modules::ModuleVoIP::Instance().VarVoIPVolumeModifier->ValueString;
	return true;
}
bool VariableAGCUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	unsigned int error;
	uint64 scHandlerID = VoIPGetscHandlerID();
	if (scHandlerID != NULL){
		if ((error = ts3client_setPreProcessorConfigValue(scHandlerID, "agc", Modules::ModuleVoIP::Instance().VarVoIPAGC->ValueInt ? "true" : "false")) != ERROR_ok) {
			returnInfo = "Unable to set Automatic Gain Control, are you connected to a server? Error: " + std::to_string(error);
			return false;
		}
	}
	returnInfo = Modules::ModuleVoIP::Instance().VarVoIPAGC->ValueInt ? "Enabled VoIP Automatic Gain Control" : "Disabled VoIP Automatic Gain Control";
	return true;
}
bool VariableEchoCancellationUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	unsigned int error;
	uint64 scHandlerID = VoIPGetscHandlerID();
	if (scHandlerID != NULL){
		if ((error = ts3client_setPreProcessorConfigValue(scHandlerID, "echo_canceling", Modules::ModuleVoIP::Instance().VarVoIPEchoCancellation->ValueInt ? "true" : "false")) != ERROR_ok) {
			returnInfo = "Unable to set echo cancelation. Error: " + std::to_string(error);
			return false;
		}
	}
	returnInfo = Modules::ModuleVoIP::Instance().VarVoIPEchoCancellation->ValueInt ? "Enabled VoIP Echo Cancellation" : "Disabled VoIP Echo Cancellation";
	return true;
}
bool VariableVADLevelUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	unsigned int error;
	uint64 scHandlerID = VoIPGetscHandlerID();
	if (scHandlerID != NULL){
		if ((error = ts3client_setPreProcessorConfigValue(scHandlerID, "voiceactivation_level", Modules::ModuleVoIP::Instance().VarVoIPVADLevel->ValueString.c_str())) != ERROR_ok) {
			returnInfo = "Error setting voice activation level. Error: " + std::to_string(error);
			return false;
		}
	}
	returnInfo = "Set Voice Activation Level to " + Modules::ModuleVoIP::Instance().VarVoIPVADLevel->ValueString;
	return true;
}
bool VariableServerEnabledUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	//TODO: Check if host, kill client too. StopTeamspeakClient();
	//TODO: Figure out why this doesn't stop the teamspeak server when setting to 0....
	if (Modules::ModuleVoIP::Instance().VarVoIPServerEnabled->ValueInt == 0){
		StopTeamspeakClient();
		StopTeamspeakServer();
	}
	returnInfo = Modules::ModuleVoIP::Instance().VarVoIPServerEnabled->ValueInt ? "VoIP Server will start when a new lobby is created" : "Disabled VoIP Auto Startup.";
	return true;
}

bool VariableEnabledUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	//TODO: Connect to lobby VOIP if changing to 1, for now this is fine because it will join next time they connet to a lobby
	if (Modules::ModuleVoIP::Instance().VarVoIPEnabled->ValueInt == 0){
		StopTeamspeakClient();
	}
	returnInfo = Modules::ModuleVoIP::Instance().VarVoIPEnabled->ValueInt ? "VoIP client will start when joining a lobby" : "Disabled VoIP.";
	return true;
}

bool CommandVoIPMutePlayer(const std::vector<std::string>& Arguments, std::string& returnInfo)
{
	if (Arguments.size() <= 0)
	{
		returnInfo = "Invalid arguments";
		return false;
	}

	std::string mutePlayerName = Arguments[0];

	auto* session = Blam::Network::GetActiveSession();
	if (!session || !session->IsEstablished())
	{
		returnInfo = "No session found, are you in a game?";
		return false;
	}

	int peerIdx = session->MembershipInfo.FindFirstPeer();
	while (peerIdx != -1)
	{
		int playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
		if (playerIdx != -1)
		{
			auto* player = &session->MembershipInfo.PlayerSessions[playerIdx];

			std::stringstream uidStream;
			uidStream << std::hex << player->Properties.Uid;
			auto uidString = uidStream.str();

			if (!Utils::String::Trim(Utils::String::ThinString(player->Properties.DisplayName)).compare(mutePlayerName) || !uidString.compare(mutePlayerName))
			{
				if (muteTeamspeakClient(Utils::String::Trim(Utils::String::ThinString(player->Properties.DisplayName))) == 0){
					returnInfo = "Toggled mute on " + mutePlayerName;
					return true;
				}
			}
		}

		peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
	}

	returnInfo = "Player " + mutePlayerName + " not found in game?";
	return false;
}

namespace Modules
{
	ModuleVoIP::ModuleVoIP() : ModuleBase("VoIP")
	{
		//Push To Talk
		VarVoIPPushToTalk = AddVariableInt("PushToTalk", "voip_ptt", "If push to talk is enabled.", eCommandFlagsArchived, 1, VariablePushToTalkUpdate);
		VarVoIPPushToTalk->ValueIntMin = 0;
		VarVoIPPushToTalk->ValueIntMax = 1;

		//Volume Modifier
		VarVoIPVolumeModifier = AddVariableInt("VolumeModifier", "voip_vm", "Modify the volume of other speakers (db)."
												"0 = no modification, negative values make the signal quieter,"
												"greater than zero boost the signal louder. High positive values = really bad audio quality."
												, eCommandFlagsArchived, 6, VariableVolumeModifierUpdate);

		VarVoIPVolumeModifier->ValueInt64Min = -15;
		VarVoIPVolumeModifier->ValueInt64Max = 30;

		//Automatic Gain Control
		VarVoIPAGC = AddVariableInt("AGC", "voip_agc", "Automatic Gain Control keeps volumes level with each other. Less really soft, and less really loud.", eCommandFlagsArchived, 1, VariableAGCUpdate);
		VarVoIPAGC->ValueIntMin = 0;
		VarVoIPAGC->ValueIntMax = 1;

		//Echo Cancellation
		VarVoIPEchoCancellation = AddVariableInt("EchoCancellation", "voip_ec", "Reduces echo over voice chat", eCommandFlagsArchived, 1, VariableEchoCancellationUpdate);
		VarVoIPEchoCancellation->ValueIntMin = 0;
		VarVoIPEchoCancellation->ValueIntMax = 1;

		//Voice Activation Detection Level
		VarVoIPVADLevel = AddVariableFloat("VoiceActivationLevel", "voip_vadlevel", "A high voice activation level means you have to speak louder into the microphone"
			"in order to start transmitting. Reasonable values range from -50 to 50. Default is -45."
			, eCommandFlagsArchived, -45.0f, VariableVADLevelUpdate);

		VarVoIPVADLevel->ValueFloatMin = -50.0f;
		VarVoIPVADLevel->ValueFloatMax = 50.0f;

		//Should we start a VoIP Server?
		VarVoIPServerEnabled = AddVariableInt("ServerEnabled", "voip_server", "Enabled or disable the VoIP Server.", eCommandFlagsArchived, 1, VariableServerEnabledUpdate);
		VarVoIPServerEnabled->ValueIntMin = 0;
		VarVoIPServerEnabled->ValueIntMax = 1;

		//Are we talking right now?
		VarVoIPTalk = AddVariableInt("Talk", "voip_Talk", "Enables or disables talking (for push to talk)", eCommandFlagsNone, 0);
		VarVoIPTalk->ValueIntMin = 0;
		VarVoIPTalk->ValueIntMax = 1;

		//Should we start a VoIP client?
		VarVoIPEnabled = AddVariableInt("Enabled", "voip_enabled", "Enabled or disable the VoIP Client.", eCommandFlagsArchived, 1, VariableEnabledUpdate);
		VarVoIPEnabled->ValueIntMin = 0;
		VarVoIPEnabled->ValueIntMax = 1;

		AddCommand("MutePlayer", "mute", "Toggles mute on a player in VoIP", eCommandFlagsNone, CommandVoIPMutePlayer, { "playername/UID The name or UID of the player to mute or unmute" });

	}
}
