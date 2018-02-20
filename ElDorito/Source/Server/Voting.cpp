#include <sstream>
#include <io.h>
#include <iostream>

#include "../Utils/Utils.hpp"
#include "Voting.hpp"
#include "VotingSystem.hpp"
#include "boost/filesystem.hpp"
#include "../Modules/ModuleGame.hpp"
#include "../Patches/Network.hpp"
#include "../Patches/Core.hpp"
#include "../ElDorito.hpp"

namespace Server::Voting
{
	std::vector<AbstractVotingSystem *> VotingSystems;
	VotingSystem votingSystem;
	VetoSystem vetoSystem;

	//Callback for when the loading screen back to the main menu finishes. We use this to determine when to start a new vote.
	void MapLoadedCallback(const char *mapPath)
	{
		if(Blam::Network::GetActiveSession()->Parameters.GetSessionMode() == 1 && Blam::Network::GetNetworkMode() == 3){
			StartNewVote();
		}
	}

	//Reset the state of voting in case people start a game manually. 
	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState)
	{
		switch (newState)
		{
		case Blam::Network::eLifeCycleStateStartGame:
		{

			if (Modules::ModuleServer::Instance().VarServerTeamShuffleEnabled->ValueInt == 1)
				Modules::CommandMap::Instance().ExecuteCommand("Server.ShuffleTeams");

			for (auto elem : VotingSystems)
			{
				if (elem->isEnabled()) {
					elem->Reset();
				}
			}
			break;
		}

		case Blam::Network::eLifeCycleStateNone:
		{
			for (auto elem : VotingSystems)
			{
				if (elem->isEnabled()) {
					elem->Reset();
				}
			}
			break;
		}
		}
	}

	void Init()
	{
		InitializePackets();
		Patches::Core::OnMapLoaded(MapLoadedCallback);
		Patches::Network::OnLifeCycleStateChanged(LifeCycleStateChanged);

		VotingSystems.push_back((AbstractVotingSystem*)&votingSystem);
		VotingSystems.push_back((AbstractVotingSystem*)&vetoSystem);
			
		for (auto elem : VotingSystems)
		{
			elem->Init();
		}
	}
	bool ReloadVotingJson() {
		return votingSystem.ReloadVotingJson(Modules::ModuleServer::Instance().VarVotingJsonPath->ValueString);
	}
	bool ReloadVetoJson() {
		return vetoSystem.ReloadVotingJson(Modules::ModuleServer::Instance().VarVetoJsonPath->ValueString);
	}
	void Tick() 
	{	
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost()))
			return;

		for (auto elem : VotingSystems)
		{		
			if (elem->isEnabled()) {
				elem->Tick();
			}
		}
	}
		
	//This allows players who join a vote in progress to be able to see the options and vote
	void PlayerJoinedVoteInProgress(int playerIndex)
	{
		//if we aren't in a vote or voting isn't enabled, then do nothing
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost() && Blam::Network::GetLobbyType() == 2))
			return;

		auto peerIdx = session->MembershipInfo.GetPlayerPeer(playerIndex);
		if (peerIdx != session->MembershipInfo.LocalPeerIndex)
		{

			for (auto elem : VotingSystems)
			{
				if (elem->isEnabled()) {
					elem->GenerateVotingOptionsMessage(peerIdx);
					if(elem->ShouldSendVotingOptions())
						elem->SendVoteCountsToEveryone();
				}
			}
		}
			
	}

	//Starts a new vote
	void StartNewVote() 
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost() && Blam::Network::GetLobbyType() == 2))
			return;

		for (auto elem : VotingSystems)
		{
			if (elem->isEnabled()) {
				elem->NewVote();
			}
		}
	}
	void CancelVoteInProgress()
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost()))
			return;

		for (auto elem : VotingSystems)
		{
			if (elem->isEnabled()) {
				elem->Reset();
			}
		}
			
			
	}

	void LogVote(const VotingMessage &message, std::string name)
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost()))
			return;

		for (auto elem : VotingSystems)
		{
			if (elem->isEnabled()) {
				elem->LogVote(message, name);
			}
		}
	}
}
