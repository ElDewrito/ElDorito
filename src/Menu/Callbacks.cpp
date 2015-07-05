#include "Callbacks.hpp"
#include "..\ElModules.hpp"
#include <Awesomium/STLHelpers.h>
#include "Menu.hpp"

// TODO: Does mouse filter even do anything? Should I add a callback for it?
// TODO: add callbacks for keyboard bindings
// TODO: call setters using WebView::executejavascript
// TODO: add GameOptions struct
// TODO: add transparency for armor customization (webView->SetTransparent(true) + it seems you need alpha blending)
// Callback explanation/documentation: https://docs.google.com/spreadsheets/d/1ciHsJzeSkhECDJ9F6FTU5GDMgJoVsF33Z5S4KGXCytM/edit#gid=0

Settings* Callbacks::settings = (Settings*) 0x23019B8;

uint16_t* Callbacks::state = (uint16_t*)0x5260730; // TEMP hack; replace with proper hook later

// By selector, I mean the horizontal orange highlight that selects an option. You can move the selector up and down vertically.
uint16_t* Callbacks::startScreenSelecter; // 0=44912, 1=46576, 2=48240
uint16_t* Callbacks::multiplayerScreenSelector; // 0=4976, 1=6640, 2=8304, 3=9968, 4=11632
uint16_t* Callbacks::mapSelector; // 0=39504, 1=41840, 2=44176, 3=46512, 4=48848, 5=51184, 6=53520, 7=55856
HWND Callbacks::hWnd = *((HWND*)0x199C014); // TEMP hack

// TODO: try force map loading with online

void Callbacks::HUDShakeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->HUD_SHAKE = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::playerMarkerColorsCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->PLAYER_MARKER_COLORS = args.At(0).ToInteger();
}

void Callbacks::fovCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->FOV_DOUBLED = args.At(0).ToInteger() * 2;
}

void Callbacks::hostStartTimerCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	std::string cmd("server.countdown ");
	cmd.append(std::to_string(args.At(0).ToInteger()));
	Modules::CommandMap::Instance().ExecuteCommand(cmd);
}

void Callbacks::playerNameCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	std::string cmd("player.name ");
	cmd.append(Awesomium::ToString(args.At(0).ToString()));
	Modules::CommandMap::Instance().ExecuteCommand(cmd);
}

void Callbacks::showFPSCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::enableDXExtensionsCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::keyboardInputCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->CONTROLS_METHOD = args.At(0).ToBoolean() ? 0 : 1;
}

void Callbacks::toggleCrouchCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->TOGGLE_CROUCH = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::infantrySensCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->INFANTRY_HORIZONTAL_MOUSE_SENS = args.At(0).ToInteger();
	settings->INFANTRY_VERTICAL_MOUSE_SENS = args.At(0).ToInteger();
}

void Callbacks::vehicleSensCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->VEHICLE_HORIZONTAL_MOUSE_SENS = args.At(0).ToInteger();
	settings->VEHICLE_VERTICAL_MOUSE_SENS = args.At(0).ToInteger();
}

void Callbacks::mouseAccelerationCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->MOUSE_ACCELERATION = args.At(0).ToInteger();
}

void Callbacks::mouseInvertedCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->INVERT_MOUSE = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::centeredCrosshairCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	std::string cmd("camera.crosshair ");
	cmd.append(std::to_string(args.At(0).ToInteger()));
	Modules::CommandMap::Instance().ExecuteCommand(cmd);
}

void Callbacks::rawInputCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	std::string cmd("input.rawinput ");
	cmd.append(std::to_string(args.At(0).ToInteger()));
	Modules::CommandMap::Instance().ExecuteCommand(cmd);
}

void Callbacks::resolutionCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: needs a hook to apply
	settings->HORIZONTAL_RESOLUTION = args.At(0).ToInteger();
	settings->VERTICAL_RESOLUTION = args.At(1).ToInteger();
}

void Callbacks::textureResolutionCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: needs a game restart
	settings->TEXTURE_RESOLUTION = args.At(0).ToInteger();
}

void Callbacks::textureFilteringCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ? I cannot tell a difference between low and high.
	settings->TEXTURE_FILTERING = args.At(0).ToInteger();
}

void Callbacks::lightningQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->LIGHTNING_QUALITY = args.At(0).ToInteger();
}

void Callbacks::effectsQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->EFFECTS_QUALITY = args.At(0).ToInteger();
}

void Callbacks::shadowQualityCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->SHADOW_QUALITY = args.At(0).ToInteger();
}

void Callbacks::detailsLevelCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->DETAILS_LEVEL = args.At(0).ToInteger();
}

void Callbacks::postProcessingCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->POSTPROCESSING = args.At(0).ToInteger();
}

void Callbacks::motionBlurCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->MOTION_BLUR = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::brightnessCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies immediately
	settings->BRIGHTNESS = args.At(0).ToInteger();
}

void Callbacks::fullScreenCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: needs a hook to apply
	settings->FULL_SCREEN = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::VSyncCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->VSYNC = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::antiAliasingCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// Applies ?
	settings->ANTI_ALIASING = args.At(0).ToBoolean() ? 1 : 0;
}

void Callbacks::masterVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: not applied immediately
	settings->MASTER_VOLUME = args.At(0).ToInteger();
}

void Callbacks::sfxVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: not applied immediately
	settings->SFX_VOLUME = args.At(0).ToInteger();
}

void Callbacks::musicVolumeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	// TODO: not applied immediately
	settings->MUSIC_VOLUME = args.At(0).ToInteger();
}

void Callbacks::lanEnabledCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::gameTypeCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::sendInput(UINT vKeyCode)
{
	INPUT ip;

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = vKeyCode;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void Callbacks::changeMapTemp(int mapIndex) // this is a temp thing/hack; replace with a proper hook later
{
	while (true)
	{
		switch (*state)
		{
		case 40: // Game is loading
		case 54: // Game is loading
			break;

		case 24: // Start screen
			if (!startScreenSelecter)
			{
				Patch(0x6B0F4C, { 0x89, 0x0D, 0xC8, 0xC4, 0x51, 0x00 }).Apply(); // mov [0051C4C8], ecx
				uint32_t* tempStorage = (uint32_t*)0x51C4C8;
				while (*tempStorage == 0xCCCCCCCC)
				{
					Sleep(25);
				}
				Patch(0x6B0F4C, { 0x8B, 0x81, 0xF0, 0x00, 0x00, 0x00 }).Apply(); // mov eax,[ecx+000000F0]
				startScreenSelecter = (uint16_t*) (((uint8_t*)*tempStorage) + 0xF0);
			}
			*startScreenSelecter = 46576; // set it to second option (multiplayer)
			Sleep(100);
			sendInput(0x41);
			break;

		case 39: // Multiplayer screen
			if (!multiplayerScreenSelector)
			{
				Patch(0x6B0F4C, { 0x89, 0x0D, 0xD3, 0xC4, 0x51, 0x00 }).Apply(); // mov [0051C4D3], ecx
				uint32_t* tempStorage = (uint32_t*)0x51C4D3;
				while (*tempStorage == 0xCCCCCCCC)
				{
					Sleep(25);
				}
				Patch(0x6B0F4C, { 0x8B, 0x81, 0xF0, 0x00, 0x00, 0x00 }).Apply(); // mov eax,[ecx+000000F0]
				multiplayerScreenSelector = (uint16_t*)(((uint8_t*)*tempStorage) + 0xF0);
			}
			*multiplayerScreenSelector = 9968; // set it to fourth option (map select)
			Sleep(100);
			sendInput(0x41);
			break;

		case 41: // Map selection screen
			if (!mapSelector)
			{
				Patch(0x6B0F4C, { 0x89, 0x0D, 0xDB, 0xC4, 0x51, 0x00 }).Apply(); // mov [0051C4DB], ecx
				uint32_t* tempStorage = (uint32_t*)0x51C4DB;
				while (*tempStorage == 0xCCCCCCCC && *tempStorage != (uint32_t) multiplayerScreenSelector)
				{
					Sleep(25);
				}
				Patch(0x6B0F4C, { 0x8B, 0x81, 0xF0, 0x00, 0x00, 0x00 }).Apply(); // mov eax,[ecx+000000F0]
				mapSelector = (uint16_t*)(((uint8_t*)*tempStorage) + 0xF0);
			}

			switch (mapIndex) // set mapSelector = mapindex + 2
			{
			case 0:
				*mapSelector = 44176;
				break;

			case 1:
				*mapSelector = 46512;
				break;

			case 2:
				*mapSelector = 48848;
				break;

			case 3:
				*mapSelector = 51184;
				break;

			case 4:
				*mapSelector = 53520;
				break;

			case 5:
				*mapSelector = 55856;
				break;
			}
			Sleep(100);
			sendInput(0x41);
			Sleep(100);
			sendInput(0x41);
			return;

		case 38: // forge screen
			sendInput(0x42);
			break;

		default:
			break;
		}
		Sleep(100);
	}
}

void Callbacks::mapCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&changeMapTemp, (LPVOID) args.At(0).ToInteger(), 0, 0); // hack: use CreateThread's pointer argument as integer
}

void Callbacks::forgeMapCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::scoreToWinCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{

}

void Callbacks::startGameCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args) {
	OutputDebugString("Start game!");
}

void Callbacks::connectCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	Menu::Instance().disableMenu();

	std::string cmd("connect ");
	cmd.append(Awesomium::ToString(args.At(0).ToString()));
	Modules::CommandMap::Instance().ExecuteCommand(cmd);
}