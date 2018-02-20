#pragma once
#include <string>
#include <vector>
#include <map>
#include "../Server/VotingPackets.hpp"
#include "../Modules/ModuleServer.hpp"

namespace Server::Voting
{

	/*
		
	* - These are the structs used to hold Map/Variant information.
	*
	* - Each has a DisplayName field, so they can display a string other than the actual mapName.
	*   For example, they might want to just display 'Narrows' instead of 'Narrowsv2Fixed'. Same goes for gametypes
	*
	* - Maps contain the mapId so the clients can know which image to display in the UI
	*
	* - Each gametype has an optional list of specific maps for that gametype.
	*   For example, you may want CTF to only be run on large maps, ect.
	*
	* - You can also specify whether sprint should be enabled or disabled for any given gametype
	*
	*/
	struct HaloMap {
		std::string mapName;
		std::string mapDisplayName;
		int mapId;
		HaloMap() {}
		HaloMap(std::string m, std::string dn, int id) {
			mapName = m;
			if (dn.length() > 16)
				mapDisplayName = dn.substr(0,16);
			else
				mapDisplayName = dn;
			mapId = id;
		}
	};

	struct HaloType {
		std::vector<HaloMap> specificMaps = std::vector <HaloMap>{};
		std::string typeName;
		std::string typeDisplayName;
		std::vector<std::string> commands = std::vector<std::string>{};
		HaloType() {}
		HaloType(std::string t, std::string dn) {
			typeName = t;
			if (dn.length() > 16)
				typeDisplayName = dn.substr(0,16);
			else
				typeDisplayName = dn;
		}

	};



	struct MapAndType {
		HaloMap haloMap;
		HaloType haloType;
		int Count; //number of votes
		int index = -1;
		bool canveto = false;
		bool isRevoteOption = false;
		MapAndType() { Count = 0; }
		MapAndType(HaloMap hm, HaloType ht) {
			haloMap = hm;
			haloType = ht;
			Count = 0;
		}

		//Used for sorting
		bool operator<(const MapAndType& val) const {
			return Count < val.Count;
		}

		//This is used for comparing voting options to see if they are unique. 
		bool operator==(const MapAndType& val) const
		{
			auto &serverModule = Modules::ModuleServer::Instance();
			if (serverModule.VarServerVotingDuplicationLevel->ValueInt == 0)
				return ((haloMap.mapDisplayName == val.haloMap.mapDisplayName) || (haloType.typeDisplayName == val.haloType.typeDisplayName));
			else if (serverModule.VarServerVotingDuplicationLevel->ValueInt == 1)
				return ((haloMap.mapDisplayName == val.haloMap.mapDisplayName) && (haloType.typeDisplayName == val.haloType.typeDisplayName));
			else
				return false;
		}
	};


	class AbstractVotingSystem
	{
	public:

		virtual void Init() = 0;
		virtual void NewVote() = 0;
		virtual void Tick() = 0;
		virtual void Reset() = 0;
		virtual void StartVoting() = 0;
		virtual bool isEnabled() = 0;
		virtual void SendVoteCountsToEveryone() = 0;
			
		virtual bool ShouldSendVotingOptions();
		virtual VotingMessage GenerateVotingOptionsMessage(bool updateTimeRemaining) = 0;
		virtual void LogVote(const VotingMessage &message, std::string name) = 0; //TODO abstract VotingMessage out of VotingSystem
		void GenerateVotingOptionsMessage(int peer);
		bool ReloadVotingJson(std::string filename);
		AbstractVotingSystem();
	protected:
			
			
		//Map of playerNames and their vote. We can use the playerName safely since mid-session name changes are no longer allowed.
		std::map<std::string, int> mapVotes = std::map<std::string, int>{};
		time_t voteStartedTime = 0;
		bool revoteFlag = false;
		bool idle = false;

	private:
		virtual bool LoadJson(std::string filename) = 0;
		virtual MapAndType GenerateVotingOption() = 0;
		virtual void loadDefaultMapsAndTypes() = 0;

	};

	class VotingSystem : public AbstractVotingSystem
	{
	public:
		virtual void Init();
		virtual void NewVote();
		virtual void Tick();
		virtual void Reset();
		virtual void StartVoting();
		virtual VotingMessage GenerateVotingOptionsMessage(bool updateTimeRemaining);
		virtual bool isEnabled();
		virtual void SendVoteCountsToEveryone();
			
			
		virtual void LogVote(const VotingMessage &message, std::string name); //TODO abstract VotingMessage out of VotingSystem
		VotingSystem();

	private:
		virtual bool LoadJson(std::string filename);
		virtual void loadDefaultMapsAndTypes();
		virtual MapAndType GenerateVotingOption();

		void countVotes();
		void FindWinner();
		//The time the winner was chosen. Used to determine when to start the game ( 5 seconds after the winner is chosen )
		time_t winnerChosenTime = 0;

		unsigned int numberOfRevotesUsed = 0;
		//The current voting options being voted on.
		std::vector<MapAndType> currentVotingOptions = std::vector<MapAndType>{};


		//The pool of maps and gametypes to choose from
		std::vector<HaloType> gameTypes = std::vector <HaloType>{};
		std::vector<HaloMap> haloMaps = std::vector <HaloMap>{};


	};

	class VetoSystem : public AbstractVotingSystem
	{
	public:
		virtual void Init();
		virtual void NewVote();
		virtual void Tick();
		virtual void SendVoteCountsToEveryone();
		virtual void Reset();
		virtual void StartVoting();
		virtual VotingMessage GenerateVotingOptionsMessage(bool updateTimeRemaining);
		virtual bool isEnabled();
		virtual void LogVote(const VotingMessage &message, std::string name); //TODO abstract VotingMessage out of VotingSystem
		VetoSystem();

	private:
		virtual bool LoadJson(std::string filename);
		virtual MapAndType GenerateVotingOption();
		void SetGameAndMap();
		void SetStartTimer();
		void countVotes();
		void FindWinner();
		virtual void loadDefaultMapsAndTypes();

		bool loadedJson = true;
		//The time the vote started. Used to calculate how much time remains and to check if a vote is in progress.
		time_t startime = 0;
		//The time the vote started. Used to calculate how much time remains and to check if a vote is in progress.
		std::vector<MapAndType> currentPlaylist = std::vector<MapAndType>{};
		std::vector<MapAndType> entirePlaylist = std::vector<MapAndType>{};
		MapAndType currentVetoOption;
		int numberOfVetosUsed = 0;
		int currentNumberOfVotes = 0;
	};
}


	



