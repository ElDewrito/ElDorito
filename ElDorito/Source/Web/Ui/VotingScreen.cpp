#include "VotingScreen.hpp"
#include "ScreenLayer.hpp"
#include "../../Server/VotingPackets.hpp"
#include "../../Patches/Network.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include <unordered_map>
#include "../../Patches/Input.hpp"

using namespace Server::Voting;

namespace
{
	void OnUiInputUpdate();

	std::unordered_map<int, std::string> MapNames =
	{
		{ 320, "guardian" },
		{ 340, "riverworld" },
		{ 705, "s3d_avalanche" },
		{ 703, "s3d_edge" },
		{ 700, "s3d_reactor" },
		{ 31, "s3d_turf" },
		{ 390, "cyberdyne" },
		{ 380, "chill" },
		{ 310, "deadlock" },
		{ 410, "bunkerworld" },
		{ 400, "shrine" },
		{ 30, "zanzibar" },
	};

	bool currentlyVoting = false;
}

class VotingOutputHandler : public VotingMessageHandler
{
public:
	VotingOutputHandler(){}

	void MessageReceived(const VotingMessage &message) override
	{
		if (message.Type == VotingMessageType::VotingOptions)
		{
			rapidjson::StringBuffer jsonBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
			jsonWriter.StartObject();

			jsonWriter.Key("timeRemaining");
			jsonWriter.Int(message.voteTime);
			jsonWriter.Key("votingOptions");
			jsonWriter.StartArray();

			for (int i = 0; i < 5; i++){

				std::string imageName;
				auto it = MapNames.find(message.votingOptions[i].mapId);
				if (it != MapNames.end())
					imageName = it->second;

				jsonWriter.StartObject();
				jsonWriter.Key("index");
				jsonWriter.Int(i + 1);

				jsonWriter.Key("image");
				jsonWriter.String(imageName.c_str());
				jsonWriter.Key("mapname");
				jsonWriter.String(message.votingOptions[i].mapName);
				jsonWriter.Key("typename");
				jsonWriter.String(message.votingOptions[i].typeName);

				jsonWriter.EndObject();

			}

			jsonWriter.EndArray();
			jsonWriter.EndObject();
			Web::Ui::Voting::Show();
			Web::Ui::ScreenLayer::Notify("VotingOptionsUpdated", jsonBuffer.GetString(), true);
			currentlyVoting = true;
		}
		else if (message.Type == VotingMessageType::VetoOption) {

			rapidjson::StringBuffer jsonBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
			jsonWriter.StartObject();

			jsonWriter.Key("timeRemaining");
			jsonWriter.Int(message.voteTime);
			jsonWriter.Key("votesNeededToPass");
			jsonWriter.Int(message.votesNeededToPass);
			jsonWriter.Key("vetoOption");
			jsonWriter.StartObject();


			std::string imageName;
			auto it = MapNames.find(message.votingOptions[0].mapId);
			if (it != MapNames.end())
				imageName = it->second;


			jsonWriter.Key("image");
			jsonWriter.String(imageName.c_str());
			jsonWriter.Key("mapname");
			jsonWriter.String(message.votingOptions[0].mapName);
			jsonWriter.Key("typename");
			jsonWriter.String(message.votingOptions[0].typeName);
			jsonWriter.Key("canveto");
			jsonWriter.Bool(message.votingOptions[0].canVeto);

			jsonWriter.EndObject();
			jsonWriter.EndObject();
			Web::Ui::Voting::Show();
			Web::Ui::ScreenLayer::Notify("VetoOptionsUpdated", jsonBuffer.GetString(), true);
			currentlyVoting = true;
		}
		else if (message.Type == VotingMessageType::Winner)
		{
			rapidjson::StringBuffer jsonBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
			jsonWriter.StartObject();
			jsonWriter.Key("Winner");
			jsonWriter.Int(message.winner);
			jsonWriter.Key("timeUntilGameStart");
			jsonWriter.Int(message.voteTime);
			jsonWriter.EndObject();
			Web::Ui::ScreenLayer::Notify("Winner", jsonBuffer.GetString(), true);

		}
		else if (message.Type == VotingMessageType::VoteTally)
		{
			rapidjson::StringBuffer jsonBuffer;
			rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonBuffer);
			jsonWriter.StartObject();
			jsonWriter.Key("votesNeededToPass");
			jsonWriter.Int(message.votesNeededToPass);

			jsonWriter.Key("voteCounts");
			jsonWriter.StartArray();
			for (int i = 0; i < 5; i++){
				jsonWriter.StartObject();
				jsonWriter.Key("OptionIndex");
				jsonWriter.Int(i + 1);
				jsonWriter.Key("Count");
				jsonWriter.Int(message.votes[i]);
				jsonWriter.EndObject();

			}

			jsonWriter.EndArray();
			jsonWriter.EndObject();
			Web::Ui::ScreenLayer::Notify("VoteCountsUpdated", jsonBuffer.GetString(), true);

		}
	}
};

namespace
{
	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState)
	{
		switch (newState)
		{
		case Blam::Network::eLifeCycleStateStartGame:
			currentlyVoting = false;
			Web::Ui::Voting::Hide();
			break;
		case Blam::Network::eLifeCycleStateNone:
			currentlyVoting = false;
			Web::Ui::Voting::Hide();
			break;
		case Blam::Network::eLifeCycleStateLeaving:
			currentlyVoting = false;
			Web::Ui::Voting::Hide();
			break;
		}
	}
}

namespace Web::Ui::Voting
{
	void Init()
	{
		Server::Voting::AddMessageHandler(std::make_shared<VotingOutputHandler>());
		Patches::Network::OnLifeCycleStateChanged(LifeCycleStateChanged);
		Patches::Input::RegisterMenuUIInputHandler(OnUiInputUpdate);
	}

	void Show()
	{
		ScreenLayer::Show("voting", "{}");
	}

	void Hide()
	{
		ScreenLayer::Hide("voting");
	}
}

namespace
{
	void OnUiInputUpdate()
	{
		using namespace Blam::Input;

		if (currentlyVoting)
		{
			auto action = GetActionState(Blam::Input::eGameActionUiY);
			if (!(action->Flags & eActionStateFlagsHandled) && action->Ticks == 1)
			{
				action->Ticks |= eActionStateFlagsHandled;
				Web::Ui::ScreenLayer::Show("voting", "{}");
			}
		}
	}
}
