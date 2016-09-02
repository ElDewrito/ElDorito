#pragma once
#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Patches
{
	namespace Ui
	{
		void ApplyAll();
		void ApplyMapNameFixes();
		void ApplyUIResolution();

		void Tick();

		typedef std::function<void(HWND window)> CreateWindowCallback;

		// Registers a function to be called when the game window is created.
		void OnCreateWindow(CreateWindowCallback callback);

		extern bool DialogShow; // todo: put this somewhere better
		extern unsigned int DialogStringId;
		extern int DialogArg1; // todo: figure out a better name for this
		extern int DialogFlags;
		extern unsigned int DialogParentStringId;
		extern void* UIData;

		enum VoiceChatIcon
		{
			None,
			Speaking,
			Available,
			Unavailable,
			PushToTalk
		};

		void ToggleSpeaker(bool newSomeoneSpeaking);
		void SetSpeakingPlayer(std::string speakingPlayer);

		void SetVoIPIcon(VoiceChatIcon newIcon);
		void UpdateVoiceChatHUD();
	}
}
