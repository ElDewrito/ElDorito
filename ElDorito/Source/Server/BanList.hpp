#pragma once

#include <string>
#include <unordered_set>
#include <cstdint>
#include <istream>
#include <map>
#include "../Utils/Utils.hpp"

namespace Server
{


	/*
	* - Temporary Ban List
	*
	* - This is a temporary ban list that ban players for a specified amount of games.
	* - The amound of games is configurable via server variable.
	*/
	class TempBanList : public Utils::Singleton<TempBanList>
	{
	public:
		TempBanList() { }
		// Adds an IP address to the ban list.
		void AddIp(const std::string ip);
		// Returns whether an IP address is in the ban list.
		inline bool ContainsIp(std::string ip)
		{
			return ipAddresses.find(ip) != ipAddresses.end();
		}
		inline bool RemoveIp(const std::string &ip)
		{
			return ipAddresses.erase(ip) == 1;
		}
		void decrementDuration();
		// Clears ban list. Returns true if successful.
		inline void ClearList()
		{
			ipAddresses.clear();
		}
	private:
		std::map<std::string, int> ipAddresses = std::map<std::string, int>{};
	};

	class BanList
	{
	public:
		BanList() { }

		// Constructs a ban list from a stream.
		explicit BanList(std::istream &stream)
		{
			Read(stream);
		}

		// Constructs a ban list from a file.
		explicit BanList(const std::string &path);

		// Adds an IP address to the ban list.
		inline void AddIp(const std::string &ip)
		{
			ipAddresses.insert(ip);
		}

		// Returns whether an IP address is in the ban list.
		inline bool ContainsIp(const std::string &ip) const
		{
			return ipAddresses.find(ip) != ipAddresses.end();
		}

		// Removes an IP address from the ban list. Returns true if successful.
		inline bool RemoveIp(const std::string &ip)
		{
			return ipAddresses.erase(ip) == 1;
		}

		// Adds a UID to the ban list.
		inline void AddUid(uint64_t uid)
		{
			uids.insert(uid);
		}

		// Returns whether a UID is in the ban list.
		inline bool ContainsUid(uint64_t uid)
		{
			return uids.find(uid) != uids.end();
		}

		// Removes a UID from the ban list. Returns true if successful.
		inline bool RemoveUid(uint64_t uid)
		{
			return uids.erase(uid) == 1;
		}

		// Writes the ban list to a stream.
		void Save(std::ostream &stream) const;

		// Writes the ban list to a file.
		void Save(const std::string &path) const;

	private:
		void Read(std::istream &stream);

		std::unordered_set<std::string> ipAddresses;
		std::unordered_set<uint64_t> uids;
	};

	const std::string DefaultBanListPath = "mods/server/banlist.txt";

	// Loads the default ban list file.
	// If it does not exist, an empty ban list will be returned.
	BanList LoadDefaultBanList();

	// Saves the default ban list file.
	void SaveDefaultBanList(const BanList &list);
}