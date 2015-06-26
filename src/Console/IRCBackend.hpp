#pragma once
#include "Queue.hpp"
#include <string>
#include <vector>
#include "../Utils/Singleton.hpp"

class IRCBackend : public Utils::Singleton<IRCBackend>
{
private:
	char buffer[513];
	SOCKET winSocket;

	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems, bool keepDelimiter);
	bool initIRCChat();
	void ircChatLoop();
	void extractMessageAndSendToUI(std::vector<std::string> &bufferSplitBySpace, Queue* queue, size_t msgPos = 3, bool topic = false);
	bool messageIsInChannel(std::vector<std::string> &bufferSplitBySpace, std::string channel, size_t channelPos = 2);
	bool receivedPING(std::string line);
	bool receivedMessageFromIRCServer(std::vector<std::string> &bufferSplitBySpace);
	bool receivedWelcomeMessage(std::vector<std::string> &bufferSplitBySpace);
	bool receivedChannelTopic(std::vector<std::string> &bufferSplitBySpace);

public:
	std::string globalChatChannel = "";
	std::string gameChatChannel = "";

	IRCBackend();
	void joinIRCChannel(std::string channel, bool globalChat);
	void leaveIRCChannel(std::string channel);
	void sendMessageToChannel(std::string channel, Queue* queue, std::string line);
	void KickUser(std::string userNick);
};