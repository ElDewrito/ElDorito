#include "GameChatQueue.hpp"
#include "DirectXHook.hpp"

GameChatQueue::GameChatQueue() : Queue(DirectXHook::COLOR_YELLOW)
{

}

void GameChatQueue::pushLineFromKeyboardToGame(std::string line)
{
	sendThisLineToIRCServer = line;

	std::string preparedLineForUI = GameConsole::Instance().playerName;
	preparedLineForUI = preparedLineForUI.substr(preparedLineForUI.find_first_of("|") + 1, std::string::npos);
	preparedLineForUI += ": ";
	preparedLineForUI += line;
	pushLineFromGameToUI(preparedLineForUI);
}
