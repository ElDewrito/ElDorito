#include "Ui.hpp"

#include "../ElDorito.hpp"
#include "../Patch.hpp"
#include "../Patches/Core.hpp"
#include "../Patches/Input.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/UI/ChudGlobalsDefinition.hpp"
#include "../Blam/Tags/UI/ChudDefinition.hpp"
#include "../Blam/Tags/Items/Weapon.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Modules/ModuleGraphics.hpp"
#include "../Modules/ModuleInput.hpp"
#include "../Modules/ModuleGame.hpp"
#include "../Web/Ui/ScreenLayer.hpp"
#include "../Blam/Tags/UI/MultilingualUnicodeStringList.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include <cassert>

using namespace Patches::Ui;

namespace
{
	void __fastcall UI_MenuUpdateHook(void* a1, int unused, int menuIdToLoad);

	int UI_ShowHalo3PauseMenu(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	void UI_EndGame();
	char __fastcall UI_Forge_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct);
	void LocalizedStringHook();
	void LobbyMenuButtonHandlerHook();
	void WindowTitleSprintfHook(char* destBuf, char* format, char* version);
	void ResolutionChangeHook();
	int __fastcall UI_SetPlayerDesiredTeamHook(void *thisPtr, int unused, int a2, int teamIndex, int a4);
	void OnUiInputUpdated();
	void __fastcall UI_UpdateRosterColorsHook(void *thisPtr, int unused, void *a0);
	HWND CreateGameWindowHook();
	void __cdecl UI_UpdateH3HUDHook(int playerMappingIndex);
	void GetActionButtonNameHook();
	void UI_GetHUDGlobalsIndexHook();
	void __fastcall c_main_menu_screen_widget_item_select_hook(void* thisptr, void* unused, int a2, int a3, void* a4, void* a5);
	void __fastcall c_ui_view_draw_hook(void* thisptr, void* unused);

	std::vector<CreateWindowCallback> createWindowCallbacks;

	Patch unused; // for some reason a patch field is needed here (on release builds) otherwise the game crashes while loading map/game variants, wtf?

	bool tagsInitiallyLoaded = false;
	VoiceChatIcon micState = VoiceChatIcon::Unavailable;
	bool someoneSpeaking = false;
	const std::string SPEAKING_PLAYER_STRING_NAME = "speaking_player";
	int32_t speakingPlayerStringID;

	int localDesiredTeam = -1;
}

namespace Patches::Ui
{
	void ApplyAfterTagsLoaded()
	{
		if (!tagsInitiallyLoaded)
		{
			// use the correct hud globals for the player representation
			if (Modules::ModuleGame::Instance().VarFixHudGlobals->ValueInt)
				Hook(0x6895E7, UI_GetHUDGlobalsIndexHook).Apply();
		}

		tagsInitiallyLoaded = true;
	}

	const auto UI_Alloc = reinterpret_cast<void *(__cdecl *)(int32_t)>(0xAB4ED0);
	const auto UI_OpenDialogById = reinterpret_cast<void *(__thiscall *)(void *, Blam::Text::StringID, int32_t, int32_t, Blam::Text::StringID)>(0xA92780);
	const auto UI_PostMessage = reinterpret_cast<int(*)(void *)>(0xA93450);

	void *ShowDialog(const Blam::Text::StringID p_DialogID, const int32_t p_Arg1, const int32_t p_Flags, const Blam::Text::StringID p_ParentID)
	{
		auto *s_UIData = UI_Alloc(0x40);

		if (!s_UIData)
			return nullptr;

		UI_OpenDialogById(s_UIData, p_DialogID, p_Arg1, p_Flags, p_ParentID);
		UI_PostMessage(s_UIData);

		return s_UIData;
	}

	void OnCreateWindow(CreateWindowCallback callback)
	{
		createWindowCallbacks.push_back(callback);
	}

	void SetVoiceChatIcon(VoiceChatIcon newIcon)
	{
		micState = newIcon;
		UpdateVoiceChatHUD();
	}

	void ToggleSpeakingPlayer(bool newSomeoneSpeaking)
	{
		someoneSpeaking = newSomeoneSpeaking;
		UpdateVoiceChatHUD();
	}

	bool firstStringUpdate = true;
	int speakingPlayerOffset; //The offset of speaker_name in memory.
	int speakingPlayerIndex = 0; //Hud Widget Containing Speaker.
	static const char* const hex = "0123456789ABCDEF";
	std::stringstream hexStringStream;

	void SetSpeakingPlayer(std::string speakingPlayer)
	{
		if (!tagsInitiallyLoaded) //Commands might call methods a little early.
			return;


		//This method could use some refactoring, and performance could be improved I think.
		using Blam::Tags::TagInstance;
		using Blam::Tags::UI::MultilingualUnicodeStringList;
		using Blam::Tags::UI::ChudDefinition;

		//If the client is speaking, hide the text and let icons display.
		//Really this method shouldn't be called with the client's username, but here's a half working fix for that if necessary.
		//if (speakingPlayer == username)
		//{
		//	ToggleSpeakingPlayer(false);
		//	return;
		//}

		if (speakingPlayer.length() > 15) // player names are limited to 15 anyway.
			return;

		auto *unic = Blam::Tags::TagInstance(0x12c2).GetDefinition<Blam::Tags::UI::MultilingualUnicodeStringList>();

		if (firstStringUpdate)
		{
			//go through string blocks backwards to find speaking_player, as it should be at the end.
			for (int stringBlockIndex = unic->Strings.Count - 1; stringBlockIndex > -1; stringBlockIndex--)
				if (SPEAKING_PLAYER_STRING_NAME == (std::string)unic->Strings[stringBlockIndex].StringIDStr)
				{
					speakingPlayerOffset = unic->Strings[stringBlockIndex].Offsets[0]; //read the english offset,
					speakingPlayerStringID = unic->Strings[stringBlockIndex].StringID;
					break;
				}

			//If the speaking_player string cannot be found, RIP. This shouldn't happen unless tags don't have correct modifications.
			if (speakingPlayerOffset == NULL)
				throw nullptr;

			firstStringUpdate = false;
		}

		std::string newHudMessagesStrings;
		newHudMessagesStrings.reserve(30);

		for (size_t i = 0; i < speakingPlayer.length(); ++i)
		{
			const unsigned char c = speakingPlayer[i];
			newHudMessagesStrings.push_back(hex[c >> 4]);
			newHudMessagesStrings.push_back(hex[c & 15]);
		}

		//if the speaker name is less than 15 characters, fill the rest of the string with 0.
		if (speakingPlayer.length() < (size_t)15)
			for (int i = speakingPlayer.length() * 2; i < 30; ++i)
				newHudMessagesStrings.push_back('0');

		//Now convert the hex string to a byte array and poke!
		hexStringStream >> std::hex;
		for (size_t strIndex = 0, dataIndex = 0; strIndex < (size_t)newHudMessagesStrings.length(); ++dataIndex)
		{
			// Read out and convert the string two characters at a time
			const char tmpStr[3] = { newHudMessagesStrings[strIndex++], newHudMessagesStrings[strIndex++], 0 };

			// Reset and fill the string stream
			hexStringStream.clear();
			hexStringStream.str(tmpStr);

			// Do the conversion
			int tmpValue = 0;
			hexStringStream >> tmpValue;
			unic->Data.Elements[dataIndex + speakingPlayerOffset] = static_cast<unsigned char>(tmpValue);
		}

		if (speakingPlayerIndex != NULL)
		{
			auto *chud = Blam::Tags::TagInstance(0x12BD).GetDefinition<Blam::Tags::UI::ChudDefinition>();
			//Make sure that the speaking player HUD widget has the correct string.
			chud->HudWidgets[speakingPlayerIndex].TextWidgets[0].TextStringID = speakingPlayerStringID;
		}

	}

	bool firstHudUpdate = true;
	unsigned short teamBroadcastIndicatorIndex = 0; //Hud Widget containing Icons.
	unsigned short broadcastAvailableIndex = 0; //Can Talk Icon
	unsigned short broadcastIndex = 0; //Talking Icon.
	unsigned short broadcastPTTIndex = 0; //Push To Talk Icon
	unsigned short broadcastNoIndex = 0; //Can't Talk Icon
	unsigned short spartanMotionTrackerIndex = 0; //motion_tracker hud widget.
	unsigned short spartanVoiceBroadcast1Index = 0; //Speaking radar icon 1.
	unsigned short spartanVoiceBroadcast2Index = 0; //Speaking radar icon 2.
	//Support for the elite HUD, just in case any mods need it.
	unsigned short eliteMotionTrackerIndex = 0;
	unsigned short eliteVoiceBroadcast1Index = 0; //Speaking radar icon 1.
	unsigned short eliteVoiceBroadcast2Index = 0; //Speaking radar icon 2.

	bool voiceChatIconValidTags = false;

	void UpdateVoiceChatHUD()
	{
		if (!tagsInitiallyLoaded)
			return;

		typedef void *(*GetTagAddressPtr)(int groupTag, uint32_t index);
		auto GetTagAddressImpl = reinterpret_cast<GetTagAddressPtr>(0x503370);

		using Blam::Tags::TagInstance;
		using Blam::Tags::UI::ChudDefinition;

		//If there's no data to toggle (due to modded tags), do nothing.
		if (!firstHudUpdate && !voiceChatIconValidTags)
			return;

		if (GetTagAddressImpl('chdt', 0x12BD) == nullptr)
			return;
		if (GetTagAddressImpl('chdt', 0x0C1E) == nullptr)
			return;

		auto *scoreboardChud = Blam::Tags::TagInstance(0x12BD).GetDefinition<Blam::Tags::UI::ChudDefinition>();
		auto *spartanChud = Blam::Tags::TagInstance(0x0C1E).GetDefinition<Blam::Tags::UI::ChudDefinition>();
		//auto *eliteChud = Blam::Tags::TagInstance(0x0E5F).GetDefinition<Blam::Tags::UI::ChudDefinition>();

		//If it's the first time updating the HUD, find the tagblock indexes.
		if (firstHudUpdate)
		{
			//Find widgets in spartan.
			for (int hudWidgetBlock = 0; hudWidgetBlock < spartanChud->HudWidgets.Count; hudWidgetBlock++)
			{
				if (spartanChud->HudWidgets[hudWidgetBlock].NameStringID == 0x2A76) //motion_tracker
				{
					spartanMotionTrackerIndex = hudWidgetBlock;

					for (int bitmapWidgetBlock = 0; bitmapWidgetBlock < spartanChud->HudWidgets[hudWidgetBlock].BitmapWidgets.Count; bitmapWidgetBlock++)
					{
						if (spartanChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x2A77) //voice_broadcast1
							spartanVoiceBroadcast1Index = bitmapWidgetBlock;
						else if (spartanChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x2A78) //voice_broadcast2
							spartanVoiceBroadcast2Index = bitmapWidgetBlock;

						if (spartanVoiceBroadcast1Index != NULL && spartanVoiceBroadcast2Index != NULL)
							break;
					}
				}
				if (spartanMotionTrackerIndex != NULL)
					break;
			}

			/*
			//Find widgets in elite.
			for (int hudWidgetBlock = 0; hudWidgetBlock < eliteChud->HudWidgets.Count; hudWidgetBlock++)
			{
				if (eliteChud->HudWidgets[hudWidgetBlock].NameStringID == 0x2A76) //motion_tracker
				{
					eliteMotionTrackerIndex = hudWidgetBlock;

					for (int bitmapWidgetBlock = 0; bitmapWidgetBlock < eliteChud->HudWidgets[hudWidgetBlock].BitmapWidgets.Count; bitmapWidgetBlock++)
					{
						if (eliteChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x2A77) //voice_broadcast1
							eliteVoiceBroadcast1Index = bitmapWidgetBlock;
						else if (eliteChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x2A78) //voice_broadcast2
							eliteVoiceBroadcast2Index = bitmapWidgetBlock;

						if (eliteVoiceBroadcast1Index != NULL && eliteVoiceBroadcast2Index != NULL)
							break;
					}
				}
				if (eliteMotionTrackerIndex != NULL)
					break;
			}
			*/

			//Find widgets in scoreboard.
			for (int hudWidgetBlock = 0; hudWidgetBlock < scoreboardChud->HudWidgets.Count; hudWidgetBlock++)
			{
				if (scoreboardChud->HudWidgets[hudWidgetBlock].NameStringID == 0x45C2) // team_broadcast_indicator
				{
					teamBroadcastIndicatorIndex = hudWidgetBlock;

					for (int bitmapWidgetBlock = 0; bitmapWidgetBlock < scoreboardChud->HudWidgets[hudWidgetBlock].BitmapWidgets.Count; bitmapWidgetBlock++)
					{
						if (scoreboardChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x45C3) // broadcast
							broadcastIndex = bitmapWidgetBlock;
						else if (scoreboardChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x45C4) // broadcast_available
							broadcastAvailableIndex = bitmapWidgetBlock;
						else if (scoreboardChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x45C5) // broadcast_ptt_sybmol
							broadcastPTTIndex = bitmapWidgetBlock;
						else if (scoreboardChud->HudWidgets[hudWidgetBlock].BitmapWidgets[bitmapWidgetBlock].NameStringID == 0x45C6) // broadcast_no
							broadcastNoIndex = bitmapWidgetBlock;

						//if everything is found, break early.
						if (((broadcastIndex != NULL) && (broadcastAvailableIndex != NULL)) && ((broadcastPTTIndex != NULL) && (broadcastNoIndex != NULL)))
							break;
					}
				}
				if (scoreboardChud->HudWidgets[hudWidgetBlock].NameStringID == 0x45BF) // speaker_name
				{
					speakingPlayerIndex = hudWidgetBlock;
				}
				if ((teamBroadcastIndicatorIndex != NULL) && (speakingPlayerIndex != NULL))
					break;
			}

			//Check the availability of statedata, positiondata.
			if (scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastIndex].StateData.Count > 0 &&
				scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastAvailableIndex].StateData.Count > 0 &&
				scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastNoIndex].StateData.Count > 0 &&
				scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastPTTIndex].StateData.Count > 0 &&
				scoreboardChud->HudWidgets[speakingPlayerIndex].StateData.Count > 0 &&
				spartanChud->HudWidgets[spartanMotionTrackerIndex].BitmapWidgets[spartanVoiceBroadcast1Index].StateData.Count > 0 &&
				spartanChud->HudWidgets[spartanMotionTrackerIndex].BitmapWidgets[spartanVoiceBroadcast2Index].StateData.Count > 0)/* &&
				eliteChud->HudWidgets[eliteMotionTrackerIndex].BitmapWidgets[eliteVoiceBroadcast1Index].StateData.Count > 0 &&
				eliteChud->HudWidgets[eliteMotionTrackerIndex].BitmapWidgets[eliteVoiceBroadcast2Index].StateData.Count > 0)*/
				voiceChatIconValidTags = true;

			firstHudUpdate = false;
		}

		//Hide all Icons.
		//Bit 7 is the broken "Tap to Talk" state used in halo 3.
		//Disabling the icon using a broken state triggers the close animation. Then removing this state triggers the open animation.
		scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastIndex].StateData[0].EngineFlags3 = ChudDefinition::StateDataEngineFlags3::Bit7;
		scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastAvailableIndex].StateData[0].EngineFlags3 = ChudDefinition::StateDataEngineFlags3::Bit7;
		scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastNoIndex].StateData[0].EngineFlags3 = ChudDefinition::StateDataEngineFlags3::Bit7;
		scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastPTTIndex].StateData[0].EngineFlags3 = ChudDefinition::StateDataEngineFlags3::Bit7;

		scoreboardChud->HudWidgets[speakingPlayerIndex].StateData[0].ScoreboardFlags1 = ChudDefinition::StateDataScoreboardFlags1::Bit3;

		spartanChud->HudWidgets[spartanMotionTrackerIndex].BitmapWidgets[spartanVoiceBroadcast1Index].StateData[0].UnknownFlags13 = ChudDefinition::StateDataUnknownFlags::Bit10;
		spartanChud->HudWidgets[spartanMotionTrackerIndex].BitmapWidgets[spartanVoiceBroadcast2Index].StateData[0].UnknownFlags13 = ChudDefinition::StateDataUnknownFlags::Bit10;

		//eliteChud->HudWidgets[eliteMotionTrackerIndex].BitmapWidgets[eliteVoiceBroadcast1Index].StateData[0].UnknownFlags13 = ChudDefinition::StateDataUnknownFlags::Bit10;
		//eliteChud->HudWidgets[eliteMotionTrackerIndex].BitmapWidgets[eliteVoiceBroadcast2Index].StateData[0].UnknownFlags13 = ChudDefinition::StateDataUnknownFlags::Bit10;

		//Show the correct Icon.
		//To remove hardcoded scale values, we could store these default scales earlier by reading them on first update. I'm gonna leave them for now.
		switch (micState)
		{
		case VoiceChatIcon::Speaking:
			scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastIndex].StateData[0].EngineFlags3 = (ChudDefinition::StateDataEngineFlags3)0;
			spartanChud->HudWidgets[spartanMotionTrackerIndex].BitmapWidgets[spartanVoiceBroadcast1Index].StateData[0].UnknownFlags13 = (ChudDefinition::StateDataUnknownFlags)0;
			spartanChud->HudWidgets[spartanMotionTrackerIndex].BitmapWidgets[spartanVoiceBroadcast2Index].StateData[0].UnknownFlags13 = (ChudDefinition::StateDataUnknownFlags)0;
			//eliteChud->HudWidgets[eliteMotionTrackerIndex].BitmapWidgets[eliteVoiceBroadcast1Index].StateData[0].UnknownFlags13 = (ChudDefinition::StateDataUnknownFlags)0;
			//eliteChud->HudWidgets[eliteMotionTrackerIndex].BitmapWidgets[eliteVoiceBroadcast2Index].StateData[0].UnknownFlags13 = (ChudDefinition::StateDataUnknownFlags)0;
			break;
		case VoiceChatIcon::Available:
			scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastAvailableIndex].StateData[0].EngineFlags3 = (ChudDefinition::StateDataEngineFlags3)0;
			break;
		case VoiceChatIcon::Unavailable:
			scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastNoIndex].StateData[0].EngineFlags3 = (ChudDefinition::StateDataEngineFlags3)0;
			break;
		case VoiceChatIcon::PushToTalk:
			scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastPTTIndex].StateData[0].EngineFlags3 = (ChudDefinition::StateDataEngineFlags3)0;
			scoreboardChud->HudWidgets[teamBroadcastIndicatorIndex].BitmapWidgets[broadcastAvailableIndex].StateData[0].EngineFlags3 = (ChudDefinition::StateDataEngineFlags3)0;
			break;
		case VoiceChatIcon::None: //Not sure if this is even needed.
			break;
		}

		if (someoneSpeaking)
			scoreboardChud->HudWidgets[speakingPlayerIndex].StateData[0].ScoreboardFlags1 = (ChudDefinition::StateDataScoreboardFlags1)0;
	}

	void ApplyAll()
	{
		// Rewire $hq.MatchmakingLeaveQueue() to end the game
		Hook(0x3B6826, UI_EndGame, HookFlags::IsCall).Apply();
		Patch::NopFill(Pointer::Base(0x3B6826 + 5), 1);

		// Rewire $hf2pEngine.PerformLogin() to show the pause menu
		Hook(0x234756, &UI_ShowHalo3PauseMenu, HookFlags::IsCall).Apply();
		Patch::NopFill(Pointer::Base(0x234756 + 5), 1);

		// Allows you to press B to close the H3 pause menu
		// TODO: find out what the byte that's being checked does, we're just patching out the check here but maybe it's important
		Patch::NopFill(Pointer::Base(0x6E05F3), 2);

		// Fix "Network" setting in lobbies (change broken 0x100B7 menuID to 0x100B6)
		Patch(0x6C34B0, { 0xB6 }).Apply();

		// Fix gamepad option in settings (todo: find out why it doesn't detect gamepads
		// and find a way to at least enable pressing ESC when gamepad is enabled)
		Patch::NopFill(Pointer::Base(0x20D7F2), 2);

		// Fix menu update code to include missing mainmenu code
		Hook(0x6DFB73, &UI_MenuUpdateHook, HookFlags::IsCall).Apply();

		// Sorta hacky way of getting game options screen to show when you press X on lobby
		// TODO: find real way of showing the [X] Edit Game Options text, that might enable it to work without patching
		Hook(0x721B8A, LobbyMenuButtonHandlerHook, HookFlags::IsJmpIfEqual).Apply();

		// Hook UI vftable's forge menu button handler, so arrow keys can act as bumpers
		// added side effect: analog stick left/right can also navigate through menus
		DWORD temp;
		DWORD temp2;
		auto writeAddr = Pointer(0x169EFD8);
		if (!VirtualProtect(writeAddr, 4, PAGE_READWRITE, &temp))
		{
			std::stringstream ss;
			ss << "Failed to set protection on memory address " << std::hex << (void*)writeAddr;
			OutputDebugString(ss.str().c_str());
		}
		else
		{
			writeAddr.Write<uint32_t>((uint32_t)&UI_Forge_ButtonPressHandlerHook);
			VirtualProtect(writeAddr, 4, temp, &temp2);
		}

		// Remove Xbox Live from the network menu
		Patch::NopFill(Pointer::Base(0x723D85), 0x17);
		Pointer::Base(0x723DA1).Write<uint8_t>(0);
		Pointer::Base(0x723DB8).Write<uint8_t>(1);
		Patch::NopFill(Pointer::Base(0x723DFF), 0x3);
		Pointer::Base(0x723E07).Write<uint8_t>(0);
		Pointer::Base(0x723E1C).Write<uint8_t>(0);

		// Localized string override hook
		Hook(0x11E040, LocalizedStringHook).Apply();

		// Remove "BUILT IN" text when choosing map/game variants by feeding the UI_SetVisiblityOfElement func a nonexistant string ID for the element (0x202E8 instead of 0x102E8)
		// TODO: find way to fix this text instead of removing it, since the 0x102E8 element (subitem_edit) is used for other things like editing/viewing map variant metadata
		Patch(0x705D6F, { 0x2 }).Apply();

		// Hook window title sprintf to replace the dest buf with our string
		Hook(0x2EB84, WindowTitleSprintfHook, HookFlags::IsCall).Apply();

		// Runs when the game's resolution is changed
		Hook(0x621303, ResolutionChangeHook, HookFlags::IsCall).Apply();

		if (Modules::ModuleGraphics::Instance().VarUIScaling->ValueInt == 1) {
			// Enable H3UI scaling
			Patch::NopFill(Pointer::Base(0x61FAD1), 2);
		}

		// Change the way that Forge handles dpad up so that it doesn't mess with key repeat
		// Comparing the action tick count to 1 instead of using the "handled" flag does roughly the same thing and lets the menu UI read the key too
		Patch(0x19F17F, { 0x75 }).Apply();
		Patch::NopFill(Pointer::Base(0x19F198), 4);

		// Reimplement the function that assigns lobby roster colors
		Hook(0x726100, UI_UpdateRosterColorsHook).Apply();

		// Game window creation callbacks
		Hook(0x622057, CreateGameWindowHook, HookFlags::IsCall).Apply();

		// disable saber's weapon hud
		Patch(0x6866F2, { 0xEB }).Apply();
		// restore h3 weapon hud
		Hook(0x006970BB, UI_UpdateH3HUDHook, HookFlags::IsCall).Apply();
		// fix equipment swap message
		Pointer(0xABD324).Write<uint32_t>(0xABD2F7);
		// fix dual-wield message
		Pointer(0xABD388).Write<uint32_t>(0xABD300);
		// add support for mouse buttons in HUD messages
		Hook(0x6BC83F, GetActionButtonNameHook, HookFlags::IsJmpIfEqual).Apply();
		// enable controller buttons in HUD messages
		Patch(0x6BC5D7, { 0x98,0xDE, 0x44, 0x02, 01 }).Apply();

		Pointer(0x0169FCE0).Write(uint32_t(&c_main_menu_screen_widget_item_select_hook));
		Hook(0x2047BF, c_ui_view_draw_hook, HookFlags::IsCall).Apply();

		Hook(0x721D38, UI_SetPlayerDesiredTeamHook, HookFlags::IsCall).Apply();
		Patches::Input::RegisterDefaultInputHandler(OnUiInputUpdated);
	}

	void ApplyMapNameFixes()
	{
		uint32_t levelsGlobalPtr = Pointer::Base(0x149E2E0).Read<uint32_t>();
		if (!levelsGlobalPtr)
			return;

		// TODO: map out these global arrays, content items seems to use same format

		uint32_t numLevels = Pointer(levelsGlobalPtr + 0x34).Read<uint32_t>();

		const wchar_t* search[12] = { L"guardian", L"riverworld", L"s3d_avalanche", L"s3d_edge", L"s3d_reactor", L"s3d_turf", L"cyberdyne", L"chill", L"deadlock", L"bunkerworld", L"shrine", L"zanzibar" };
		const wchar_t* names[12] = { L"Guardian", L"Valhalla", L"Diamondback", L"Edge", L"Reactor", L"Icebox", L"The Pit", L"Narrows", L"High Ground", L"Standoff", L"Sandtrap", L"Last Resort" };
		// TODO: Get names/descs using string ids? Seems the unic tags have descs for most of the maps
		const wchar_t* descs[12] = {
			L"Millennia of tending has produced trees as ancient as the Forerunner structures they have grown around. 2-6 players.",
			L"The crew of V-398 barely survived their unplanned landing in this gorge...this curious gorge. 6-16 players.",
			L"Hot winds blow over what should be a dead moon. A reminder of the power Forerunners once wielded. 6-16 players.",
			L"The remote frontier world of Partition has provided this ancient databank with the safety of seclusion. 6-16 players.",
			L"Being constructed just prior to the Invasion, its builders had to evacuate before it was completed. 6-16 players.",
			L"Downtown Tyumen's Precinct 13 offers an ideal context for urban combat training. 4-10 players.",
			L"Software simulations are held in contempt by the veteran instructors who run these training facilities. 4-10 players.",
			L"Without cooling systems such as these, excess heat from the Ark's forges would render the construct uninhabitable. 2-8 players.",
			L"A relic of older conflicts, this base was reactivated after the New Mombasa Slipspace Event. 4-12 players.",
			L"Once, nearby telescopes listened for a message from the stars. Now, these silos contain our prepared response. 4-12 players.",
			L"Although the Brute occupiers have been driven from this ancient structure, they left plenty to remember them by. 6-16 players",
			L"Remote industrial sites like this one are routinely requisitioned and used as part of Spartan training exercises. 4-12 players."

		};
		for (uint32_t i = 0; i < numLevels; i++)
		{
			Pointer levelNamePtr = Pointer(levelsGlobalPtr + 0x54 + (0x360 * i) + 0x8);
			Pointer levelDescPtr = Pointer(levelsGlobalPtr + 0x54 + (0x360 * i) + 0x8 + 0x40);

			wchar_t levelName[0x21] = { 0 };
			levelNamePtr.Read(levelName, sizeof(wchar_t) * 0x20);

			for (uint32_t y = 0; y < sizeof(search) / sizeof(*search); y++)
			{
				if (wcscmp(search[y], levelName) == 0)
				{
					memset(levelNamePtr, 0, sizeof(wchar_t) * 0x20);
					wcscpy_s(levelNamePtr, 0x20, names[y]);

					memset(levelDescPtr, 0, sizeof(wchar_t) * 0x80);
					wcscpy_s(levelDescPtr, 0x80, descs[y]);
					break;
				}
			}
		}
	}

	int HUDResolutionWidth = 0;
	int HUDResolutionHeight = 0;
	float HUDResolutionScaleX = 0;
	float HUDResolutionScaleY = 0;
	float HUDMotionSensorOffsetX = 0;
	float HUDBottomVisorOffsetY = 0;
	bool firstResolutionChange = true;

	void ApplyUIResolution()
	{
		if (Modules::ModuleGraphics::Instance().VarUIScaling->ValueInt == 1) {
			using Blam::Tags::TagInstance;
			using Blam::Tags::UI::ChudGlobalsDefinition;
			using Blam::Tags::UI::ChudDefinition;

			auto *gameResolution = reinterpret_cast<int *>(0x19106C0);
			auto *globals = TagInstance(0x01BD).GetDefinition<ChudGlobalsDefinition>();
			auto *chud = Blam::Tags::TagInstance(0x0C1E).GetDefinition<Blam::Tags::UI::ChudDefinition>();
			if (!globals || !chud || globals->HudGlobals.Count == 0 || globals->HudGlobals[0].HudAttributes.Count == 0)
				return;

			// Store initial HUD resolution values the first time the resolution is changed.
			if (firstResolutionChange)
			{
				HUDResolutionWidth = globals->HudGlobals[0].HudAttributes[0].ResolutionWidth;
				HUDResolutionHeight = globals->HudGlobals[0].HudAttributes[0].ResolutionHeight;
				HUDResolutionScaleX = globals->HudGlobals[0].HudAttributes[0].HorizontalScale;
				HUDResolutionScaleY = globals->HudGlobals[0].HudAttributes[0].VerticalScale;
				HUDMotionSensorOffsetX = globals->HudGlobals[0].HudAttributes[0].MotionSensorOffsetX;
				// Store bottom visor offset
				for (auto &widget : chud->HudWidgets)
				{
					if (widget.NameStringID == 0x2ABD) // in_helmet_bottom_new
					{
						HUDBottomVisorOffsetY = widget.PlacementData[0].OffsetY;
					}
				}
				firstResolutionChange = false;
			}

			// Make UI match it's original width of 1920 pixels on non-widescreen monitors.
			// Fixes the visor getting cut off.
			globals->HudGlobals[0].HudAttributes[0].ResolutionWidth = HUDResolutionWidth;

			// H3UI Resolution
			int* UIResolution = reinterpret_cast<int*>(0x19106C8);

			if (((float)gameResolution[0] / (float)gameResolution[1] >  16.0f / 9.0f)) {
				// On aspect ratios with a greater width than 16:9 center the UI on the screen
				globals->HudGlobals[0].HudAttributes[0].ResolutionHeight = HUDResolutionHeight;
				globals->HudGlobals[0].HudAttributes[0].HorizontalScale = (globals->HudGlobals[0].HudAttributes[0].ResolutionWidth / (float)gameResolution[0]) * HUDResolutionScaleX;
				globals->HudGlobals[0].HudAttributes[0].VerticalScale = (globals->HudGlobals[0].HudAttributes[0].ResolutionHeight / (float)gameResolution[1]) * HUDResolutionScaleY;

				UIResolution[0] = (int)(((float)gameResolution[0] / (float)gameResolution[1]) * 640);;
				UIResolution[1] = 640;
			}
			else
			{
				globals->HudGlobals[0].HudAttributes[0].ResolutionHeight = (int)(((float)gameResolution[1] / (float)gameResolution[0]) * globals->HudGlobals[0].HudAttributes[0].ResolutionWidth);
				globals->HudGlobals[0].HudAttributes[0].HorizontalScale = HUDResolutionScaleX;
				globals->HudGlobals[0].HudAttributes[0].VerticalScale = HUDResolutionScaleY;

				UIResolution[0] = 1152;//1152 x 640 resolution
				UIResolution[1] = (int)(((float)gameResolution[1] / (float)gameResolution[0]) * 1152);
			}

			// Adjust motion sensor blip to match the UI resolution
			globals->HudGlobals[0].HudAttributes[0].MotionSensorOffsetX = HUDMotionSensorOffsetX;
			globals->HudGlobals[0].HudAttributes[0].MotionSensorOffsetY = (float)(globals->HudGlobals[0].HudAttributes[0].ResolutionHeight - (globals->HudGlobals[0].HudAttributes[0].MotionSensorRadius - globals->HudGlobals[0].HudAttributes[0].MotionSensorScale));

			// Search for the visor bottom and fix it if found
			for (auto &widget : chud->HudWidgets)
			{
				if (widget.NameStringID == 0x2ABD) // in_helmet_bottom_new
				{
					widget.PlacementData[0].OffsetY = (((float)globals->HudGlobals[0].HudAttributes[0].ResolutionHeight - HUDResolutionHeight) / 2) + HUDBottomVisorOffsetY;
					break;
				}
			}
		}
	}
}

namespace
{
	void __fastcall UI_MenuUpdateHook(void* a1, int unused, int menuIdToLoad)
	{
		auto& dorito = ElDorito::Instance();
		if (menuIdToLoad == 0x10083)
			dorito.OnMainMenuShown();

		bool shouldUpdate = *(DWORD*)((uint8_t*)a1 + 0x10) >= 0x1E;
		int uiData0x18Value = 1;
		//if (menuIdToLoad == 0x100A8) // TODO1: find what 0x100A8(H3E) stringid is in HO
		//	uiData0x18Value = 5;

		typedef void(__thiscall *UI_MenuUpdatePtr)(void* a1, int menuIdToLoad);
		auto UI_MenuUpdate = reinterpret_cast<UI_MenuUpdatePtr>(0xADF6E0);
		UI_MenuUpdate(a1, menuIdToLoad);

		if (shouldUpdate)
		{
			typedef void*(__cdecl * UI_AllocPtr)(int size);
			auto UI_Alloc = reinterpret_cast<UI_AllocPtr>(0xAB4ED0);
			void* UIData = UI_Alloc(0x3C);

			// fill UIData with proper data
			typedef void*(__thiscall * UI_OpenDialogByIdPtr)(void* a1, unsigned int dialogStringId, int a3, int dialogFlags, unsigned int parentDialogStringId);
			auto UI_OpenDialogById = reinterpret_cast<UI_OpenDialogByIdPtr>(0xA92780);
			UI_OpenDialogById(UIData, menuIdToLoad, 0xFF, 4, 0x1000D);

			// post UI message
			typedef int(*UI_PostMessagePtr)(void* uiDataStruct);
			auto UI_PostMessage = reinterpret_cast<UI_PostMessagePtr>(0xA93450);
			UI_PostMessage(UIData);

			*(uint32_t*)((char*)UIData + 0x18) = uiData0x18Value;
		}
	}

	int UI_ShowHalo3PauseMenu(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		return Patches::Ui::ShowDialog(0x10084, 0, 4, 0x1000C) != nullptr;
	}

	void UI_EndGame()
	{
		auto session = Blam::Network::GetActiveSession();
		if (!session || !session->IsEstablished())
			return;
		if (session->IsHost())
			Blam::Network::EndGame();
		else
			Blam::Network::LeaveGame();
	}

	char __fastcall UI_Forge_ButtonPressHandlerHook(void* a1, int unused, uint8_t* controllerStruct)
	{
		uint32_t btnCode = *(uint32_t*)(controllerStruct + 0x1C);

		if (btnCode == Blam::Input::eUiButtonCodeLeft || btnCode == Blam::Input::eUiButtonCodeRight ||
			btnCode == Blam::Input::eUiButtonCodeDpadLeft || btnCode == Blam::Input::eUiButtonCodeDpadRight)
		{
			if (btnCode == Blam::Input::eUiButtonCodeLeft || btnCode == Blam::Input::eUiButtonCodeDpadLeft) // analog left / arrow key left
				*(uint32_t*)(controllerStruct + 0x1C) = Blam::Input::eUiButtonCodeLB;

			if (btnCode == Blam::Input::eUiButtonCodeRight || btnCode == Blam::Input::eUiButtonCodeDpadRight) // analog right / arrow key right
				*(uint32_t*)(controllerStruct + 0x1C) = Blam::Input::eUiButtonCodeRB;
		}

		typedef char(__thiscall *UI_Forge_ButtonPressHandler)(void* a1, void* controllerStruct);
		UI_Forge_ButtonPressHandler buttonHandler = (UI_Forge_ButtonPressHandler)0xAE2180;
		return buttonHandler(a1, controllerStruct);
	}

	bool LocalizedStringHookImpl(int tagIndex, int stringId, wchar_t *outputBuffer)
	{
		const size_t MaxStringLength = 0x400;

		switch (stringId)
		{
		case 0x1010A: // start_new_campaign
		{
			// Get the version string, convert it to uppercase UTF-16, and return it
			std::string version = Utils::Version::GetVersionString();
			std::transform(version.begin(), version.end(), version.begin(), toupper);
			std::wstring unicodeVersion(version.begin(), version.end());
			swprintf(outputBuffer, MaxStringLength, L"ELDEWRITO %s", unicodeVersion.c_str());
			return true;
		}
		}
		return false;
	}

	void WindowTitleSprintfHook(char* destBuf, char* format, char* version)
	{
		std::string windowTitle = "ElDewrito | Version: " + Utils::Version::GetVersionString() + " | Build Date: " __DATE__;
		strcpy_s(destBuf, 0x40, windowTitle.c_str());
	}

	__declspec(naked) void LocalizedStringHook()
	{
		__asm
		{
			// Run the hook implementation function and fallback to the original if it returned false
			push ebp
			mov ebp, esp
			push[ebp + 0x10]
			push[ebp + 0xC]
			push[ebp + 0x8]
			call LocalizedStringHookImpl
			add esp, 0xC
			test al, al
			jz fallback

			// Don't run the original function
			mov esp, ebp
			pop ebp
			ret

		fallback:
			// Execute replaced code and jump back to original function
			sub esp, 0x800
			mov edx, 0x51E049
			jmp edx
		}
	}

	__declspec(naked) void LobbyMenuButtonHandlerHook()
	{
		__asm
		{
			// call sub that handles showing game options
			mov ecx, esi
			push [edi+0x10]
			mov eax, 0xB225B0
			call eax
			// jump back to original function
			mov eax, 0xB21B9F
			jmp eax
		}
	}

	void ShowLanBrowser()
	{
		typedef void*(__cdecl * UI_AllocPtr)(int size);
		auto UI_Alloc = reinterpret_cast<UI_AllocPtr>(0xAB4ED0);
		typedef void*(__fastcall* c_load_game_browser_screen_messagePtr)(void *thisPtr, int unused, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);
		auto c_load_game_browser_screen_message = reinterpret_cast<c_load_game_browser_screen_messagePtr>(0xADE0B0);
		typedef void(*UI_Dialog_QueueLoadPtr)(void *dialog);
		auto UI_Dialog_QueueLoad = reinterpret_cast<UI_Dialog_QueueLoadPtr>(0xA93450);

		auto dialog = UI_Alloc(0x44);
		if (!dialog)
			return;
		dialog = c_load_game_browser_screen_message(dialog, 0, 0x10355, 0, 4, 0x1000C, 0, 0);
		if (dialog)
			UI_Dialog_QueueLoad(dialog);
	}

	void __fastcall c_main_menu_screen_widget_item_select_hook(void* thisptr, void* unused, int a2, int a3, void* a4, void* a5)
	{
		static auto  c_main_menu_screen_widget_item_select = (void(__thiscall*)(void* thisptr, int a2, int a3, void* dataSource, void* a5))(0xAE77D0);

		if (a3 == 0x10083) // main_menu
		{
			uint32_t id;
			auto itemIndex = (*(int(__thiscall **)(void*))(*(uint8_t**)a4 + 0x18))(a4);
			if ((*(bool(__thiscall **)(void*, uint32_t, uint32_t, uint32_t*))(*(uint8_t**)a5 + 0x34))(a5, itemIndex, 0x2AA, &id))
			{
				// TODO: Maybe check the menu item's text over the index?

				switch (itemIndex)
				{
				case 0: // Server Browser
					Web::Ui::ScreenLayer::Show("browser", "{}");
					return;

				case 1: // Host Multiplayer
					break;

				case 2: // Host Forge
					break;

				case 3: // Local Games
					ShowLanBrowser();
					return;

				case 4: // Settings
					Web::Ui::ScreenLayer::Show("settings", "{}");
					return;

				case 5: // Exit
					Patches::Core::ExecuteShutdownCallbacks();
					std::exit(0);
					return;
				}
			}
		}

		c_main_menu_screen_widget_item_select(thisptr, a2, a3, a4, a5);
	}

	void ResolutionChangeHook()
	{
		typedef void(__thiscall *ApplyResolutionChangeFunc)();
		ApplyResolutionChangeFunc ApplyResolutionChange = reinterpret_cast<ApplyResolutionChangeFunc>(0xA226D0);
		ApplyResolutionChange();

		// Update the ingame UI's resolution
		Patches::Ui::ApplyUIResolution();
		Web::Ui::ScreenLayer::Resize();
	}

	int UI_ListItem_GetIndex(void *item)
	{
		typedef int(__fastcall* UI_ListItem_GetIndexPtr)(void *thisPtr, int unused);
		auto vtable = *reinterpret_cast<void***>(item);
		auto GetIndex = reinterpret_cast<UI_ListItem_GetIndexPtr>(vtable[6]);
		return GetIndex(item, 0);
	}

	bool UI_OrderedDataSource_Get(void *data, int index, int key, void *result)
	{
		typedef bool(__fastcall* UI_OrderedDataSource_GetPtr)(void *thisPtr, int unused, int index, int key, void *result);
		auto vtable = *reinterpret_cast<void***>(data);
		auto Get = reinterpret_cast<UI_OrderedDataSource_GetPtr>(vtable[11]);
		return Get(data, 0, index, key, result);
	}

	int __fastcall UI_SetPlayerDesiredTeamHook(void *thisPtr, int unused, int a2, int teamIndex, int a4)
	{
		localDesiredTeam = teamIndex;

		typedef int(__thiscall *UI_SetPlayerDesiredTeamFunc)(void* thisPtr, int a2, int a3, int a4);
		UI_SetPlayerDesiredTeamFunc UI_SetPlayerDesiredTeam = reinterpret_cast<UI_SetPlayerDesiredTeamFunc>(0xB25C30);
		return UI_SetPlayerDesiredTeam(thisPtr, a2, teamIndex, a4);
	}

	void OnUiInputUpdated()
	{
		auto isUsingController = *(bool*)0x0244DE98;

		if (isUsingController && Blam::Input::GetActionState(Blam::Input::eGameActionUiB)->Ticks == 1) //reset desired team after cancel
			localDesiredTeam = -1;
		else if (Blam::Input::GetKeyTicks(Blam::Input::KeyCode::eKeyCodeB, Blam::Input::InputType::eInputTypeGame) == 1) //kbm
			localDesiredTeam = -1;
	}

	void __fastcall UI_UpdateRosterColorsHook(void *thisPtr, int unused, void *a0)
	{
		typedef void*(__fastcall* UI_GetOrderedDataSourcePtr)(void *thisPtr, int unused);
		auto UI_GetOrderedDataSource = reinterpret_cast<UI_GetOrderedDataSourcePtr>(0xB14FE0);
		typedef void(__fastcall* UI_BaseUpdateListColorsPtr)(void *thisPtr, int unused, void *a0);
		auto UI_BaseUpdateListColors = reinterpret_cast<UI_BaseUpdateListColorsPtr>(0xB16650);
		typedef void*(__fastcall* UI_Widget_FindFirstChildPtr)(void *thisPtr, int unused, int type);
		auto UI_Widget_FindFirstChild = reinterpret_cast<UI_Widget_FindFirstChildPtr>(0xAB8F80);
		typedef void*(__fastcall* UI_ListItem_NextPtr)(void *thisPtr, int unused, bool a0);
		auto UI_ListItem_Next = reinterpret_cast<UI_ListItem_NextPtr>(0xAB9230);
		typedef void*(__fastcall* UI_Widget_FindChildTextPtr)(void *thisPtr, int unused, int name);
		auto UI_Widget_FindChildText = reinterpret_cast<UI_Widget_FindChildTextPtr>(0xAB8AE0);
		typedef void*(__fastcall* UI_Widget_FindChildBitmapPtr)(void *thisPtr, int unused, int name);
		auto UI_Widget_FindChildBitmap = reinterpret_cast<UI_Widget_FindChildBitmapPtr>(0xAB8A40);
		typedef void(*UI_ColorWidgetWithPlayerColorPtr)(void *widget, int colorIndex, bool isTeam);
		auto UI_ColorWidgetWithPlayerColor = reinterpret_cast<UI_ColorWidgetWithPlayerColorPtr>(0xAA4C80);
		typedef void(__fastcall* UI_Widget_MultiplyColorPtr)(void *thisPtr, int unused, float *argbColor);
		auto UI_Widget_MultiplyColor = reinterpret_cast<UI_Widget_MultiplyColorPtr>(0xAB9D70);
		typedef void(*RgbToFloatColorPtr)(uint32_t rgbColor, float *result);
		auto RgbToFloatColor = reinterpret_cast<RgbToFloatColorPtr>(0x521300);

		const auto str_name = 0x2AA;
		const auto str_base_color = 0x10144;
		const auto str_base_color_hilite = 0x103DD;
		const auto str_team = 0x11D;
		const auto str_player_row_type = 0x103CD;

		// Call the base function in c_gui_list_widget
		UI_BaseUpdateListColors(thisPtr, 0, a0);

		// Get the data source to fetch player information from
		auto data = UI_GetOrderedDataSource(thisPtr, 0);
		if (!data)
			return;

		// Get whether teams are enabled by querying the session parameter
		// TODO: Is there a better way of doing this? The code that H3E uses doesn't seem to work anymore...
		static auto teamsEnabled = false; // This is static so that last value of this can be reused if the session closes
		auto session = Blam::Network::GetActiveSession();
		if (session && session->IsEstablished())
			teamsEnabled = session->HasTeams();

		int playerWidgetIndex = *reinterpret_cast<uint32_t*>(reinterpret_cast<int>(data) + session->MembershipInfo.GetPeerPlayer(session->MembershipInfo.LocalPeerIndex) * 0x1678 + 0x11C);

		// Loop through each list item widget
		auto item = UI_Widget_FindFirstChild(thisPtr, 0, 5); // 5 = List item
		while (item)
		{
			// Get child widget pointers
			auto nameWidget = UI_Widget_FindChildText(item, 0, str_name);
			auto baseColorWidget = UI_Widget_FindChildBitmap(item, 0, str_base_color);
			auto baseColorHiliteWidget = UI_Widget_FindChildBitmap(item, 0, str_base_color_hilite);

			// Get the list item index needed for data binding
			auto itemIndex = UI_ListItem_GetIndex(item);

			// Only set the color if player_row_type == 0
			auto playerRowType = 0;
			if (UI_OrderedDataSource_Get(data, itemIndex, str_player_row_type, &playerRowType) && playerRowType == 0)
			{
				// If teams are enabled and the player is assigned to a team, then use the team color,
				// otherwise use the primary color
				auto teamIndex = 0;
				if (teamsEnabled && UI_OrderedDataSource_Get(data, itemIndex, str_team, &teamIndex) && teamIndex >= 0)
				{
					if (itemIndex == playerWidgetIndex)
					{
						if (localDesiredTeam > -1 && localDesiredTeam < 8)
							teamIndex = localDesiredTeam;
					}
					
					if (nameWidget)
						UI_ColorWidgetWithPlayerColor(nameWidget, teamIndex, true);
					if (baseColorWidget)
						UI_ColorWidgetWithPlayerColor(baseColorWidget, teamIndex, true);
					if (baseColorHiliteWidget)
						UI_ColorWidgetWithPlayerColor(baseColorHiliteWidget, teamIndex, true);
				}
				else
				{
					// Pull the primary color directly out of the data source (it's easier)
					auto primaryColor = *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(data) + itemIndex * 0x1678 + 0x7F8);
					float primaryColorF[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
					RgbToFloatColor(primaryColor, &primaryColorF[1]);
					if (baseColorWidget)
						UI_Widget_MultiplyColor(baseColorWidget, 0, primaryColorF);
					if (baseColorHiliteWidget)
						UI_Widget_MultiplyColor(baseColorHiliteWidget, 0, primaryColorF);
				}
			}

			// Move to the next item
			item = UI_ListItem_Next(item, 0, true);
		}
	}

	HWND CreateGameWindowHook()
	{
		typedef HWND(*CreateGameWindowPtr)();
		auto CreateGameWindow = reinterpret_cast<CreateGameWindowPtr>(0xA223F0);
		auto hwnd = CreateGameWindow();
		if (!hwnd)
			return nullptr;

		for (auto &&callback : createWindowCallbacks)
			callback(hwnd);

		return hwnd;
	}

	bool UpdateH3HUDImpl(int playerMappingIndex)
	{
		struct ControlGlobalsAction
		{
			uint16_t Type;
			uint16_t Flags;
			uint32_t ObjectIndex;
			// ...
		};

		static auto GetPlayerControlsAction = (ControlGlobalsAction*(*)(int playerMappingIndex))(0x5D0BD0);
		static auto HUD_DisplayHUDMessage = (void(*)(unsigned int playerMappingIndex, int stringId, int a3, int alignment))(0x00A964E0);

		auto action = GetPlayerControlsAction(playerMappingIndex);

		if (action->Type != 9 && action->Type != 1)
			return false;
		auto actionWeapObject = Blam::Objects::Get(action->ObjectIndex);
		if (!actionWeapObject)
			return false;

		auto actionWeapDef = Blam::Tags::TagInstance(actionWeapObject->TagIndex).GetDefinition<Blam::Tags::Items::Weapon>();

		if (action->Flags & 1)
			HUD_DisplayHUDMessage(playerMappingIndex, actionWeapDef->PickupMessage, -1, 0);
		else if (action->Flags & 2)
			HUD_DisplayHUDMessage(playerMappingIndex, actionWeapDef->SwapMessage, -1, 0);
		else
			HUD_DisplayHUDMessage(playerMappingIndex, -1, -1, 0);

		if (action->Flags & 4)
			HUD_DisplayHUDMessage(playerMappingIndex, actionWeapDef->PickupOrDualWieldMessage, -1, 1);
		else if (action->Flags & 8)
			HUD_DisplayHUDMessage(playerMappingIndex, actionWeapDef->SwapOrDualWieldMessage, -1, 1);
		else
			HUD_DisplayHUDMessage(playerMappingIndex, -1, -1, 1);

		return true;
	}

	void __cdecl UI_UpdateH3HUDHook(int playerMappingIndex)
	{
		static auto UI_UpdateHUD = (void(__cdecl*)(int playerMappingIndex))(0xA95D10);

		if (!UpdateH3HUDImpl(playerMappingIndex))
			UI_UpdateHUD(playerMappingIndex);
	}

	bool __cdecl GetBoundMouseButtonName(int code, wchar_t* buff)
	{
		using namespace Blam::Input;

		static auto GameActionFromUnicodeCode = (GameAction(__cdecl *)(int code))(0xABD240);

		auto bindings = Modules::ModuleInput::Instance().GetBindings();

		auto action = GameActionFromUnicodeCode(code);
		auto mouseButton = bindings->PrimaryMouseButtons[action];
		switch (mouseButton)
		{
		case eMouseButtonWheelDown:
			swprintf(buff, L"<WHEEL DOWN>");
			return true;
		case eMouseButtonWheelUp:
			swprintf(buff, L"<WHEEL UP>");
			return true;
		case eMouseButtonLeft:
			swprintf(buff, L"<LMB>");
			return true;
		case eMouseButtonRight:
			swprintf(buff, L"<RMB>");
			return true;
		case eMouseButtonMiddle:
			swprintf(buff, L"<MMB>");
			return true;
		}

		return false;
	}

	__declspec(naked) void GetActionButtonNameHook()
	{
		__asm
		{
			push[ebp + 0x14]
			push[ebp + 0x10]
			call GetBoundMouseButtonName
			add esp, 8
			test al, al
			jz DEFAULT
			pop ebx
			mov esp, ebp
			pop ebp
			retn
			DEFAULT:
			mov eax, 0xABCA79
			jmp eax
		}
	}

	int GetHUDGlobalsIndexForRepresentation(void* playerRepresentation)
	{
		if (!playerRepresentation)
			return 0;

		auto nameId = *(uint32_t*)playerRepresentation;
		switch (nameId)
		{
		case 0x111B: // monitor
			return 2;
		case 0x1119: //mp_elite
		case 0xCC: // dervish
			return 1;
		default:
			return 0;
		}
	}

	__declspec(naked) void UI_GetHUDGlobalsIndexHook()
	{
		__asm
		{
			push ecx
			call GetHUDGlobalsIndexForRepresentation
			add esp, 4
			pop esi
			pop ebp
			retn
		}
	}

	void __fastcall c_ui_view_draw_hook(void* thisptr, void* unused)
	{
		if (!Modules::ModuleGame::Instance().VarHideH3UI->ValueInt)
			((void(__thiscall*)(void* thisptr))(0xA290A0))(thisptr);
	}
}
