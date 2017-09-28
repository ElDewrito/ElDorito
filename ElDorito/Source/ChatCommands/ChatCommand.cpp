#include "ChatCommand.hpp"
#include "ChatCommandMap.hpp"
#include "../Server/ServerChat.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Utils/Utils.hpp"
#include "../Eldorito.hpp"
#include "../Utils/Logger.hpp"


namespace ChatCommands
{
	int findNumberOfPlayersInGame()
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
			return 0;

		int numberOfPlayers = 0;
		auto membership = &session->MembershipInfo;
		for (auto peerIdx = membership->FindFirstPeer(); peerIdx >= 0; peerIdx = membership->FindNextPeer(peerIdx))
		{
			auto playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
			if (playerIdx == -1 || !session->MembershipInfo.Peers[peerIdx].IsEstablished())
				continue;

			numberOfPlayers++;
		}
		return numberOfPlayers;
	}

	ShuffleTeamsCommand::ShuffleTeamsCommand() : AbstractChatCommand("shuffleTeams", "Starts a vote to shuffle the teams. Type !yes to vote."){}

	void ShuffleTeamsCommand::doOnVotePass()
	{
		Server::Chat::SendServerMessage("Vote Has Passed.");
		Modules::CommandMap::Instance().ExecuteCommand("server.shuffleteams");
	}
	bool ShuffleTeamsCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandShuffleTeamsEnabled->ValueInt == 1);
	}
	void ShuffleTeamsCommand::doOnVoteFail()
	{
		Server::Chat::SendServerMessage("Vote Has Not Passed.");
	}
	void ShuffleTeamsCommand::doOnVoteStart()
	{
		Server::Chat::SendServerMessage("Vote started to shuffle the teams. " + std::to_string(votesNeeded) + " votes needed to pass. Type !yes to vote");
	}

	bool ShuffleTeamsCommand::isValidArgument(std::string s, std::string& returnInfo)
	{ 
		if (Blam::Network::GetActiveSession()->HasTeams())
			return true; 
		else {
			returnInfo = "Only team games can be shuffled.";
			return false;
		}
	}

	EndGameCommand::EndGameCommand() : AbstractChatCommand("endGame", "Starts a vote to end the current game. Type !yes to vote."){}

	void EndGameCommand::doOnVotePass()
	{
		Server::Chat::SendServerMessage("Vote Has Passed");
		Modules::CommandMap::Instance().ExecuteCommand("game.stop");
	}
	bool EndGameCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandEndGameEnabled->ValueInt == 1);
	}
	void EndGameCommand::doOnVoteFail()
	{
		Server::Chat::SendServerMessage("Vote Has Not Passed.");
	}
	void EndGameCommand::doOnVoteStart()
	{
		Server::Chat::SendServerMessage("Vote started to end the game. " + std::to_string(votesNeeded) + " votes needed to pass. Type !yes to vote");
	}

	bool EndGameCommand::isValidArgument(std::string s, std::string& returnInfo){ return true; }

	KickIndexCommand::KickIndexCommand() : AbstractChatCommand("kickIndex", "Starts a vote to kick a player by index.") {}

	void KickIndexCommand::doOnVotePass()
	{
		Server::Chat::SendServerMessage("Vote Has Passed. " + playerName + " has been kicked and temporarily banned.");
		Modules::CommandMap::Instance().ExecuteCommand("server.KickTempBanPlayer " + playerName);

		playerName = "";
	}
	void KickIndexCommand::doOnVoteFail()
	{
		playerName = "";
		Server::Chat::SendServerMessage("Vote Has Not Passed.");

	}
	bool KickIndexCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandKickPlayerEnabled->ValueInt == 1);
	}
	void KickIndexCommand::doOnVoteStart()
	{
		Server::Chat::SendServerMessage("Kick vote started for: \"" + playerName + "\"; Type !Yes to vote. " + std::to_string(votesNeeded) + " votes needed to kick.");
	}

	bool KickIndexCommand::isValidArgument(std::string s, std::string& returnInfo)
	{
		int indexToKick = -1;
		try {
			indexToKick = std::stoi(s);
		}
		catch (const std::invalid_argument& e) {
			Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Info, "Error: %s", e.what());
		}
		if (indexToKick < 0 || indexToKick > 15)
		{
			returnInfo = "Invalid Index";
			return false;
		}

		auto* session = Blam::Network::GetActiveSession();
		if (indexToKick == session->MembershipInfo.HostPeerIndex && !ElDorito::Instance().IsDedicated())
		{
			returnInfo = "You cannot kick the host";
			return false;
		}

		auto player = &session->MembershipInfo.PlayerSessions[indexToKick];
		std::string kickPlayerName = Utils::String::ThinString(player->Properties.DisplayName);

		if (kickPlayerName.empty())
		{
			returnInfo = "Could Not find player";
			return false;
		}

		playerName = kickPlayerName;

		return true;
	}

	KickPlayerCommand::KickPlayerCommand() : AbstractChatCommand("kick", "Starts a vote to kick a player."){}

	void KickPlayerCommand::doOnVotePass()
	{
		Server::Chat::SendServerMessage("Vote Has Passed. " + playerName + " has been kicked and temporarily banned.");
		Modules::CommandMap::Instance().ExecuteCommand("server.KickTempBanPlayer " + playerName);

		playerName = "";
	}
	void KickPlayerCommand::doOnVoteFail()
	{
		playerName = "";
		Server::Chat::SendServerMessage("Vote Has Not Passed.");
	}
	bool KickPlayerCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandKickPlayerEnabled->ValueInt == 1);
	}

	void KickPlayerCommand::doOnVoteStart()
	{
		Server::Chat::SendServerMessage("Kick vote started for: \"" + playerName + "\"; Type !Yes to vote. " + std::to_string(votesNeeded) + " votes needed to kick.");
	}


	bool KickPlayerCommand::isValidArgument(std::string s, std::string& returnInfo)
	{
		int playerToKickIdx = -1;

		auto membership = &Blam::Network::GetActiveSession()->MembershipInfo;
		for (auto peerIdx = membership->FindFirstPeer(); peerIdx >= 0; peerIdx = membership->FindNextPeer(peerIdx))
		{
			auto playerIdx = membership->GetPeerPlayer(peerIdx);
			if (playerIdx == -1)
				continue;
			auto* player = &membership->PlayerSessions[playerIdx];
			if (Utils::String::ThinString(player->Properties.DisplayName) == s){
				playerToKickIdx = playerIdx;
			}

		}

		if (playerToKickIdx <0)
		{
			returnInfo = "Player \"" + s + "\" not found.";
			return false;
		}
		if (playerToKickIdx == 0 && !ElDorito::Instance().IsDedicated())
		{
			returnInfo = "You cannot kick the host";
			return false;
		}


		playerName = s;
		return true;
	}

	AbstractChatCommand::AbstractChatCommand(std::string n, std::string d)
	{
		name = n;
		description = d;
	}

	bool AbstractChatCommand::isCurrentlyVoting()
	{
		return currentlyVoting;
	}

	void AbstractChatCommand::tick()
	{
		if (!isEnabled() || voteTimeStarted == 0)
			return;

		time_t curTime;
		time(&curTime);
		auto elapsed = curTime - voteTimeStarted;

		//Exit if we haven't used up the time yet.
		if (elapsed < Modules::ModuleServer::Instance().VarChatCommandVoteTime->ValueInt)
			return;

		doOnVoteFail();
		endVoting();
	}

	void AbstractChatCommand::endVoting()
	{
		yesVoters.clear();
		currentlyVoting = false;
		voteTimeStarted = 0;
		votesNeeded = 0;
	}

	void AbstractChatCommand::processMessage(uint8_t sender, std::string argument)
	{
		if (!isEnabled())
			return;

		std::string returnInfo;
		if (!isValidArgument(argument, returnInfo))
		{
			Server::Chat::SendServerMessage(returnInfo, sender);
			return;
		}

		//GET UID
		auto &membership = Blam::Network::GetActiveSession()->MembershipInfo;
		uint64_t uid = membership.PlayerSessions[membership.GetPeerPlayer(sender)].Properties.Uid;

		//check if this player has recently started a vote
		if (ChatCommands::addToVoteTimes(uid))
		{
			int numPlayers = findNumberOfPlayersInGame();

			votesNeeded = 1 + (((numPlayers - 1) * Modules::ModuleServer::Instance().VarServerVotePassPercentage->ValueInt) / 100);
			currentlyVoting = true;
			time(&voteTimeStarted);

			doOnVoteStart();
		}
		else
		{
			Server::Chat::SendServerMessage("You must wait to start another vote.", sender);
		}
	}


	void AbstractChatCommand::processVote(uint8_t sender, std::string message)
	{
		if (!isEnabled())
			return;

		std::transform(message.begin(), message.end(), message.begin(), ::tolower);
		if (message == "yes")
		{

			auto &membership = Blam::Network::GetActiveSession()->MembershipInfo;
			uint64_t uid = membership.PlayerSessions[membership.GetPeerPlayer(sender)].Properties.Uid;

			if (uid == 0)
				return;

			if (std::find(yesVoters.begin(), yesVoters.end(), uid) != yesVoters.end()) {
				Server::Chat::SendServerMessage("You cannot vote twice.", sender);
				return;
			}

			yesVoters.push_back(uid);

			if (yesVoters.size() >= votesNeeded)
			{
				doOnVotePass();
				endVoting();
			}
			else
			{
				size_t votesRemaining = (votesNeeded - yesVoters.size());
				Server::Chat::SendServerMessage("Vote Cast. Remaining Votes Needed: " + std::to_string(votesRemaining));
			}
		}
	}
	std::string AbstractChatCommand::getName()
	{
		return name;
	}
	std::string AbstractChatCommand::getDescription()
	{
		return description;
	}
}