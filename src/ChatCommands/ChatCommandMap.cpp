#include "ChatCommandMap.hpp"
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp> 

#include "ChatCommand.hpp"
#include "../Server/BanList.hpp"
#include "../Utils/Utils.hpp"
#include "../Patches/Events.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Utils/Logger.hpp"
#include "../CommandMap.hpp"
#include "../Patches/Network.hpp"



namespace ChatCommands
{
	std::vector<AbstractChatCommand *> Commands;
	bool chatCommandsActive; // they are only allowed in-game, not in the lobby
	std::vector<std::string> helpText = std::vector <std::string> {};
	std::map<uint64_t, time_t> voteTimes = std::map<uint64_t, time_t>{};

	KickPlayerCommand kickPlayerCommand;
	KickIndexCommand kickIndexCommand;
	EndGameCommand endGameCommand;
	ShuffleTeamsCommand shuffleTeamsCommand;


	void OnEvent(Blam::DatumIndex player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)
	{

		if (event->NameStringId == 262221) //Game Ended event
		{
			Server::TempBanList::Instance().decrementDuration();
			voteTimes.clear();
			chatCommandsActive = false;

			for (auto elem : Commands)
			{
				if (elem->isCurrentlyVoting())
				{
					elem->doOnVoteFail();
					elem->endVoting();
				}

			}
		}
	}
	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState)
	{
		auto* session = Blam::Network::GetActiveSession();
		switch (newState)
		{
		case Blam::Network::eLifeCycleStateStartGame:
			chatCommandsActive = true;
			break;
				
		}
		
	}

	void Init()
	{
		Patches::Events::OnEvent(OnEvent);
		Patches::Network::OnLifeCycleStateChanged(LifeCycleStateChanged);

		Commands.push_back((AbstractChatCommand*) &kickPlayerCommand);
		Commands.push_back((AbstractChatCommand*) &kickIndexCommand);
		Commands.push_back((AbstractChatCommand*) &endGameCommand);
		Commands.push_back((AbstractChatCommand*) &shuffleTeamsCommand);

		for (auto elem : Commands)
		{
			if (elem->isEnabled())
				helpText.push_back("!" +elem->getName() + " - " + elem->getDescription());
		}
		//manually add this one since its not a voting command
		helpText.push_back("!listPlayers - Lists the Index and Name of each player in the session. ");
	}

	bool addToVoteTimes(uint64_t sender)
	{
		time_t curTime;
		time(&curTime);
		//check if the person is spamming voting
		std::map<uint64_t, time_t>::iterator it = voteTimes.find(sender);
		if (it != voteTimes.end())
		{
			//Now we know that the user has started a vote recently. Check to see if it's under 90 seconds ago. 
			if (curTime - it->second < 90)
				return false;
			it->second = curTime;
		}
		else
			voteTimes.insert(std::make_pair(sender, curTime));
		return true;
	}
	void Tick()
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished() || !session->IsHost())
			return;

		if (chatCommandsActive)
		{
			for (auto elem : Commands)
				elem->tick();
		}

	}

	std::vector<std::string> getHelpText()
	{
		return helpText;
	}

	//We can guarantee that we are the host here and the the message isnt a server message
	bool handleChatMessage(const Server::Chat::ChatMessage &message, int peer)
	{
		std::string line = Utils::String::Trim(std::string(message.Body));
		auto* session = Blam::Network::GetActiveSession();

		if (line[0] != '!')
			return false;

		line.erase(0, 1);

		//TODO move the logic for !help and !listPlayers into a new non-voting chat command type
		if (line == "help")
		{
			for (auto i : getHelpText()) {
				Server::Chat::SendServerMessage(i, peer);
			}
			return true;
		}
		else if (line == "listPlayers" || line == "list")
		{
			int peerIdx = session->MembershipInfo.FindFirstPeer();
			while (peerIdx != -1)
			{

				int playerIdx = session->MembershipInfo.GetPeerPlayer(peerIdx);
				if (playerIdx != -1)
				{
					auto* player = &session->MembershipInfo.PlayerSessions[playerIdx];
					Server::Chat::SendServerMessage(("(" + std::to_string(playerIdx) + ") " + "\"" + Utils::String::ThinString(player->Properties.DisplayName) + "\""), peer);
				}

				peerIdx = session->MembershipInfo.FindNextPeer(peerIdx);
			}
			return true;
		}

		//If the voting commands are not active at the moment, don't do anything.
		if (!chatCommandsActive)
		{
			return false;
		}


		for (auto elem : Commands)
		{
			if (elem->isCurrentlyVoting())
			{
				elem->processVote(peer, line);
				return true;
			}
		}

		int numArgs = 0;
		auto args = Modules::CommandLineToArgvA((PCHAR)line.c_str(), &numArgs);
		std::string cmd = args[0];
		boost::algorithm::to_lower(cmd);

		//If we are not already voting, check if the command is initiating one
		for (auto elem : Commands)
		{
			std::string name = elem->getName();
			boost::algorithm::to_lower(name);
			if (cmd == (name))
			{
				//Now that we know this is the command being invoked, lets prep the argument (if there is one) and pass it in
				std::vector<std::string> argsVect;
				for (int i = 1; i < numArgs; i++)
					argsVect.push_back(args[i]);

				if (argsVect.size() > 0)
					elem->processMessage(peer, Utils::String::Join(argsVect));
				else
					elem->processMessage(peer, "");
			}
		}
		return true;
	}



}

