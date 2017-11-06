#include "../Server/ServerChat.hpp"

namespace ChatCommands
{
	void Init();
	void Tick();
	bool handleChatMessage(const Server::Chat::ChatMessage &message, int peer);
	bool addToVoteTimes(uint64_t sender);
}