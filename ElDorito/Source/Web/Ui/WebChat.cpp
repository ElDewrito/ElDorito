#include "WebChat.hpp"
#include "ScreenLayer.hpp"
#include "../../Blam/BlamPlayers.hpp"
#include "../../Patches/Input.hpp"
#include "../../Server/ServerChat.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../Utils/String.hpp"

#include <iomanip>

using namespace Blam::Input;

namespace
{
	class GameChatHandler : public Server::Chat::ChatHandler
	{
	public:
		void MessageSent(int senderPeer, Server::Chat::ChatMessage *message, bool *ignore) override;
		void MessageReceived(const Server::Chat::ChatMessage &message) override;
	};

	void OnGameInputUpdated();
}

namespace Web::Ui::WebChat
{
	void Init()
	{
		Server::Chat::AddHandler(std::make_shared<GameChatHandler>());
		Patches::Input::RegisterDefaultInputHandler(OnGameInputUpdated);
	}

	void Show(bool teamChat)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		jsonWriter.StartObject();
		jsonWriter.Key("teamChat");
		jsonWriter.Bool(teamChat);
		jsonWriter.Key("captureInput");
		jsonWriter.Bool(true);
		jsonWriter.EndObject();

		ScreenLayer::Show("chat", jsonBuffer.GetString());
	}

	void Hide()
	{
		ScreenLayer::Hide("chat");
	}
}

namespace
{
	void GameChatHandler::MessageSent(int senderPeer, Server::Chat::ChatMessage *message, bool *ignore)
	{

	}

	void GameChatHandler::MessageReceived(const Server::Chat::ChatMessage &message)
	{
		rapidjson::StringBuffer jsonBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);

		jsonWriter.StartObject();
		jsonWriter.Key("message");
		jsonWriter.String(std::string(message.Body).c_str());

		jsonWriter.Key("sender");
		jsonWriter.String(Server::Chat::GetSenderName(message).c_str());

		char* chatType;

		switch (message.Type) {
			case Server::Chat::ChatMessageType::Global:
				chatType = "GLOBAL";
				break;
			case Server::Chat::ChatMessageType::Team:
				chatType = "TEAM";
				break;
			case Server::Chat::ChatMessageType::Whisper:
				chatType = "WHISPER";
				break;
			case Server::Chat::ChatMessageType::Server:
				chatType = "SERVER";
				break;
			default:
				chatType = "UNKNOWN";
				break;
		}

		jsonWriter.Key("chatType");
		jsonWriter.String(chatType);

		if (message.Type != Server::Chat::ChatMessageType::Server) {
			auto session = Blam::Network::GetActiveSession();
			if (session) {
				auto player = session->MembershipInfo.PlayerSessions[message.SenderPlayer];
				jsonWriter.Key("teamIndex");
				jsonWriter.Int(player.Properties.TeamIndex);

				char uid[17];
				Blam::Players::FormatUid(uid, player.Properties.Uid);

				jsonWriter.Key("UID");
				jsonWriter.String(uid);

				std::stringstream color;
				color << "#" << std::setw(6) << std::setfill('0') << std::hex << player.Properties.Customization.Colors[Blam::Players::ColorIndices::Primary];
				jsonWriter.Key("color");
				jsonWriter.String(color.str().c_str());
			}
			jsonWriter.Key("hasTeams");
			jsonWriter.Bool(session->HasTeams());
		}
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("chat", jsonBuffer.GetString(), true);
	}

	void OnGameInputUpdated()
	{
		BindingsTable bindings;
		GetBindings(0, &bindings);

		if (GetKeyTicks(bindings.PrimaryKeys[eGameActionTeamChat], eInputTypeUi) == 1 || GetKeyTicks(bindings.SecondaryKeys[eGameActionTeamChat], eInputTypeUi) == 1)
			Web::Ui::WebChat::Show(true);

		if (GetKeyTicks(bindings.PrimaryKeys[eGameActionGeneralChat], eInputTypeUi) == 1 || GetKeyTicks(bindings.SecondaryKeys[eGameActionGeneralChat], eInputTypeUi) == 1)
			Web::Ui::WebChat::Show(false);
	}
}