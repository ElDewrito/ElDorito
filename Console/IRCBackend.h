#pragma once
#include <vector>
#include <ws2tcpip.h>

class IRCBackend {
private:
	std::string server = "";
	std::string channel = "";
	bool inChannel = false;
	char buffer[512];
	SOCKET winSocket;
	DWORD lastTimeReceivedPacket = 0;

	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	bool initIRCChat();
	void ircChatLoop();
	void extractMessageAndSendToUI(std::vector<std::string> &bufferSplitBySpace);
	bool messageIsInChannel(std::vector<std::string> &bufferSplitBySpace);
	bool receivedPING();
	bool receivedMessageFromIRCServer(std::vector<std::string> &bufferSplitBySpace);
	bool receivedWelcomeMessage(std::vector<std::string> &bufferSplitBySpace);
	void joinIRCChannel();
	void sendMessageToIRCServer();

public:
	IRCBackend();
};