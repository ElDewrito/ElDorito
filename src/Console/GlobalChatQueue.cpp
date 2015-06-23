#include "GlobalChatQueue.hpp"
#include "GameConsole.hpp"
#include "DirectXHook.hpp"

GlobalChatQueue::GlobalChatQueue() : Queue(DirectXHook::COLOR_YELLOW)
{

}

void GlobalChatQueue::pushLineFromKeyboardToGame(std::string line)
{
	sendThisLineToIRCServer = line;

	std::string preparedLineForUI = GameConsole::Instance().playerName;
	preparedLineForUI = preparedLineForUI.substr(preparedLineForUI.find_first_of("|") + 1, std::string::npos);
	preparedLineForUI += ": ";
	preparedLineForUI += line;
	pushLineFromGameToUI(preparedLineForUI);
}