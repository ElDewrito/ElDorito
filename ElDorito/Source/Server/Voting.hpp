#pragma once
#include "../Server/VotingPackets.hpp"

namespace Server::Voting
{
	void Init();
	void Tick();
	bool ReloadVetoJson(std::string filename);
	bool ReloadVotingJson(std::string filenam);
	void PlayerJoinedVoteInProgress(int playerIndex);
	void CancelVoteInProgress();
	void StartNewVote();
	void LogVote(const VotingMessage &message, std::string name);
}
