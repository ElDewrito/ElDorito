#include "ModuleVoIP.hpp"
#include <sstream>
#include "../Patches/Core.hpp"
#include "../Patches/Ui.hpp"
#include "../Patches/Input.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"

namespace
{
	static bool ready = false;
	static bool isMainMenu = false;
	static bool isChatting = false;

	bool UpdateVoip(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!ready)
			return false;

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();
		writer.Key("PTT_Enabled");
		writer.Int(Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt);
		writer.Key("MicrophoneID");
		writer.String(Modules::ModuleVoIP::Instance().VarMicrophone->ValueString.c_str());
		writer.Key("EchoCancellation");
		writer.Int(Modules::ModuleVoIP::Instance().VarEchoCancellation->ValueInt);
		writer.Key("AGC");
		writer.Int(Modules::ModuleVoIP::Instance().VarAGC->ValueInt);
		writer.Key("NoiseSupress");
		writer.Int(Modules::ModuleVoIP::Instance().VarNoiseSupress->ValueInt);
		writer.Key("Enabled");
		writer.Int(Modules::ModuleVoIP::Instance().VarVoipEnabled->ValueInt);
		writer.EndObject();

		Web::Ui::ScreenLayer::Notify("voip-settings", buffer.GetString(), true);

		//notify ptt state in case ptt was enabled or disabled
		std::stringstream ss;
		if (Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt == 1)
			ss << "{\"talk\":" << Modules::ModuleVoIP::Instance().VarPTT->ValueInt << "}";
		else
			ss << "{\"talk\":" << 1 << "}";
		Web::Ui::ScreenLayer::Notify("voip-ptt", ss.str(), true);

		if (Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt == 0)
			Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Speaking);
		else
			Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Available);

		returnInfo = "";
		return true;
	}

	void RendererStarted(const char* map)
	{
		if (std::string(map).find("mainmenu") == std::string::npos)
		{
			isMainMenu = false;

			//initial voip icons when maps loads
			if (Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt == 0)
				Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Speaking);
			else
				Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Available);
		}
		else
			isMainMenu = true;
		ready = true;
	}

	void OnGameInputUpdated()
	{
		Blam::Input::BindingsTable bindings;
		GetBindings(0, &bindings);
		
		if (Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt == 1)
		{
			//keyboard/controller in-game
			if (isChatting && Blam::Input::GetActionState(Blam::Input::eGameActionVoiceChat)->Ticks == 0)
			{
				isChatting = false;
				if (!isMainMenu)
					Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Available);
				Web::Ui::ScreenLayer::Notify("voip-ptt", "{\"talk\":0}", true);
			}
			else if (!isChatting && Blam::Input::GetActionState(Blam::Input::eGameActionVoiceChat)->Ticks == 1)
			{
				isChatting = true;
				if (!isMainMenu)
					Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Speaking);
				Web::Ui::ScreenLayer::Notify("voip-ptt", "{\"talk\":1}", true);
			}

			
		}
	}

	void OnUiInputUpdated()
	{
		auto isUsingController = *(bool*)0x0244DE98;

		//controller in lobby
		if (isMainMenu && isUsingController)
		{
			if (isChatting && Blam::Input::GetActionState(Blam::Input::eGameActionUiY)->Ticks == 0) //hard code Y since its unused in main menu
			{
				isChatting = false;
				Web::Ui::ScreenLayer::Notify("voip-ptt", "{\"talk\":0}", true);
			}
			else if (!isChatting && Blam::Input::GetActionState(Blam::Input::eGameActionUiY)->Ticks == 1)
			{
				isChatting = true;
				Web::Ui::ScreenLayer::Notify("voip-ptt", "{\"talk\":1}", true);
			}
		}
	}
}

namespace Modules
{
	ModuleVoIP::ModuleVoIP() : ModuleBase("VoIP")
	{
		VarVoipEnabled = AddVariableInt("Enabled", "enabled", "Toggle voip on or off", eCommandFlagsArchived, 1, UpdateVoip);
		VarVoipEnabled->ValueIntMin = 0;
		VarVoipEnabled->ValueIntMax = 1;

		VarPTTEnabled = AddVariableInt("PTT_Enabled", "ptt_enabled", "Enable PTT(1) or voice activation(0)", eCommandFlagsArchived, 1, UpdateVoip);
		VarPTTEnabled->ValueIntMin = 0;
		VarPTTEnabled->ValueIntMax = 1;

		VarMicrophone = AddVariableString("MicrophoneID", "microphoneid", "microphone id to use for voip, blank is default device", eCommandFlagsArchived, "", UpdateVoip);

		VarEchoCancellation = AddVariableInt("EchoCancelation", "echocancellation", "Toggle echo cancellation", eCommandFlagsArchived, 1, UpdateVoip);
		VarEchoCancellation->ValueIntMin = 0;
		VarEchoCancellation->ValueIntMax = 1;

		VarAGC = AddVariableInt("AGC", "agc", "Toggle automatic gain control", eCommandFlagsArchived, 1, UpdateVoip);
		VarAGC->ValueIntMin = 0;
		VarAGC->ValueIntMax = 1;

		VarNoiseSupress = AddVariableInt("NoiseSupress", "noisesupress", "Toggle noise supression", eCommandFlagsArchived, 1, UpdateVoip);
		VarNoiseSupress->ValueIntMin = 0;
		VarNoiseSupress->ValueIntMax = 1;


		Patches::Core::OnMapLoaded(RendererStarted);
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
		Patches::Input::RegisterMenuUIInputHandler(OnUiInputUpdated);
	}
}