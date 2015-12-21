#include "BanList.hpp"

#include <fstream>
#include <iomanip>
#include "../Utils/String.hpp"
#include "../Patches/PlayerUid.hpp"

namespace Server
{
	BanList::BanList(const std::string &path)
	{
		std::ifstream stream(path);
		Read(stream);
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
				// NOTE: UID banning is NOT implemented yet
				uint64_t uid;
				if (Patches::PlayerUid::ParseUid(components[1], &uid))
					AddUid(uid);
			}
		}
	}

	void BanList::Save(std::ostream &stream) const
	{
		stream << "# ElDewrito server ban list\n";
		stream << "# Players matching the filters in this file will not be allowed to connect to your server.\n\n";
		
		stream << "# IPv4 address bans\n";
		stream << "# Format: ip XXX.XXX.XXX.XXX\n";
		for (auto ip : ipAddresses)
			stream << "ip " << ip << '\n';
		
		/*stream << "\n# UID bans\n";
		stream << "# Format: uid XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
		for (auto uid : uids)
			stream << "uid " << std::hex << std::setw(16) << std::setfill('0') << uid << std::dec << '\n';*/
	}

	void BanList::Save(const std::string &path) const
	{
		std::ofstream stream(path, std::ios::trunc);
		Save(stream);
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