#pragma once
#include "Queue.hpp"
#include <string>
#include <vector>

class IRCBackend {
private:
	std::string server = "irc.snoonet.org";
	std::string globalChatChannel = "";
	std::string gameChatChannel = "";
	char buffer[512];
	SOCKET winSocket;
	DWORD lastTimeReceivedPacket = 0;

	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	bool initIRCChat();
	void ircChatLoop();
	void extractMessageAndSendToUI(std::vector<std::string> &bufferSplitBySpace, Queue* queue);
	bool messageIsInChannel(std::vector<std::string> &bufferSplitBySpace, std::string channel);
	bool receivedPING();
	bool receivedMessageFromIRCServer(std::vector<std::string> &bufferSplitBySpace);
	bool receivedWelcomeMessage(std::vector<std::string> &bufferSplitBySpace);
	void joinIRCChannel(std::string channel, bool globalChat);
	void sendMessageToIRCServer(std::string channel, Queue* queue);

public:
	IRCBackend();
};