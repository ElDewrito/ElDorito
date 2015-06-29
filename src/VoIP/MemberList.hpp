#pragma once
#include <string>
#include <vector>

class MemberList
{
public:
	std::vector<std::string> memberList = std::vector<std::string>{};

	void ShowPlayerTalkEvent(std::string name);
	void HidePlayerTalkEvent(std::string name);
};