#include "MemberList.hpp"

void MemberList::ShowPlayerTalkEvent(std::string name)
{
	memberList.push_back(name);
}

void MemberList::HidePlayerTalkEvent(std::string name)
{
	for (size_t i = 0; i < memberList.size(); i++)
	{
		if (memberList.at(i) == name)
		{
			memberList.erase(memberList.begin() + i);
			return;
		}
	}
}