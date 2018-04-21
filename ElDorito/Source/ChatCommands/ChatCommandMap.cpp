#include "ChatCommandMap.hpp"
#include <sstream>
#include <iostream>

#include "ChatCommand.hpp"
#include "../Server/BanList.hpp"
#include "../Utils/Utils.hpp"
#include "../Utils/Logger.hpp"
#include "../CommandMap.hpp"
#include "../Patches/Network.hpp"



namespace ChatCommands
{
	std::vector<AbstractChatCommand *> Commands;
	bool chatCommandsActive; // they are only allowed in-game, not in the lobby
	std::map<uint64_t, time_t> voteTimes = std::map<uint64_t, time_t>{};

	KickPlayerCommand kickPlayerCommand;
	KickIndexCommand kickIndexCommand;
	EndGameCommand endGameCommand;
	ShuffleTeamsCommand shuffleTeamsCommand;

	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState)
	{

		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost()))
			return;

		switch (newState)
		{

			case Blam::Network::eLifeCycleStateStartGame:
			{
				chatCommandsActive = true;
				break;
			}

			case Blam::Network::eLifeCycleStateEndGameWriteStats: //This is fired if you also hit game.stop
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
				break;
			}
			
		}
	}

	void Init()
	{
		Patches::Network::OnLifeCycleStateChanged(LifeCycleStateChanged);

		Commands.push_back((AbstractChatCommand*) &kickPlayerCommand);
		Commands.push_back((AbstractChatCommand*) &kickIndexCommand);
		Commands.push_back((AbstractChatCommand*) &endGameCommand);
		Commands.push_back((AbstractChatCommand*) &shuffleTeamsCommand);

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
		std::vector<std::string> helpText = std::vector <std::string>{};
		if (chatCommandsActive)
		{
			for (auto elem : Commands)
			{
				if (elem->isEnabled())
					helpText.push_back("!" + elem->getName() + " - " + elem->getDescription());
			}

		}
		helpText.push_back("!listPlayers - Lists the Index and Name of each player in the session. ");
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

		if (line.empty())
			return true;
		//TODO move the logic for !help and !listPlayers into a new non-voting chat command type
		std::string lowercaseline = Utils::String::ToLower(line);
		if (lowercaseline == "help")
		{
			for (auto i : getHelpText()) {
				Server::Chat::SendServerMessage(i, peer);
			}
			return true;
		}
		else if (lowercaseline == "listplayers" || lowercaseline == "list")
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
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

		//If we are not already voting, check if the command is initiating one
		for (auto elem : Commands)
		{
			std::string name = elem->getName();
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);

			if (cmd == (name))
			{
				if (!chatCommandsActive)
				{
					Server::Chat::SendServerMessage("This command is not currently active.", peer);
					return true;
				}
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
