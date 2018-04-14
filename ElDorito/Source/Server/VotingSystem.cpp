#include <sstream>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cstring>
#include <io.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <unordered_map>

#include "../Utils/Logger.hpp"
#include "../Utils/Utils.hpp"
#include "VotingSystem.hpp"
#include "boost/filesystem.hpp"
#include "../patch.hpp"
#include "../Modules/ModuleServer.hpp"
#include "../Modules/ModuleGame.hpp"
#include "../Patches/Network.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../ElDorito.hpp"
#include "../ThirdParty/rapidjson/writer.h"
#include "../ThirdParty/rapidjson/document.h"

namespace Server::Voting
{
	std::unordered_map<int, std::string> MapNames =
	{
		{ 320, "Guardian" },
		{ 340, "Valhalla" },
		{ 705, "Diamondback" },
		{ 703, "Edge" },
		{ 700, "Reactor" },
		{ 31, "Icebox" },
		{ 390, "The Pit" },
		{ 380, "Narrows" },
		{ 310, "High Ground" },
		{ 410, "Standoff" },
		{ 400, "Sandtrap" },
		{ 30, "Last Resort" },
	};

	//These are just some default gametypes that get loaded if the user doesnt specify any
	const std::string DefaultTypes[8] =
	{
		"slayer",
		"team slayer",
		"multi flag",
		"team oddball",
		"oddball",
		"crazy king",
		"team king",
		"assault",
	};
	const std::string DefaultTypeNames[8] =
	{
		"Slayer",
		"Team Slayer",
		"Multi Flag",
		"Team Oddball",
		"FFA Oddball",
		"Crazy King",
		"Team King",
		"Assault",
	};

	int getVoteNeededToPass() {
			return (1 + (((Patches::Network::GetNumPlayers() - 1) * Modules::ModuleServer::Instance().VarVetoVotePassPercentage->ValueInt) / 100));		
	}

	//Gets the mapId for the given forge map
	int getCustomMapID(std::string mapName)
	{
		auto variantFileName = "mods/maps/" + mapName + "/sandbox.map";
		std::ifstream mapVariant(variantFileName, std::ios::binary);
		if (mapVariant.is_open())
		{
			int32_t mapId = 0;
			mapVariant.seekg(0x120);
			mapVariant.read(reinterpret_cast<char*>(&mapId), sizeof(mapId));
			return mapId;
		}

		return -1;
	}

	//Gets the mapId for the given default map
	int getDefaultMapId(const std::string &mapName)
	{
		// Open the .map file
		auto mapPath = ElDorito::Instance().GetMapsFolder() + mapName + ".map";
		std::ifstream mapFile(mapPath, std::ios::binary);
		if (!mapFile.is_open())
			return -1;

		int32_t mapId = 0;
		mapFile.seekg(0x2DEC);
		mapFile.read(reinterpret_cast<char*>(&mapId), sizeof(mapId));

		return mapId;
	}
	int getMapID(std::string mapName) {
		auto mapID = getDefaultMapId(mapName);
		if (mapID < 0)
			mapID = getCustomMapID(mapName);
		return mapID;
	}
	AbstractVotingSystem::AbstractVotingSystem(){}
	VotingSystem::VotingSystem() : AbstractVotingSystem() {}

	bool VotingSystem::isEnabled() {
		return Modules::ModuleServer::Instance().VarServerVotingEnabled->ValueInt == 1;
	}
	

	void VotingSystem::Init() {
		if (!LoadJson(Modules::ModuleServer::Instance().VarVotingJsonPath->ValueString))
			loadDefaultMapsAndTypes();
	}
	void VetoSystem::Init() {
		if (!LoadJson(Modules::ModuleServer::Instance().VarVetoJsonPath->ValueString))
			loadDefaultMapsAndTypes();
	}

	bool AbstractVotingSystem::ReloadVotingJson(std::string fileName) {

		bool success = true;
		if (!LoadJson(fileName)) {
			loadDefaultMapsAndTypes();
			success = false;
		}


		if (voteStartedTime != 0) {
			Reset();
			StartVoting();
		}
		return success;
	}

	void VotingSystem::Reset()
	{
		numberOfRevotesUsed = 0;
		winnerChosenTime = 0;
		voteStartedTime = 0;
		mapVotes.clear();
	}

	void VotingSystem::NewVote() {
		Reset();
		StartVoting();
	}


	void VotingSystem::LogVote(const VotingMessage &message, std::string name)
	{

		// If we aren't in a vote or if voting is not enabled, exit
		if (!(voteStartedTime != 0 && Modules::ModuleServer::Instance().VarServerVotingEnabled->ValueInt))
			return;

		unsigned int vote = message.Vote;

		//unlikely to happen unless someone messes with the JS
		if (vote < 0 || vote >(Modules::ModuleServer::Instance().VarServerNumberOfVotingOptions->ValueInt + 1))
			return;

		// If this person has already voted, then we replace his vote. If not, then we add a new name-vote pair
		std::map<std::string, int>::iterator it = mapVotes.find(name);
		if (it != mapVotes.end())
			it->second = vote;
		else
			mapVotes.insert(std::make_pair(name, vote));

		SendVoteCountsToEveryone();

	}
	void VetoSystem::SendVoteCountsToEveryone() {
		countVotes();

		//create a message to send to all of the players with the new vote counts
		VotingMessage newmessage(VotingMessageType::VoteTally);
		newmessage.votes[0] = currentNumberOfVotes;
		newmessage.votes[1] = currentNumberOfVotes;
		newmessage.votesNeededToPass = getVoteNeededToPass();
		BroadcastVotingMessage(newmessage);
	}
	void VotingSystem::SendVoteCountsToEveryone() {
		countVotes();

		//create a message to send to all of the players with the new vote counts
		VotingMessage newmessage(VotingMessageType::VoteTally);
		int optionIndex = 0;
		for (auto &option : currentVotingOptions) {
			newmessage.votes[optionIndex] = option.Count;
			optionIndex++;
			option.Count = 0;
		}
		newmessage.votesNeededToPass = 0;
		BroadcastVotingMessage(newmessage);

	}

	//Populate the Maps and Gametypes with default ones if no valid json was supplied
	void VotingSystem::loadDefaultMapsAndTypes()
	{
		for (auto m : Modules::ModuleGame::Instance().MapList)
		{
			auto id = getDefaultMapId(m);
			auto it = MapNames.find(id);
			if (it != MapNames.end())
			{
				HaloMap map(m, it->second, id);
				haloMaps.push_back(map);
			}
		}

		int index = 0;
		for (auto t : DefaultTypes)
		{
			HaloType type(t, DefaultTypeNames[index]);
			gameTypes.push_back(type);
			index++;
		}
	}

	MapAndType VetoSystem::GenerateVotingOption()
	{
		MapAndType m;
		if (Modules::ModuleServer::Instance().VarVetoSystemSelectionType->ValueInt == 0) {
			//randomly pick one out of the playlist and remove it.
			int optionIndex = rand() % currentPlaylist.size();

			m = currentPlaylist[optionIndex];
			currentPlaylist.erase(currentPlaylist.begin() + optionIndex);

		}
		else {
			m = currentPlaylist.front();
			currentPlaylist.erase(currentPlaylist.begin());
		}

		if (currentPlaylist.size() == 0) {
			currentPlaylist = entirePlaylist;
		}
		return m;

	}
	//Randomly picks a voting option. If the gametype it picks has maps that are specific to that gametype, then it picks a map from those.
	MapAndType VotingSystem::GenerateVotingOption()
	{
		HaloType gametype = gameTypes[rand() % gameTypes.size()];
		HaloMap map;

		if (gametype.specificMaps.size() > 0)
			map = gametype.specificMaps[rand() % gametype.specificMaps.size()];
		else
			map = haloMaps[rand() % haloMaps.size()];

		return MapAndType(map, gametype);
	}

	bool AbstractVotingSystem::ShouldSendVotingOptions() {
		return voteStartedTime != 0;
	}
	//Creates the message to send to peers. TODO abstract VotingMessage out of VotingSystem
	VotingMessage VetoSystem::GenerateVotingOptionsMessage(bool updateTime)
	{
		VotingMessage newmessage(VotingMessageType::VetoOption);

		strncpy_s(newmessage.votingOptions[0].mapName, currentVetoOption.haloMap.mapDisplayName.c_str(), sizeof(newmessage.votingOptions[0].mapName));
		strncpy_s(newmessage.votingOptions[0].typeName, currentVetoOption.haloType.typeDisplayName.c_str(), sizeof(newmessage.votingOptions[0].typeName));
		newmessage.votingOptions[0].mapId = currentVetoOption.haloMap.mapId;
		newmessage.votingOptions[0].canVeto = currentVetoOption.canveto;

		time_t curTime;
		time(&curTime);
		if (currentVetoOption.canveto) {
			if (updateTime && voteStartedTime > 0) 
				newmessage.voteTime = Modules::ModuleServer::Instance().VarVetoVoteTime->ValueInt - (int)difftime(curTime,voteStartedTime);
			else
				newmessage.voteTime = Modules::ModuleServer::Instance().VarVetoVoteTime->ValueInt;

			newmessage.votesNeededToPass = getVoteNeededToPass();
		}
		else {
			if (updateTime && startime > 0)
				newmessage.voteTime = Modules::ModuleServer::Instance().VarVetoWinningOptionShownTime->ValueInt - (int)difftime(curTime,startime);
			else
				newmessage.voteTime = Modules::ModuleServer::Instance().VarVetoWinningOptionShownTime->ValueInt;
			newmessage.votesNeededToPass = 0;
		}

		return newmessage;
	}

	//Creates the message to send to peers. TODO abstract VotingMessage out of VotingSystem
	void AbstractVotingSystem::GenerateVotingOptionsMessage(int peer)
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost() && session->Parameters.GetSessionMode() == 1 && isEnabled()))
			return;

		VotingMessage newmessage = GenerateVotingOptionsMessage(true);
		SendVotingMessageToPeer(newmessage, peer);
	}
	//Creates the message to send to peers. TODO abstract VotingMessage out of VotingSystem
	VotingMessage VotingSystem::GenerateVotingOptionsMessage(bool updateTime)
	{
		VotingMessage newmessage(VotingMessageType::VotingOptions);
		int i = 0;
		for (auto &option : currentVotingOptions) {
			strncpy_s(newmessage.votingOptions[i].mapName, option.haloMap.mapDisplayName.c_str(), sizeof(newmessage.votingOptions[i].mapName));
			strncpy_s(newmessage.votingOptions[i].typeName, option.haloType.typeDisplayName.c_str(), sizeof(newmessage.votingOptions[i].typeName));
			newmessage.votingOptions[i].mapId = option.haloMap.mapId;
			i++;
		}
		time_t curTime;
		time(&curTime);
		if (updateTime)
			newmessage.voteTime = Modules::ModuleServer::Instance().VarServerMapVotingTime->ValueInt - (int)difftime(curTime,voteStartedTime);
		else 
			newmessage.voteTime = Modules::ModuleServer::Instance().VarServerMapVotingTime->ValueInt;
		
		return newmessage;
	}

	//Counds the votes
	void VotingSystem::countVotes()
	{
		for (auto const &vote : mapVotes)
		{
			//in case someone modifies the js and sends something other than a valid vote.
			if (vote.second > currentVotingOptions.size() || vote.second < 1)
				continue;
			currentVotingOptions.at(vote.second - 1).Count++;
		}
	}
	//Counds the votes
	void VetoSystem::countVotes()
	{
		currentNumberOfVotes = 0;
		for (auto const &vote : mapVotes)
		{
			//in case someone modifies the js and sends something other than a valid vote.
			if (vote.second != 1)
				continue;
			currentNumberOfVotes++;
		}
	}

	/*
	* Counts the votes, sorts the options by vote tally.
	*
	* - Ties are handled like Halo Reach: Ties will be awarded to the latter option.
	*   So if option 3 has 2 votes and option 4 has 2 votes, option 4 will win.
	*/
	void VotingSystem::FindWinner()
	{
		countVotes();
		std::sort(currentVotingOptions.begin(), currentVotingOptions.end());
		auto winningOption = currentVotingOptions.back();

		if (winningOption.isRevoteOption)
		{
			numberOfRevotesUsed++;
			mapVotes.clear();
			voteStartedTime = 0;
			revoteFlag = true;
			return;
		}

		VotingMessage newmessage(VotingMessageType::Winner);
		newmessage.winner = winningOption.index;
		newmessage.voteTime = Modules::ModuleServer::Instance().VarServerTimeBetweenVoteEndAndGameStart->ValueInt;
		BroadcastVotingMessage(newmessage);

		Modules::CommandMap::Instance().ExecuteCommand("Game.GameType \"" + winningOption.haloType.typeName + "\"");
		Modules::CommandMap::Instance().ExecuteCommand("Game.Map \"" + winningOption.haloMap.mapName + "\"");

		for (auto command : winningOption.haloType.commands) {
			Modules::CommandMap::Instance().ExecuteCommand(command);
		}

		time(&winnerChosenTime);
		voteStartedTime = 0;
		mapVotes.clear();
	}

	//Starts a new vote
	void VotingSystem::StartVoting()
	{
		if (idle)
			return;

		currentVotingOptions.clear();

		for (unsigned int i = 0; i < Modules::ModuleServer::Instance().VarServerNumberOfVotingOptions->ValueInt; i++)
		{
			auto Option = GenerateVotingOption();

			//dissallow duplicates
			while (std::find(currentVotingOptions.begin(), currentVotingOptions.end(), Option) != currentVotingOptions.end())
				Option = GenerateVotingOption();
			Option.index = i + 1;
			currentVotingOptions.push_back(Option);
		}
		//check how many revotes we have done.
		if (numberOfRevotesUsed < Modules::ModuleServer::Instance().VarServerNumberOfRevotesAllowed->ValueInt)
		{
			auto revote = MapAndType();
			revote.isRevoteOption = true;
			revote.haloMap.mapDisplayName = "Revote";
			revote.index = currentVotingOptions.size() + 1;
			currentVotingOptions.push_back(revote);
		}
		time(&voteStartedTime);
		auto message = GenerateVotingOptionsMessage(false);
		BroadcastVotingMessage(message);
	}
	void VotingSystem::Tick()
	{

		time_t curTime1;
		time(&curTime1);

		if (idle)
		{
			if (Patches::Network::GetNumPlayers() > 0)
			{
				idle = false;
				if (Blam::Network::GetActiveSession()->Parameters.GetSessionMode() == 1)
					StartVoting();

			}

			return;
		}
		else if (Patches::Network::GetNumPlayers() == 0)
		{
			idle = true;
			Reset();
			return;
		}

		//if game.start is called immediately after the winning option is chosen, this causes a number of players to be kicked from the game for some reason.
		//So what we are doing here is waiting 4 seconds to allow everyone to get the map and gametype loaded
		if (winnerChosenTime != 0)
		{

			auto elapsed = curTime1 - winnerChosenTime;
			if (elapsed >  Modules::ModuleServer::Instance().VarServerTimeBetweenVoteEndAndGameStart->ValueInt)
			{
				Modules::CommandMap::Instance().ExecuteCommand("Game.Start");
				Reset();
			}
		}

		//Calling from Tick() to avoid a circular loop.
		if (revoteFlag)
		{
			StartVoting();
			revoteFlag = false;
			return;
		}

		//if we aren't in a vote, return
		if (voteStartedTime == 0)
			return;

		auto elapsed = curTime1 - voteStartedTime;

		//Exit if we haven't used up the time yet.
		if (elapsed < Modules::ModuleServer::Instance().VarServerMapVotingTime->ValueInt)
			return;

		FindWinner();

	}
	//Loads the voting json. If there is an easier way to serialize into structs, let me know.
	bool VotingSystem::LoadJson(std::string filename)
	{
		//clear the current contents
		haloMaps.clear();
		gameTypes.clear();

		std::ifstream in(filename, std::ios::in | std::ios::binary);
		if (!in || !in.is_open())
			return false;

		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize((unsigned int)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();

		rapidjson::Document document;
		if (!document.Parse<0>(contents.c_str()).HasParseError() && document.IsObject())
		{
			if (!document.HasMember("Types") || !document.HasMember("Maps"))
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Json must contain 'Maps' and 'Types' arrays. Using defaults instead");
				return false;
			}

			const rapidjson::Value& maps = document["Maps"];
			// rapidjson uses SizeType instead of size_t :/
			for (rapidjson::SizeType i = 0; i < maps.Size(); i++)
			{
				const rapidjson::Value& mapObject = maps[i];
				if (!mapObject.HasMember("mapName") || !mapObject.HasMember("displayName"))
					continue;

				//Check to make sure that the map exists
				std::string mapName = mapObject["mapName"].GetString();
				auto mapID = getMapID(mapName);
				if (mapID < 0) {
					Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Invalid Map: " + mapName + ", skipping..");
					continue;
				}
				haloMaps.push_back(HaloMap(mapName, mapObject["displayName"].GetString(), mapID));

			}

			const rapidjson::Value& types = document["Types"];
			for (rapidjson::SizeType i = 0; i < types.Size(); i++)
			{
				const rapidjson::Value& c = types[i];
				if (!c.HasMember("typeName") || !c.HasMember("displayName"))
					continue;

				//TODO verify the gametypes
				HaloType ht(c["typeName"].GetString(), c["displayName"].GetString());


				if (c.HasMember("commands"))
				{
					const rapidjson::Value& commands = c["commands"];
					for (rapidjson::SizeType i = 0; i < commands.Size(); i++)
					{
						ht.commands.push_back(commands[i].GetString());
					}
				}

				if (c.HasMember("SpecificMaps"))
				{
					const rapidjson::Value& smaps = c["SpecificMaps"];
					if (!smaps.IsNull())
					{
						for (rapidjson::SizeType i = 0; i < smaps.Size(); i++)
						{
							const rapidjson::Value& map = smaps[i];
							if (!map.HasMember("mapName") || !map.HasMember("displayName"))
								continue;

							std::string mapName = map["mapName"].GetString();
							auto mapID = getMapID(mapName);
							if (mapID < 0) {
								Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Invalid Map: " + mapName + ", skipping..");
								continue;
							}
							ht.specificMaps.push_back(HaloMap(mapName, map["displayName"].GetString(), mapID));
						}
					}
				}
				gameTypes.push_back(ht);

			}
		}
		else
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Could not parse voting json. Using defaults instead");
			return false;
		}
		if (gameTypes.size() < 2 || haloMaps.size() < 2)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Json must contain at least two gametypes and two maps. Using defaults instead.");
			return false;
		}


		return true;
	}


	VetoSystem::VetoSystem() : AbstractVotingSystem() {}
	bool VetoSystem::isEnabled() {
		return Modules::ModuleServer::Instance().VarVetoSystemEnabled->ValueInt == 1 && Modules::ModuleServer::Instance().VarServerVotingEnabled->ValueInt == 0;
	}

	void VetoSystem::Reset()
	{
		voteStartedTime = 0;
		numberOfVetosUsed = 0;
		startime = 0;
		currentNumberOfVotes = 0;
		mapVotes.clear();
	}


	//Starts a new vote
	void VetoSystem::StartVoting()
	{
		if (idle)
			return;

		numberOfVetosUsed++;
		currentVetoOption = GenerateVotingOption();
		currentVetoOption.canveto = true;
		SetGameAndMap();
		auto message = GenerateVotingOptionsMessage(false);
		BroadcastVotingMessage(message);

		time(&voteStartedTime);

	}


	/*
	* Counts the votes, sorts the options by vote tally.
	*
	* - Ties are handled like Halo Reach: Ties will be awarded to the latter option.
	*   So if option 3 has 2 votes and option 4 has 2 votes, option 4 will win.
	*/
	void VetoSystem::FindWinner()
	{
		countVotes();

		if (currentNumberOfVotes >= getVoteNeededToPass()){
			revoteFlag = true;
		}
		else {
			currentVetoOption.canveto = false;
			SetGameAndMap();
			SetStartTimer();
		}

		voteStartedTime = 0;
		mapVotes.clear();
	}

	void VetoSystem::SetGameAndMap()
	{
		Modules::CommandMap::Instance().ExecuteCommand("Game.GameType \"" + currentVetoOption.haloType.typeName + "\"");
		Modules::CommandMap::Instance().ExecuteCommand("Game.Map \"" + currentVetoOption.haloMap.mapName + "\"");

		for (auto command : currentVetoOption.haloType.commands) {
			Modules::CommandMap::Instance().ExecuteCommand(command);
		}

	}

	void VetoSystem::SetStartTimer()
	{
		auto message = GenerateVotingOptionsMessage(false);
		BroadcastVotingMessage(message);
		time(&startime);
	}

	bool VetoSystem::LoadJson(std::string filename)
	{
		//clear the current contents
		entirePlaylist.clear();

		std::ifstream in(filename, std::ios::in | std::ios::binary);
		if (!in || !in.is_open())
			return false;

		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize((unsigned int)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();

		rapidjson::Document document;
		if (!document.Parse<0>(contents.c_str()).HasParseError() && document.IsObject())
		{
			if (!document.HasMember("playlist"))
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Json does not contain an array named 'playlist'. Using default maps and gametypes. ");
				return false;
			}
				

			const rapidjson::Value& mapAndTypes = document["playlist"];
			// rapidjson uses SizeType instead of size_t :/
			for (rapidjson::SizeType i = 0; i < mapAndTypes.Size(); i++)
			{
				const rapidjson::Value& mapAndTypeObject = mapAndTypes[i];
				if (!mapAndTypeObject.HasMember("map") || !mapAndTypeObject.HasMember("gametype")) {
					Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Invalid playlist entry, skipping..");
					continue;

				}


				const rapidjson::Value& map = mapAndTypeObject["map"];
				const rapidjson::Value& gametype = mapAndTypeObject["gametype"];

				if (!gametype.HasMember("typeName") || !gametype.HasMember("displayName") || !map.HasMember("mapName") || !map.HasMember("displayName"))
					continue;
				std::string mapName = map["mapName"].GetString();
				auto mapID = getMapID(mapName);
				if (mapID < 0) {
					Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Invalid Map: " + mapName + ", skipping..");
					continue;
				}

				HaloMap m = HaloMap(mapName, map["displayName"].GetString(), mapID);
				HaloType t = HaloType(gametype["typeName"].GetString(), gametype["displayName"].GetString());

				if (gametype.HasMember("commands"))
				{
					const rapidjson::Value& commands = gametype["commands"];
					for (rapidjson::SizeType i = 0; i < commands.Size(); i++)
					{
						t.commands.push_back(commands[i].GetString());
					}
				}
				entirePlaylist.push_back(MapAndType(m, t));
			}
		}
		else
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Json is formatted incorrectly. Using default maps and gametypes. ");
			return false;
		}

		if (entirePlaylist.size() < 1)
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "No items in playlist array. Using default maps and gametypes. ");
			return false;
		}
			

		currentPlaylist = entirePlaylist;
		return true;
	}



	void VetoSystem::Tick()
	{
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost() && loadedJson && Modules::ModuleServer::Instance().VarVetoSystemEnabled->ValueInt && !Modules::ModuleServer::Instance().VarServerVotingEnabled->ValueInt))
			return;

		time_t curTime1;
		time(&curTime1);

		if (idle)
		{
			if (Patches::Network::GetNumPlayers() > 0)
			{
				idle = false;
				if (Blam::Network::GetActiveSession()->Parameters.GetSessionMode() == 1)
					NewVote();
			}

			return;
		}
		else if (Patches::Network::GetNumPlayers() == 0)
		{

			idle = true;
			Reset();
			return;
		}
		//if game.start is called immediately after the winning option is chosen, this causes a number of players to be kicked from the game for some reason.
		//So what we are doing here is waiting 4 seconds to allow everyone to get the map and gametype loaded
		if (startime != 0)
		{

			auto elapsed = curTime1 - startime;
			if (elapsed > Modules::ModuleServer::Instance().VarVetoWinningOptionShownTime->ValueInt)
			{
				Modules::CommandMap::Instance().ExecuteCommand("Game.Start");
				Reset();
			}
		}
		if (revoteFlag)
		{
			NewVote();
			revoteFlag = false;
			return;
		}
		//if we aren't in a vote, return
		if (voteStartedTime == 0)
			return;

		auto elapsed = curTime1 - voteStartedTime;

		//Exit if we haven't used up the time yet.
		if (elapsed < Modules::ModuleServer::Instance().VarVetoVoteTime->ValueInt)
			return;

		FindWinner();


	}

	void VetoSystem::LogVote(const VotingMessage &message, std::string name)
	{
		// If we aren't in a vote or if voting is not enabled, exit
		auto* session = Blam::Network::GetActiveSession();
		if (!(session && session->IsEstablished() && session->IsHost() && Modules::ModuleServer::Instance().VarVetoSystemEnabled->ValueInt && !Modules::ModuleServer::Instance().VarServerVotingEnabled->ValueInt && voteStartedTime != 0))
			return;

		unsigned int vote = message.Vote;


		// If this person has already voted, then we replace his vote. If not, then we add a new name-vote pair
		std::map<std::string, int>::iterator it = mapVotes.find(name);
		if (it != mapVotes.end()) {
			mapVotes.erase(it);
		}

		else
			mapVotes.insert(std::make_pair(name, vote));

		SendVoteCountsToEveryone();

	}
	void VetoSystem::NewVote() {
		if (numberOfVetosUsed < Modules::ModuleServer::Instance().VarNumberOfVetoVotes->ValueInt) {

			//we want to start a new veto vote
			mapVotes.clear();
			voteStartedTime = 0;
			StartVoting();
		}

		else {

			//we want to send a voting option that can not be voted on, and then start the game.
			currentVetoOption = GenerateVotingOption();
			currentVetoOption.canveto = false;
			SetGameAndMap();
			SetStartTimer();
		}
	}

	//Populate the Maps and Gametypes with default ones if no valid json was supplied
	void VetoSystem::loadDefaultMapsAndTypes()
	{
		//The pool of maps and gametypes to choose from
		std::vector<HaloType> gameTypes = std::vector <HaloType>{};
		std::vector<HaloMap> haloMaps = std::vector <HaloMap>{};

		for (auto m : Modules::ModuleGame::Instance().MapList)
		{
			auto id = getDefaultMapId(m);
			auto it = MapNames.find(id);
			if (it != MapNames.end())
			{
				HaloMap map(m, it->second, id);
				haloMaps.push_back(map);
			}
		}

		int index = 0;
		for (auto t : DefaultTypes)
		{
			HaloType type(t, DefaultTypeNames[index]);
			gameTypes.push_back(type);
			index++;
		}

		//Add 10 items to the playlist
		for (int i = 0; i < 10; i++) {
			HaloType gametype = gameTypes[rand() % gameTypes.size()];
			HaloMap map = haloMaps[rand() % haloMaps.size()];
			entirePlaylist.push_back(MapAndType(map, gametype));
		}
		currentPlaylist = entirePlaylist;
	}
}


