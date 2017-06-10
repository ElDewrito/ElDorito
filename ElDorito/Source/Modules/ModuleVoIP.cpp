#include "ModuleVoIP.hpp"
#include <sstream>
#include "../Patches/Core.hpp"
#include "../Patches/Ui.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/stringbuffer.h"

namespace
{
	static bool ready = false;
	static bool isMainMenu = false;
	bool PttToggle(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!ready)
			return false;

		std::stringstream ss;
		if (Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt == 1)
			ss << "{\"talk\":" << Modules::ModuleVoIP::Instance().VarPTT->ValueInt << "}";
		else
			ss << "{\"talk\":" << 1 << "}";
		Web::Ui::ScreenLayer::Notify("voip-ptt", ss.str(), true);

		if (!isMainMenu && Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt == 1)
		{
			if (Modules::ModuleVoIP::Instance().VarPTT->ValueInt == 1)
				Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Speaking);
			else
				Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Available);
		}

		returnInfo = "";
		return true;
	}

	bool UpdateVoip(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (!ready)
			return false;

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();
		writer.Key("PTT_Enabled");
		writer.Int(Modules::ModuleVoIP::Instance().VarPTTEnabled->ValueInt);

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
			{
				if (Modules::ModuleVoIP::Instance().VarPTT->ValueInt == 1)
					Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Speaking);
				else
					Patches::Ui::SetVoiceChatIcon(Patches::Ui::VoiceChatIcon::Available);
			}
		}
		else
			isMainMenu = true;
		ready = true;
	}
}

namespace Modules
{
	ModuleVoIP::ModuleVoIP() : ModuleBase("VoIP")
	{
		VarPTT = AddVariableInt("Talk", "talk", "Bind with '+' to control push to talk. ex 'bind v +voip.talk'", eCommandFlagsNone, 0, PttToggle);
		VarPTT->ValueIntMin = 0;
		VarPTT->ValueIntMax = 1;

		VarPTTEnabled = AddVariableInt("PTT_Enabled", "ptt_enabled", "Enable PTT(1) or voice activation(0)", eCommandFlagsArchived, 1, UpdateVoip);
		VarPTTEnabled->ValueIntMin = 0;
		VarPTTEnabled->ValueIntMax = 1;

		Patches::Core::OnMapLoaded(RendererStarted);
	}
}
