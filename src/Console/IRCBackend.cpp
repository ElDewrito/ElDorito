#include <winsock2.h>
#include <ws2tcpip.h>
#include "IRCBackend.hpp"
#include "GameConsole.hpp"
#include <algorithm>
#include <sstream>
#include "../Modules/ModuleIRC.hpp"

IRCBackend::IRCBackend()
{
	std::transform(globalChatChannel.begin(), globalChatChannel.end(), globalChatChannel.begin(), ::tolower);

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
		return;
	}

	ircChatLoop();
	closesocket(winSocket);
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
	int retVal;
	auto& console = GameConsole::Instance();
	
	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	auto& ircvars = Modules::ModuleIRC::Instance();
	if (retVal = getaddrinfo(ircvars.VarIRCServer->ValueString.c_str(), ircvars.VarIRCServerPort->ValueString.c_str(), &hints, &ai))
	{
		console.globalChatQueue.pushLineFromGameToUI(std::string("IRC Error: ").append(gai_strerror(retVal)));
		return false;
	}
	winSocket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (retVal = connect(winSocket, ai->ai_addr, ai->ai_addrlen))
	{
		console.globalChatQueue.pushLineFromGameToUI(std::string("IRC Error: ").append(gai_strerror(retVal)));
		return false;
	}
	freeaddrinfo(ai);
	sprintf_s(buffer, "USER %s 0 * :#ElDorito player\r\nNICK %s\r\n", console.playerName.c_str(), console.playerName.c_str());
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
		// OutputDebugString(buffer);

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

		if (!globalChatChannel.empty() && !console.globalChatQueue.sendThisLineToIRCServer.empty())
		{
			sendMessageToIRCServer(globalChatChannel, &console.globalChatQueue);
		}

		if (!gameChatChannel.empty() && !console.gameChatQueue.sendThisLineToIRCServer.empty())
		{
			sendMessageToIRCServer(gameChatChannel, &console.gameChatQueue);
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
#ifdef _DEBUG
				joinIRCChannel("#haloonline-dev", true);
#else
				joinIRCChannel("#haloonline", true);
#endif
				console.globalChatQueue.pushLineFromGameToUI("Connected to global chat!");
			}
			else if (receivedMessageFromIRCServer(bufferSplitBySpace))
			{
				if (messageIsInChannel(bufferSplitBySpace, globalChatChannel))
				{
					extractMessageAndSendToUI(bufferSplitBySpace, &console.globalChatQueue);
				}
				else if (messageIsInChannel(bufferSplitBySpace, gameChatChannel))
				{
					extractMessageAndSendToUI(bufferSplitBySpace, &console.gameChatQueue);
				}
			}
		}

		if (globalChatChannel.empty() && lastTimeReceivedPacket != 0 && GetTickCount() - lastTimeReceivedPacket > 5000)
		{
#ifdef _DEBUG
			joinIRCChannel("#haloonline-dev", true);
#else
			joinIRCChannel("#haloonline", true);
#endif
			console.globalChatQueue.pushLineFromGameToUI("Connected to global chat!");
		}

		Sleep(100);
	}
}

void IRCBackend::sendMessageToIRCServer(std::string channel, Queue* queue)
{
	auto& console = GameConsole::Instance();
	sprintf_s(buffer, "PRIVMSG %s :%s\r\n", channel.c_str(), queue->sendThisLineToIRCServer.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	queue->sendThisLineToIRCServer.clear();
}

void IRCBackend::joinIRCChannel(std::string channel, bool globalChat)
{
	auto& console = GameConsole::Instance();
	
	if (globalChat)
	{
		globalChatChannel = channel;
	}
	else
	{
		leaveIRCChannel(gameChatChannel);
		gameChatChannel = channel;
	}

	sprintf_s(buffer, "MODE %s +B\r\nJOIN %s\r\n", console.playerName.c_str(), channel.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
}

void IRCBackend::leaveIRCChannel(std::string channel)
{
	sprintf_s(buffer, "MODE %s +B\r\nPART %s\r\n", GameConsole::Instance().playerName.c_str(), channel.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	gameChatChannel = "";
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

bool IRCBackend::messageIsInChannel(std::vector<std::string> &bufferSplitBySpace, std::string channel)
{
	if (bufferSplitBySpace.size() <= 2)
		return false;
	std::transform(bufferSplitBySpace.at(2).begin(), bufferSplitBySpace.at(2).end(), bufferSplitBySpace.at(2).begin(), ::tolower);
	return strncmp(bufferSplitBySpace.at(2).c_str(), channel.c_str(), channel.length()) == 0;
}

void IRCBackend::extractMessageAndSendToUI(std::vector<std::string> &bufferSplitBySpace, Queue* queue)
{
	if (bufferSplitBySpace.size() <= 3)
		return;
	std::string buf(buffer);
	std::string message = buf.substr(buf.find(bufferSplitBySpace.at(3)), buf.length());
	message.erase(0, 1); // remove first character ":"
	message.resize(message.size() - 2); // remove last 2 characters "\n"
	
	std::string preparedLineForUI = bufferSplitBySpace.at(0).substr(bufferSplitBySpace.at(0).find_first_of("|") + 1, std::string::npos);
	preparedLineForUI = preparedLineForUI.substr(0, preparedLineForUI.find_first_of("!"));
	preparedLineForUI += ": " + message;
	queue->pushLineFromGameToUI(preparedLineForUI);
}