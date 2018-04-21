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

	void ShuffleTeamsCommand::doOnVotePass(std::string name)
	{
		Server::Chat::SendServerMessage("Final vote cast by " + name + ". Vote has passed.");
		Modules::CommandMap::Instance().ExecuteCommand("server.shuffleteams");
	}
	bool ShuffleTeamsCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandShuffleTeamsEnabled->ValueInt == 1);
	}
	void ShuffleTeamsCommand::doOnVoteFail()
	{
		Server::Chat::SendServerMessage("Vote has not passed.");
	}
	void ShuffleTeamsCommand::doOnVoteStart(std::string starterName)
	{
		Server::Chat::SendServerMessage(starterName + " has started a vote to shuffle the teams. " + std::to_string(votesNeeded) + " vote" + ((votesNeeded > 1) ? "s" : "") + " needed to pass. Type !yes to vote");
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

	void EndGameCommand::doOnVotePass(std::string name)
	{
		Server::Chat::SendServerMessage("Final vote cast by " + name + ". Vote has passed.");
		Modules::CommandMap::Instance().ExecuteCommand("game.end");
	}
	bool EndGameCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandEndGameEnabled->ValueInt == 1);
	}
	void EndGameCommand::doOnVoteFail()
	{
		Server::Chat::SendServerMessage("Vote has not passed.");
	}
	void EndGameCommand::doOnVoteStart(std::string starterName)
	{
		Server::Chat::SendServerMessage(starterName + " has started a vote to end the game. " + std::to_string(votesNeeded) + " vote" + ((votesNeeded > 1) ? "s" : "") + " needed to pass. Type !yes to vote.");
	}

	bool EndGameCommand::isValidArgument(std::string s, std::string& returnInfo){ return true; }

	KickIndexCommand::KickIndexCommand() : AbstractChatCommand("kickIndex", "Starts a vote to kick a player by index.") {}

	void KickIndexCommand::doOnVotePass(std::string name)
	{
		Server::Chat::SendServerMessage("Final vote cast by " + name + ". " + playerName + " has been kicked and temporarily banned.");
		Modules::CommandMap::Instance().ExecuteCommand("server.KickTempBanPlayer " + playerName);

		playerName = "";
	}
	void KickIndexCommand::doOnVoteFail()
	{
		playerName = "";
		Server::Chat::SendServerMessage("Vote has not passed.");

	}
	bool KickIndexCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandKickPlayerEnabled->ValueInt == 1);
	}
	void KickIndexCommand::doOnVoteStart(std::string starterName)
	{
		Server::Chat::SendServerMessage(starterName + " has started a kick vote for: \"" + playerName + "\"; Type !yes to vote. " + std::to_string(votesNeeded) + " vote" + ((votesNeeded > 1) ? "s" : "") + " needed to kick.");
	}

	bool KickIndexCommand::isValidArgument(std::string s, std::string& returnInfo)
	{
		if (s.empty()) {
			returnInfo = "You must enter a valid player index to use this command.";
			return false;
		}
		int indexToKick = -1;
		try {
			indexToKick = std::stoi(s);
		}
		catch (...) {}

		if (indexToKick < 0 || indexToKick > 15)
		{
			returnInfo = "Invalid index";
			return false;
		}

		auto* session = Blam::Network::GetActiveSession();
		if(indexToKick == starterIndex)
		{
			returnInfo = "You cannot kick yourself.";
			return false;
		}
		else if (indexToKick == session->MembershipInfo.HostPeerIndex && !ElDorito::Instance().IsDedicated())
		{
			returnInfo = "You cannot kick the host.";
			return false;
		}

		auto player = &session->MembershipInfo.PlayerSessions[indexToKick];
		std::string kickPlayerName = Utils::String::ThinString(player->Properties.DisplayName);

		if (kickPlayerName.empty())
		{
			returnInfo = "Could not find player.";
			return false;
		}

		playerName = kickPlayerName;

		return true;
	}

	KickPlayerCommand::KickPlayerCommand() : AbstractChatCommand("kick", "Starts a vote to kick a player."){}

	void KickPlayerCommand::doOnVotePass(std::string name)
	{
		Server::Chat::SendServerMessage("Final vote cast by " + name + ". " + playerName + " has been kicked and temporarily banned.");
		Modules::CommandMap::Instance().ExecuteCommand("server.KickTempBanPlayer " + playerName);

		playerName = "";
	}
	void KickPlayerCommand::doOnVoteFail()
	{
		playerName = "";
		Server::Chat::SendServerMessage("Vote has not passed.");
	}
	bool KickPlayerCommand::isEnabled()
	{
		return (Modules::ModuleServer::Instance().VarChatCommandKickPlayerEnabled->ValueInt == 1);
	}

	void KickPlayerCommand::doOnVoteStart(std::string starterName)
	{
		Server::Chat::SendServerMessage(starterName + " has started a kick vote for: \"" + playerName + "\"; Type !yes to vote. " + std::to_string(votesNeeded) + " vote" + ((votesNeeded > 1) ? "s" : "") + " needed to kick.");
	}


	bool KickPlayerCommand::isValidArgument(std::string s, std::string& returnInfo)
	{
		if (s.empty()) {
			returnInfo = "You must enter a name to use this command.";
			return false;
		}
		else if (s.length() > 16) {
			returnInfo = "Player not found.";
			return false;
		}
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
		else if (playerToKickIdx == starterIndex)
		{
			returnInfo = "You cannot kick yourself.";
			return false;
		}
		else if (playerToKickIdx == 0 && !ElDorito::Instance().IsDedicated())
		{
			returnInfo = "You cannot kick the host.";
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
		auto &membership = Blam::Network::GetActiveSession()->MembershipInfo;
		starterIndex = membership.GetPeerPlayer(sender);
		if (!isValidArgument(argument, returnInfo))
		{
			Server::Chat::SendServerMessage(returnInfo, sender);
			return;
		}
		
		//GET UID
		uint64_t uid = membership.PlayerSessions[starterIndex].Properties.Uid;
		std::string name = Utils::String::ThinString(membership.PlayerSessions[starterIndex].Properties.DisplayName);

		//check if this player has recently started a vote
		if (ChatCommands::addToVoteTimes(uid))
		{
			int numPlayers = findNumberOfPlayersInGame();

			votesNeeded = 1 + (((numPlayers - 1) * Modules::ModuleServer::Instance().VarServerVotePassPercentage->ValueInt) / 100);
			currentlyVoting = true;
			time(&voteTimeStarted);

			doOnVoteStart(name);
			processVote(sender, "yes");
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
			std::string name = Utils::String::ThinString(membership.PlayerSessions[membership.GetPeerPlayer(sender)].Properties.DisplayName);

			if (uid == 0)
				return;

			if (std::find(yesVoters.begin(), yesVoters.end(), uid) != yesVoters.end()) {
				Server::Chat::SendServerMessage("You cannot vote twice.", sender);
				return;
			}

			yesVoters.push_back(uid);

			if (yesVoters.size() >= votesNeeded)
			{
				doOnVotePass(name);
				endVoting();
			}
			else
			{
				size_t votesRemaining = (votesNeeded - yesVoters.size());
				Server::Chat::SendServerMessage("Vote cast by \"" + name + "\". Remaining votes needed: " + std::to_string(votesRemaining));
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