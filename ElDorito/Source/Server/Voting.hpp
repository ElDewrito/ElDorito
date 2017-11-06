#pragma once
#include "../Server/VotingPackets.hpp"

namespace Server::Voting
{
	void Init();
	void Tick();
	bool ReloadVetoJson();
	bool ReloadVotingJson();
	void PlayerJoinedVoteInProgress(int playerIndex);
	void CancelVoteInProgress();
	void StartNewVote();
	void LogVote(const VotingMessage &message, std::string name);
}
