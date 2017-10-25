#include "WhiteList.hpp"

#include <fstream>
#include <iomanip>
#include "../Utils/String.hpp"
#include "../Patches/PlayerUid.hpp"
#include "../Modules/ModuleServer.hpp"

namespace Server
{
	WhiteList::WhiteList(const std::string &path)
	{
		std::ifstream stream(path);
		Read(stream);
	}

	void WhiteList::Read(std::istream &stream)
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

	void WhiteList::Save(std::ostream &stream) const
	{
		stream << "# ElDewrito server white list\n";
		stream << "# Players matching the filters in this file will not be considered to be kicked in votes.\n\n";

		stream << "# IPv4 address whitelist\n";
		stream << "# Format: ip XXX.XXX.XXX.XXX\n";
		for (auto ip : ipAddresses)
			stream << "ip " << ip << '\n';

		/*stream << "\n# UID whitelist\n";
		stream << "# Format: uid XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
		for (auto uid : uids)
			stream << "uid " << std::hex << std::setw(16) << std::setfill('0') << uid << std::dec << '\n';*/
	}

	void WhiteList::Save(const std::string &path) const
	{
		std::ofstream stream(path, std::ios::trunc);
		Save(stream);
	}

	WhiteList LoadDefaultWhiteList()
	{
		std::ifstream file(DefaultWhiteListPath);
		if (file)
			return WhiteList(file);
		return WhiteList();
	}

	void SaveDefaultWhiteList(const WhiteList &list)
	{
		list.Save(DefaultWhiteListPath);
	}
}
