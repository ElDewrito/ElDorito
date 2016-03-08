#include "GameConsole.hpp"
#include "../Utils/VersionInfo.hpp"
#include "../DirectXHook.hpp"
#include "../Modules/ModulePlayer.hpp"
#include <algorithm>
#include "../Server/ServerChat.hpp"
#include "../Patches/PlayerUid.hpp"
#include "../Patches/Input.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Patches/WebOverlay.hpp"

namespace
{
	class GameChatHandler: public Server::Chat::ChatHandler
	{
	public:
		explicit GameChatHandler(GameChatQueue *gameChat)
			: gameChat(gameChat)
		{
		}

		virtual void MessageSent(int senderPeer, Server::Chat::ChatMessage *message, bool *ignore) override
		{
			Censor(message, "dodger"); // ayyyyy
		}

		virtual void MessageReceived(const Server::Chat::ChatMessage &message) override
		{
			std::string sender;
			if (message.Type == Server::Chat::ChatMessageType::Server)
				sender = "SERVER";
			else
				sender = Utils::String::ThinString(message.Sender);

			std::string line;
			if (message.Type == Server::Chat::ChatMessageType::Team)
				line += "(TEAM) ";
			line += "<" + sender + "> " + Utils::String::Trim(std::string(message.Body));
			Utils::String::RemoveCharsFromString(line, "\t\r\n");
			gameChat->pushLineFromGameToUI(line);
		}

	private:
		void Censor(Server::Chat::ChatMessage *message, const std::string &str)
		{
			std::string body = message->Body;
			Utils::String::ReplaceString(body, str, "BLAM!");
			strncpy_s(message->Body, body.c_str(), sizeof(message->Body) - 1);
			message->Body[sizeof(message->Body) - 1] = 0;
		}

		GameChatQueue *gameChat;
	};

	class GameConsoleInputContext: public Patches::Input::InputContext
	{
	public:
		explicit GameConsoleInputContext(GameConsole* console) : console(console) {}

		virtual void Activated() override
		{
		}

		virtual void Deactivated() override
		{
		}

		virtual bool GameInputTick() override
		{
			// The "done" state is delayed a tick in order to prevent input
			// replication, since the UI can get new keys before the game does
			return done;
		}

		virtual bool UiInputTick() override
		{
			done = console->consoleKeyCallBack();
			return true;
		}

	private:
		GameConsole* console;
		bool done;
	};
}

GameConsole::GameConsole()
{
	auto gameChatHandler = std::make_shared<GameChatHandler>(&gameChatQueue);
	Server::Chat::AddHandler(gameChatHandler);

	PushLineFromGameToUIQueues("ElDewrito Version: " + Utils::Version::GetVersionString() + " Build Date: " + __DATE__ + " " + __TIME__);

	Patches::PlayerUid::Get(); // ensure a UID is generated

	// Register our game input handler
	Patches::Input::RegisterDefaultInputHandler(std::bind(&GameConsole::gameInputCallBack, this));
}

void GameConsole::PushLineFromGameToUIQueues(std::string text)
{
	consoleQueue.pushLineFromGameToUI(text);
}

int GameConsole::getMsSinceLastConsoleOpen()
{
	return GetTickCount() - selectedQueue->lastTimeQueueShown;
}

int GameConsole::getMsSinceLastConsoleBlink()
{
	return GetTickCount() - lastTimeConsoleBlink;
}

void GameConsole::hideConsole()
{
	selectedQueue->lastTimeQueueShown = GetTickCount();
	showChat = false;
	showConsole = false;
	currentInput.set("");
}

void GameConsole::displayChat(bool console)
{
	if (console)
	{
		if (selectedQueue != &consoleQueue)
		{
			lastChatQueue = selectedQueue;
		}
		selectedQueue = &consoleQueue;
		showConsole = true;
	}
	else
	{
		selectedQueue = lastChatQueue;
		showChat = true;
	}

	selectedQueue->startIndexForScrolling = 0;
	tabHitLast = false;

	PushContext(std::make_shared<GameConsoleInputContext>(this));
}

bool GameConsole::consoleKeyCallBack()
{
	Blam::Input::KeyEvent event;
	while (ReadKeyEvent(&event, Blam::Input::eInputTypeUi))
	{
		switch (event.Type)
		{
		case Blam::Input::eKeyEventTypeDown:
			if (!keyDownCallBack(event))
				return false;
			break;
		case Blam::Input::eKeyEventTypeChar:
			if (!keyTypedCallBack(event))
				return false;
			break;
		}
	}
	return true;
}

bool GameConsole::keyDownCallBack(const Blam::Input::KeyEvent& key)
{
	using Blam::Input::KeyCode;

	switch (key.Code)
	{
	case KeyCode::eKeyCodeEnter:
		if (!currentInput.currentInput.empty())
		{
			selectedQueue->unchangingBacklog.push_back(currentInput.currentInput);
			selectedQueue->pushLineFromKeyboardToGame(currentInput.currentInput);
			selectedQueue->startIndexForScrolling = 0;
		}
		hideConsole();
		return false;

	case KeyCode::eKeyCodeEscape:
		hideConsole();
		return false;

	case KeyCode::eKeyCodeBack:
		if (!currentInput.currentInput.empty())
		{
			currentInput.backspace();
		}
		break;

	case KeyCode::eKeyCodeDelete:
		if (!currentInput.currentInput.empty())
		{
			currentInput.del();
		}
		break;

	case KeyCode::eKeyCodeHome:
		if (!currentInput.currentInput.empty())
		{
			currentInput.home();
		}
		break;

	case KeyCode::eKeyCodeEnd:
		if (!currentInput.currentInput.empty())
		{
			currentInput.end();
		}
		break;

	case KeyCode::eKeyCodePageUp:
		if (selectedQueue->startIndexForScrolling < selectedQueue->numOfLinesBuffer - selectedQueue->numOfLinesToShow)
		{
			selectedQueue->startIndexForScrolling++;
		}
		break;

	case KeyCode::eKeyCodePageDown:
		if (selectedQueue->startIndexForScrolling > 0)
		{
			selectedQueue->startIndexForScrolling--;
		}
		break;

	case KeyCode::eKeyCodeUp:
		currentBacklogIndex++;
		if (currentBacklogIndex > (int)selectedQueue->unchangingBacklog.size() - 1)
		{
			currentBacklogIndex--;
		}
		if (currentBacklogIndex >= 0)
		{
			currentInput.currentInput = selectedQueue->unchangingBacklog.at(selectedQueue->unchangingBacklog.size() - currentBacklogIndex - 1);
		}
		break;

	case KeyCode::eKeyCodeDown:
		currentBacklogIndex--;
		if (currentBacklogIndex < 0)
		{
			currentBacklogIndex = -1;
			currentInput.currentInput = "";
		}
		else
		{
			currentInput.currentInput = selectedQueue->unchangingBacklog.at(selectedQueue->unchangingBacklog.size() - currentBacklogIndex - 1);
		}
		break;

	case KeyCode::eKeyCodeLeft:
		currentInput.left();
		break;

	case KeyCode::eKeyCodeRight:
		currentInput.right();
		break;

	case KeyCode::eKeyCodeTab:
		if (currentInput.currentInput.find_first_of(" ") == std::string::npos && currentInput.currentInput.length() > 0)
		{
			if (tabHitLast)
			{
				if (currentCommandList.size() > 0)
				{
					currentInput.set(currentCommandList.at((++tryCount) % currentCommandList.size()));
				}
			}
			else
			{
				tryCount = 0;
				currentCommandList.clear();
				commandPriorComplete = currentInput.currentInput;

				auto currentLine = currentInput.currentInput;
				std::transform(currentLine.begin(), currentLine.end(), currentLine.begin(), ::tolower);

				for (auto cmd : Modules::CommandMap::Instance().Commands)
				{
					auto commandName = cmd.Name;
					std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);

					if (commandName.compare(0, currentLine.length(), currentLine) == 0)
					{
						currentCommandList.push_back(commandName);
					}
				}
				consoleQueue.pushLineFromGameToUI(std::to_string(currentCommandList.size()) + " commands found starting with \"" + currentLine + ".\"");
				consoleQueue.pushLineFromGameToUI("Press tab to go through them.");
			}
		}
		break;

	case KeyCode::eKeyCodeV:
		if (!(key.Modifiers & Blam::Input::eKeyEventModifiersCtrl)) // CTRL+V pasting
			break;
		if (!OpenClipboard(nullptr))
			break;
		auto hData = GetClipboardData(CF_TEXT);
		if (hData)
		{
			auto textPointer = static_cast<char*>(GlobalLock(hData));
			for (size_t i = 0; textPointer[i]; i++)
			{
				auto ch = textPointer[i];
				if (ch == '\r' || ch == '\n' || ch == '\t')
					continue;
				if (currentInput.currentInput.size() >= INPUT_MAX_CHARS)
					break;
				currentInput.type(ch);
			}
			GlobalUnlock(hData);
		}
		CloseClipboard();
		break;
	}
	tabHitLast = (key.Code == KeyCode::eKeyCodeTab);
	return true;
}

bool GameConsole::keyTypedCallBack(const Blam::Input::KeyEvent& key)
{
	if (key.Char >= 0 && key.Char <= 127 && currentInput.currentInput.size() < INPUT_MAX_CHARS)
		currentInput.type(static_cast<char>(key.Char));
	return true;
}

// TODO: Get this working again
void GameConsole::mouseCallBack(RAWMOUSE mouseInfo)
{
	if (mouseInfo.usButtonFlags == RI_MOUSE_WHEEL)
	{
		if ((short) mouseInfo.usButtonData > 0)
		{
			if (selectedQueue->startIndexForScrolling < selectedQueue->numOfLinesBuffer - selectedQueue->numOfLinesToShow)
			{
				selectedQueue->startIndexForScrolling++;
			}
		}
		else
		{
			if (selectedQueue->startIndexForScrolling > 0)
			{
				selectedQueue->startIndexForScrolling--;
			}
		}
	}
}

void GameConsole::gameInputCallBack()
{
	using namespace Blam::Input;

	BindingsTable bindings;
	GetBindings(0, &bindings);

	// TODO: Change the input type for these actions to eInputTypeUi somehow so
	// that they can be bound to controllers...
	auto primaryChatKey = bindings.PrimaryKeys[eGameActionGeneralChat];
	auto secondaryChatKey = bindings.SecondaryKeys[eGameActionGeneralChat];
	auto primaryTeamChatKey = bindings.PrimaryKeys[eGameActionTeamChat];
	auto secondaryTeamChatKey = bindings.SecondaryKeys[eGameActionTeamChat];

	if (!disableUI)
	{
		if ((primaryChatKey != eKeyCode_None && GetKeyTicks(primaryChatKey, eInputTypeUi)) ||
			(secondaryChatKey != eKeyCode_None && GetKeyTicks(secondaryChatKey, eInputTypeUi)))
		{
			displayChat(false);
		}
	}

	if (!disableUI && gameChatQueue.visible)
	{
		if ((primaryTeamChatKey != eKeyCode_None && GetKeyTicks(primaryTeamChatKey, eInputTypeUi)) ||
			(secondaryTeamChatKey != eKeyCode_None && GetKeyTicks(secondaryTeamChatKey, eInputTypeUi)))
		{
			displayChat(false);
			currentInput.type("!team ");
		}
	}

	// TODO: Make these rebindable!

	if (!disableUI && (GetKeyTicks(eKeyCodeTilde, eInputTypeUi) == 1 || GetKeyTicks(eKeyCodeF1, eInputTypeUi) == 1))
		displayChat(true);

	if (GetKeyTicks(eKeyCodeF9, eInputTypeUi) == 1)
		DirectXHook::helpMessageStartTime = GetTickCount();

	if (GetKeyTicks(eKeyCodeF10, eInputTypeUi) == 1)
		disableUI = !disableUI;

	// TODO: Should we keep this since we have the server browser option on the menu now?
	if (GetKeyTicks(eKeyCodeF11, eInputTypeUi) == 1)
		Patches::WebOverlay::ShowScreen("example", "{}");

	if (GetKeyTicks(eKeyCodeF12, eInputTypeUi) == 1)
		DirectXHook::drawVoIPSettings = !DirectXHook::drawVoIPSettings;
}