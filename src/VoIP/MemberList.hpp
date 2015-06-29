#pragma once
#include <string>
#include <vector>
#include "..\Utils\Singleton.hpp"

class MemberList : public Utils::Singleton<MemberList>
{
public:
	std::vector<std::string> memberList = std::vector<std::string>{};

	void ShowPlayerTalkEvent(std::string name);
	void HidePlayerTalkEvent(std::string name);
};