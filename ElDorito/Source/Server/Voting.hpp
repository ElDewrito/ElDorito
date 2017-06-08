#pragma once
#include "../Server/VotingPackets.hpp"

namespace Server
{
	namespace Voting
	{

		void Init();
		void Tick();
		void PlayerJoinedVoteInProgress(int playerIndex);
		void CancelVoteInProgress();
		void StartNewVote();
		void LogVote(const VotingMessage &message, std::string name);

	}
}