#pragma once
#include <string>
#include <vector>
#include <d3dx9core.h>
#include <map>
#include "../Utils/Utils.hpp"
#include "../Server/VotingPackets.hpp"
#include <chrono>

namespace Server
{
	namespace Voting
	{
		void PlayerJoinedVoteInProgress(int playerIndex);
		void Init();
		void Tick();
		void Enable();
		void Disable();
		void LogVote(const VotingMessage &message, std::string name); //TODO abstract VotingMessage out of VotingSystem
		void StartNewVote();
		bool LoadVotingJson();
		void CancelVoteInProgress();

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
			HaloMap(){}
			HaloMap(std::string m, std::string dn, int id) {
				mapName = m;
				mapDisplayName = dn;
				mapId = id;
			}
		};

		struct HaloType {
			std::vector<HaloMap> specificMaps = std::vector <HaloMap> {}; //Some gametypes only work with specific maps
			std::string typeName;
			std::string typeDisplayName;
			std::string SprintEnabled;
			HaloType(){}
			HaloType(std::string t, std::string dn) {
				typeName = t;
				typeDisplayName = dn;
				SprintEnabled = "0";
			}

		};

		struct MapAndType {
			HaloMap haloMap;
			HaloType haloType;
			int Count; //number of votes
			int index = -1;
			bool isRevoteOption = false;
			MapAndType(){ Count = 0; }
			MapAndType(HaloMap hm, HaloType ht) {
				haloMap = hm;
				haloType = ht;
				Count = 0;
			}

			//Used for sorting
			bool operator<(const MapAndType& val) const {
				return Count < val.Count;
			}
			
			//Tests equality, which lets us generate completely unique options
			bool operator==(const MapAndType& val) const
			{
				return ((haloMap.mapDisplayName == val.haloMap.mapDisplayName) || (haloType.typeDisplayName == val.haloType.typeDisplayName));
			}
		};

	}
}


	



