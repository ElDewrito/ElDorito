#include "GlobalChatQueue.hpp"
#include "GameConsole.hpp"
#include "DirectXHook.hpp"

GlobalChatQueue::GlobalChatQueue() : Queue(
#ifdef _DEBUG
	DirectXHook::COLOR_YELLOW
#else
	DirectXHook::COLOR_GREEN
#endif
	)
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