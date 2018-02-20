#pragma once

#include <string>
#include <bitset>
#include <memory>
#include "../Blam/BlamNetwork.hpp"
namespace Server::Voting
{
	// Chat message types.
	enum class VotingMessageType : uint32_t
	{
		VetoVote,

		VetoOption,

		// A user's vote, to be sent to the host.
		Vote,

		// The Voting Options, sent to all clients
		VotingOptions,

		// The Vote Tally, sent to all clients
		VoteTally,

		// The winning option
		Winner,

		// Not actually a message type, just used to indicate the number of
		// valid message types.
		Count
	};
	struct VotingOption
	{
		char mapName[17];  // The Display Name of the map to show in the UI
		char typeName[17]; // The Display Name of the gametype to show in the UI
		bool canVeto;
		int mapId;		   // mapId, used to determine which image to show
	};

	// Voting Message Data
	struct VotingMessage
	{
		VotingMessage() { }

		VotingMessage(VotingMessageType type);
		// The message type.
		VotingMessageType Type;

		VotingOption votingOptions[5]; //  up to 4 options and one revote

		int voteTime; //The length of time that will be allowed for voting 

		int votes[5]; // Vote tally

		int Vote; // The user's vote

		int winner; 
		
		int votesNeededToPass;


	};

	// Interface for a class which processes and handles voting messages.
	class VotingMessageHandler
	{
	public:
		virtual ~VotingMessageHandler() { }

		// Called after a message has been received.
		virtual void MessageReceived(const VotingMessage &message) = 0;
	};

	bool BroadcastVotingMessage(VotingMessage &message);

	bool SendVotingMessageToPeer(VotingMessage &message, int peer);

	void InitializePackets();
		
	bool SendVoteToHost(const int vote);

	void AddMessageHandler(std::shared_ptr<VotingMessageHandler> handler);
}
