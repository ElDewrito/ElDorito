#pragma once

#include <string>
#include <unordered_set>
#include <cstdint>
#include <istream>
#include <map>
#include "../Utils/Utils.hpp"

namespace Server
{
	class WhiteList
	{
	public:
		WhiteList() { }

		// Constructs a white list from a stream.
		explicit WhiteList(std::istream &stream)
		{
			Read(stream);
		}

		// Constructs a white list from a file.
		explicit WhiteList(const std::string &path);

		// Adds an IP address to the white list.
		inline void AddIp(const std::string &ip)
		{
			ipAddresses.insert(ip);
		}

		// Returns whether an IP address is in the white list.
		inline bool ContainsIp(const std::string &ip) const
		{
			return ipAddresses.find(ip) != ipAddresses.end();
		}

		// Removes an IP address from the white list. Returns true if successful.
		inline bool RemoveIp(const std::string &ip)
		{
			return ipAddresses.erase(ip) == 1;
		}

		// Adds a UID to the white list.
		inline void AddUid(uint64_t uid)
		{
			uids.insert(uid);
		}

		// Returns whether a UID is in the white list.
		inline bool ContainsUid(uint64_t uid)
		{
			return uids.find(uid) != uids.end();
		}

		// Removes a UID from the white list. Returns true if successful.
		inline bool RemoveUid(uint64_t uid)
		{
			return uids.erase(uid) == 1;
		}

		// Writes the white list to a stream.
		void Save(std::ostream &stream) const;

		// Writes the white list to a file.
		void Save(const std::string &path) const;

	private:
		void Read(std::istream &stream);

		std::unordered_set<std::string> ipAddresses;
		std::unordered_set<uint64_t> uids;
	};

	const std::string DefaultWhiteListPath = "mods/server/whitelist.txt";

	// Loads the default ban list file.
	// If it does not exist, an empty ban list will be returned.
	WhiteList LoadDefaultWhiteList();

	// Saves the default ban list file.
	void SaveDefaultWhiteList(const WhiteList &list);
}
