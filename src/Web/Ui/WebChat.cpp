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

namespace Web
{
	namespace Ui
	{
		namespace WebChat
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

		jsonWriter.Key("teamChat");
		jsonWriter.Bool(message.Type == Server::Chat::ChatMessageType::Team);

		auto session = Blam::Network::GetActiveSession();
		if (session) {
			auto player = session->MembershipInfo.PlayerSessions[message.SenderPlayer];
			jsonWriter.Key("teamIndex");
			jsonWriter.Int(player.Properties.TeamIndex);

			std::string uidStr;
			Utils::String::BytesToHexString(&player.Properties.Uid, sizeof(uint64_t), uidStr);
			jsonWriter.Key("UID");
			jsonWriter.String(uidStr.c_str());

			std::stringstream color;
			color << "#" << std::setw(6) << std::setfill('0') << std::hex << player.Properties.Customization.Colors[Blam::Players::ColorIndices::Primary];
			jsonWriter.Key("color");
			jsonWriter.String(color.str().c_str());
		}
		jsonWriter.EndObject();

		Web::Ui::ScreenLayer::Notify("chat", jsonBuffer.GetString(), true);
	}

	void OnGameInputUpdated()
	{
		//TODO: Don't hardocde
		if (GetKeyTicks(eKeyCodeT, eInputTypeUi) == 1)
			Web::Ui::WebChat::Show(true);

		if (GetKeyTicks(eKeyCodeY, eInputTypeUi) == 1)
			Web::Ui::WebChat::Show(false);
	}
}