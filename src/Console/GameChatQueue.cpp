#include "GameChatQueue.hpp"
#include "DirectXHook.hpp"

GameChatQueue::GameChatQueue() : Queue(DirectXHook::COLOR_YELLOW)
{

}

void GameChatQueue::pushLineFromKeyboardToGame(std::string line)
{
	auto& ircBackend = IRCBackend::Instance();
	auto& console = GameConsole::Instance();
	ircBackend.sendMessageToChannel(ircBackend.gameChatChannel, &console.gameChatQueue, line);

	std::string preparedLineForUI = GameConsole::Instance().playerName;
	preparedLineForUI = preparedLineForUI.substr(preparedLineForUI.find_first_of("|") + 1, std::string::npos);
	preparedLineForUI += ": ";
	preparedLineForUI += line;
	pushLineFromGameToUI(preparedLineForUI);
}
