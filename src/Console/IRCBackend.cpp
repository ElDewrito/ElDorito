#include <winsock2.h>
#include <ws2tcpip.h>
#include "IRCBackend.hpp"
#include "GameConsole.hpp"
#include <algorithm>
#include <sstream>

IRCBackend::IRCBackend()
{
	std::transform(channel.begin(), channel.end(), channel.begin(), ::tolower);

	int i = 0;

	for (; i < 3 && !initIRCChat(); i++)
	{
		if (i >= 2)
		{
			GameConsole::Instance().globalChatQueue.pushLineFromGameToUI("Error: failed to connect to IRC.");
		}
		else
		{
			GameConsole::Instance().globalChatQueue.pushLineFromGameToUI("Error: failed to connect to IRC. Retrying in 5 seconds.");
			Sleep(5000);
		}
	}

	if (i >= 3)
	{
		closesocket(winSocket);
		WSACleanup();
		return;
	}

	ircChatLoop();
	closesocket(winSocket);
	WSACleanup();
}

std::vector<std::string> &IRCBackend::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

bool IRCBackend::initIRCChat()
{
	int ret;
	struct WSAData* wd = (struct WSAData*)malloc(sizeof(struct WSAData));
	ret = WSAStartup(MAKEWORD(2, 0), wd);
	free(wd);

	auto& console = GameConsole::Instance();
	if (ret)
	{
		console.globalChatQueue.pushLineFromGameToUI("Error loading Windows Socket API");
		return false;
	}
	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (ret = getaddrinfo(server.c_str(), "6667", &hints, &ai))
	{
		console.globalChatQueue.pushLineFromGameToUI(std::string("IRC Error: ").append(gai_strerror(ret)));
		return false;
	}
	winSocket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (ret = connect(winSocket, ai->ai_addr, ai->ai_addrlen))
	{
		console.globalChatQueue.pushLineFromGameToUI(std::string("IRC Error: ").append(gai_strerror(ret)));
		return false;
	}
	freeaddrinfo(ai);
	sprintf_s(buffer, "USER %s 0 * :null\r\n", console.playerName.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	sprintf_s(buffer, "NICK %s\r\n", console.playerName.c_str());
	send(winSocket, buffer, strlen(buffer), 0);

	u_long iMode = 1;
	ioctlsocket(winSocket, FIONBIO, &iMode); // sets the socket to non-blocking mode
	return true;
}

void IRCBackend::ircChatLoop()
{
	auto& console = GameConsole::Instance();

	while (true) {
		int inDataLength = recv(winSocket, buffer, 512, 0);
		
		// Use below line to debug IRC backend
		// OutputDebugString(buf);

		int nError = WSAGetLastError();
		if (nError != WSAEWOULDBLOCK && nError != 0)
		{
			std::string errorString("Winsock error code: ");
			errorString.append(std::to_string(nError));
			console.globalChatQueue.pushLineFromGameToUI(errorString);
			break;
		}

		if (inDataLength != -1 && inDataLength < 512)
		{
			buffer[inDataLength] = '\0';
		}

		if (inChannel && !console.globalChatQueue.sendThisLineToIRCServer.empty())
		{
			sendMessageToIRCServer();
		}

		if (inDataLength > 0) // received packet from IRC server
		{
			lastTimeReceivedPacket = GetTickCount();

			if (receivedPING())
			{
				buffer[1] = 'O';
				send(winSocket, buffer, strlen(buffer), 0);
			}

			std::vector<std::string> bufferSplitBySpace;
			split(buffer, ' ', bufferSplitBySpace);

			if (receivedWelcomeMessage(bufferSplitBySpace))
			{
				joinIRCChannel();

			}
			else if (receivedMessageFromIRCServer(bufferSplitBySpace))
			{
				if (messageIsInChannel(bufferSplitBySpace))
				{
					extractMessageAndSendToUI(bufferSplitBySpace);
				}
			}
		}

		if (!inChannel && lastTimeReceivedPacket != 0 && GetTickCount() - lastTimeReceivedPacket > 5000)
		{
			joinIRCChannel();
		}

		Sleep(100);
	}
}

void IRCBackend::sendMessageToIRCServer()
{
	auto& console = GameConsole::Instance();
	sprintf_s(buffer, "PRIVMSG %s :%s\r\n", channel.c_str(), console.globalChatQueue.sendThisLineToIRCServer.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	console.globalChatQueue.sendThisLineToIRCServer.clear();
}

void IRCBackend::joinIRCChannel()
{
	auto& console = GameConsole::Instance();
	sprintf_s(buffer, "MODE %s +B\r\nJOIN %s\r\n", console.playerName.c_str(), channel.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	inChannel = true;
	console.globalChatQueue.pushLineFromGameToUI("Connected to global chat!");
}

bool IRCBackend::receivedWelcomeMessage(std::vector<std::string> &bufferSplitBySpace)
{
	if (bufferSplitBySpace.size() <= 1)
		return false;
	return strncmp(bufferSplitBySpace.at(1).c_str(), "001", 3) == 0;
}

bool IRCBackend::receivedMessageFromIRCServer(std::vector<std::string> &bufferSplitBySpace)
{
	if (bufferSplitBySpace.size() <= 1)
		return false;
	return strncmp(bufferSplitBySpace.at(1).c_str(), "PRIVMSG", 7) == 0;
}

bool IRCBackend::receivedPING()
{
	return strncmp(buffer, "PING ", 5) == 0;
}

bool IRCBackend::messageIsInChannel(std::vector<std::string> &bufferSplitBySpace)
{
	if (bufferSplitBySpace.size() <= 2)
		return false;
	std::transform(bufferSplitBySpace.at(2).begin(), bufferSplitBySpace.at(2).end(), bufferSplitBySpace.at(2).begin(), ::tolower);
	return strncmp(bufferSplitBySpace.at(2).c_str(), channel.c_str(), channel.length()) == 0;
}

void IRCBackend::extractMessageAndSendToUI(std::vector<std::string> &bufferSplitBySpace)
{
	if (bufferSplitBySpace.size() <= 3)
		return;
	std::string buffer(buffer);
	std::string message = buffer.substr(buffer.find(bufferSplitBySpace.at(3)), buffer.length());
	message.erase(0, 1); // remove first character
	message.resize(message.size() - 2); // remove last 2 characters
	GameConsole::Instance().globalChatQueue.pushLineFromGameToUI(message);
}