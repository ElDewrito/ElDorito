#include "GlobalChatQueue.h"
#include "GameConsole.h"
#include "DirectXHook.h"

GlobalChatQueue::GlobalChatQueue() : Queue(DirectXHook::COLOR_YELLOW)
{

}

void GlobalChatQueue::pushLineFromKeyboardToGame(std::string line)
{
	std::string preparedLineForIRC = GameConsole::Instance().playerName;
	preparedLineForIRC += ": ";
	preparedLineForIRC += line;
	sendThisLineToIRCServer = preparedLineForIRC;
	pushLineFromGameToUI(preparedLineForIRC);
}