#include <winsock2.h>
#include <ws2tcpip.h>
#include "IRCBackend.hpp"
#include "GameConsole.hpp"
#include <algorithm>
#include <sstream>
#include "../Modules/ModuleIRC.hpp"

IRCBackend::IRCBackend()
{
	for (int i = 0; !initIRCChat(); i++)
	{
		if (i >= 2)
		{
			GameConsole::Instance().globalChatQueue.pushLineFromGameToUI("Error: failed to connect to IRC.");
			closesocket(winSocket);
			return;
		}
		else
		{
			GameConsole::Instance().globalChatQueue.pushLineFromGameToUI("Error: failed to connect to IRC. Retrying in 5 seconds.");
			Sleep(5000);
		}
	}

	for (int i = 0; ; i++)
	{
		ircChatLoop();

		if (i >= 2)
		{
			GameConsole::Instance().globalChatQueue.pushLineFromGameToUI("Error: failed to loop in IRC.");
			break;
		}
		else
		{
			GameConsole::Instance().globalChatQueue.pushLineFromGameToUI("Error: failed to loop in IRC. Retrying in 5 seconds.");
			Sleep(5000);
		}
	}
	closesocket(winSocket);
}

std::vector<std::string> &IRCBackend::split(const std::string &s, char delim, std::vector<std::string> &elems, bool keepDelimiter)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (keepDelimiter)
		{
			elems.push_back(item + delim);
		}
		else
		{
			elems.push_back(item);
		}
	}
	return elems;
}

bool IRCBackend::initIRCChat()
{
	int retVal;
	auto& console = GameConsole::Instance();
	
	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// HACK: If the IRC server is set to SnooNet, switch it to qmarchi's
	// Otherwise people with existing cfg files will still be stuck on SnooNet
	auto& ircvars = Modules::ModuleIRC::Instance();
	if (ircvars.VarIRCServer->ValueString == "irc.snoonet.org")
	{
		ircvars.VarIRCServer->ValueString = "irc.justsomegamers.com";
		Modules::CommandMap::Instance().ExecuteCommand("WriteConfig"); // ugh
	}
	
	if (retVal = getaddrinfo(ircvars.VarIRCServer->ValueString.c_str(), ircvars.VarIRCServerPort->ValueString.c_str(), &hints, &ai))
	{
		console.globalChatQueue.pushLineFromGameToUI("IRC GAI error: " + std::string(gai_strerror(retVal)) + " (" + std::to_string(retVal) + "/" + std::to_string(WSAGetLastError()) + ")");
		return false;
	}
	winSocket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (retVal = connect(winSocket, ai->ai_addr, ai->ai_addrlen))
	{
		console.globalChatQueue.pushLineFromGameToUI("IRC connect error: " + std::string(gai_strerror(retVal)) + " (" + std::to_string(retVal) + "/" + std::to_string(WSAGetLastError()) + ")");
		return false;
	}
	freeaddrinfo(ai);
	sprintf_s(buffer, "USER %s 0 * :#ElDorito player\r\n", console.ircName.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	sprintf_s(buffer, "NICK %s\r\n", console.ircName.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	return true;
}

void IRCBackend::ircChatLoop()
{
	auto& console = GameConsole::Instance();

	int inDataLength;

	while ((inDataLength = recv(winSocket, buffer, 512, 0)) > 0) { // received a packet from IRC server
		buffer[inDataLength] = '\0'; // recv function doesn't put a null-terminator character

		std::vector<std::string> bufferSplitByNewLines;
		split(buffer, '\n', bufferSplitByNewLines, true);

		for (size_t i = 0; i < bufferSplitByNewLines.size(); i++)
		{
			// OutputDebugString(bufferSplitByNewLines.at(i).c_str()); // use this line to debug IRC backend

			if (receivedPING(bufferSplitByNewLines.at(i)))
			{
				std::string sendBuffer = bufferSplitByNewLines.at(i);
				sendBuffer[1] = 'O'; // modify PING to PONG
				send(winSocket, sendBuffer.c_str(), strlen(sendBuffer.c_str()), 0);
			}

			std::vector<std::string> bufferSplitBySpace;
			split(bufferSplitByNewLines.at(i), ' ', bufferSplitBySpace, false);

			if (receivedWelcomeMessage(bufferSplitBySpace))
			{
				joinIRCChannel(Modules::ModuleIRC::Instance().VarIRCGlobalChannel->ValueString);
				console.globalChatQueue.pushLineFromGameToUI("Connected to global chat!");
			}
			else if (receivedChannelTopic(bufferSplitBySpace))
			{
				if (messageIsInChannel(bufferSplitBySpace, globalChatChannel, 3))
				{
					extractMessageAndSendToUI(bufferSplitBySpace, &console.globalChatQueue, 4, true);
				}
			}
			else if (receivedMessageFromIRCServer(bufferSplitBySpace))
			{
				if (messageIsInChannel(bufferSplitBySpace, globalChatChannel))
					extractMessageAndSendToUI(bufferSplitBySpace, &console.globalChatQueue);
			}
			else if (bufferSplitByNewLines.at(i).find("Erroneous Nickname") != std::string::npos)
			{
				GameConsole::Instance().globalChatQueue.pushLineFromGameToUI("Error: invalid username.");
			}
		}
	}

	int nError = WSAGetLastError();
	std::string errorString("Winsock error code: ");
	errorString.append(std::to_string(nError));
	console.globalChatQueue.pushLineFromGameToUI(errorString);
}

void IRCBackend::sendMessageToChannel(std::string channel, Queue* queue, std::string line)
{
	auto& console = GameConsole::Instance();
	sprintf_s(buffer, "PRIVMSG %s :%s\r\n", channel.c_str(), line.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	line.clear();
}

void IRCBackend::joinIRCChannel(std::string channel)
{
	std::transform(channel.begin(), channel.end(), channel.begin(), ::tolower);
	
	globalChatChannel = channel;

	sprintf_s(buffer, "MODE %s +BIc\r\nJOIN %s\r\n", GameConsole::Instance().ircName.c_str(), channel.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
}

void IRCBackend::leaveIRCChannel(std::string channel)
{
	sprintf_s(buffer, "MODE %s +BIc\r\nPART %s\r\n", GameConsole::Instance().ircName.c_str(), channel.c_str());
	send(winSocket, buffer, strlen(buffer), 0);
	globalChatChannel = "";
}

bool IRCBackend::receivedWelcomeMessage(std::vector<std::string> &bufferSplitBySpace)
{
	if (bufferSplitBySpace.size() <= 1)
		return false;
	return strncmp(bufferSplitBySpace.at(1).c_str(), "001", 3) == 0;
}

bool IRCBackend::receivedChannelTopic(std::vector<std::string> &bufferSplitBySpace)
{
	if (bufferSplitBySpace.size() <= 1)
		return false;
	return strncmp(bufferSplitBySpace.at(1).c_str(), "332", 3) == 0;
}

bool IRCBackend::receivedMessageFromIRCServer(std::vector<std::string> &bufferSplitBySpace)
{
	if (bufferSplitBySpace.size() <= 1)
		return false;
	return strncmp(bufferSplitBySpace.at(1).c_str(), "PRIVMSG", 7) == 0;
}

bool IRCBackend::receivedPING(std::string line)
{
	return strncmp(line.c_str(), "PING ", 5) == 0;
}

bool IRCBackend::messageIsInChannel(std::vector<std::string> &bufferSplitBySpace, std::string channel, size_t channelPos)
{
	if (bufferSplitBySpace.size() <= channelPos)
		return false;
	std::transform(bufferSplitBySpace.at(channelPos).begin(), bufferSplitBySpace.at(channelPos).end(), bufferSplitBySpace.at(channelPos).begin(), ::tolower);
	return strncmp(bufferSplitBySpace.at(channelPos).c_str(), channel.c_str(), channel.length()) == 0;
}

void IRCBackend::extractMessageAndSendToUI(std::vector<std::string> &bufferSplitBySpace, Queue* queue, size_t msgPos, bool topic)
{
	if (bufferSplitBySpace.size() <= msgPos)
		return;
	std::string buf(buffer);
	std::string message = buf.substr(buf.find(bufferSplitBySpace.at(msgPos)), buf.length());
	message.erase(0, 1); // remove first character ":"
	message.resize(message.size() - 2); // remove last 2 characters "\n"
	if (message.find("\r\n") != std::string::npos)
		message = message.substr(0, message.find("\r\n"));

	std::string preparedLineForUI = bufferSplitBySpace.at(0).substr(bufferSplitBySpace.at(0).find_first_of("|") + 1, std::string::npos);
	preparedLineForUI = "<" + preparedLineForUI.substr(0, preparedLineForUI.find_first_of("!"));
	preparedLineForUI += "> " + message;
	if (topic)
		preparedLineForUI = "Channel topic: " + message;

	Utils::String::RemoveCharsFromString(preparedLineForUI, "\t\r\n");
	queue->pushLineFromGameToUI(preparedLineForUI);
}