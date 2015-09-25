#include "BanList.hpp"

#include <fstream>
#include "../Utils/String.hpp"

namespace
{
	bool ParseUid(const std::string &str, uint64_t *out)
	{
		try
		{
			size_t end;
			*out = std::stoull(str, &end, 16);
			return end == str.length(); // Only succeed if there's nothing after the number
		}
		catch (std::exception&)
		{
			return false;
		}
	}
}

namespace Server
{
	BanList::BanList(const std::string &path)
	{
		Read(std::ifstream(path));
	}

	void BanList::Read(std::istream &stream)
	{
		while (true)
		{
			// Read a line from the stream
			std::string line;
			std::getline(stream, line);
			if (stream.fail())
				break;

			// Remove any comment in the line
			auto commentStart = line.find('#');
			if (commentStart >= 0)
				line = line.substr(0, commentStart);

			// Trim whitespace and split the line
			// TODO: Make a trim function for trimming from both ends...
			line = Utils::String::Trim(Utils::String::Trim(line, false), true);
			auto components = Utils::String::SplitString(line);
			if (components.size() < 2)
				continue;

			if (components[0] == "ip")
			{
				AddIp(components[1]);
			}
			else if (components[0] == "uid")
			{
				uint64_t uid;
				if (ParseUid(components[1], &uid))
					AddUid(uid);
			}
		}
	}

	void BanList::Save(std::ostream &stream) const
	{
		stream << "# ElDewrito server ban list\n";
		stream << "# Players matching the filters in this file will not be allowed to connect to your server.\n";
		stream << "# Use the \"ban\" and \"unban\" console commands to easily edit this file.\n\n";
		
		stream << "# IPv4 address bans\n";
		stream << "# Format: ip XXX.XXX.XXX.XXX\n";
		for (auto ip : ipAddresses)
			stream << "ip " << ip << '\n';
		stream << '\n';
		
		stream << "# UID bans\n";
		stream << "# Format: uid XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
		for (auto uid : uids)
			stream << "uid " << std::hex << uid << std::dec << '\n';
	}

	void BanList::Save(const std::string &path) const
	{
		Save(std::ofstream(path, std::ios::trunc));
	}

	BanList LoadDefaultBanList()
	{
		std::ifstream file(DefaultBanListPath);
		if (file)
			return BanList(file);
		return BanList();
	}

	void SaveDefaultBanList(const BanList &list)
	{
		list.Save(DefaultBanListPath);
	}
}