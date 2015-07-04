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

void changeMap(int mapIndex)
{
	while (true)
	{
		switch (*state)
		{
		case 40:
		case 54: // Game is loading
			break;

		case 24: // Start screen
			*startScreenSelecter = 46576; // set it to second option (multiplayer)
			Sleep(100);
			// push A
			break;

		case 39: // Multiplayer screen
			*lobbySelector = 9968; // set it to fourth option (map select)
			Sleep(100);
			// press A
			Sleep(100);
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
				*mapSelector = 535255856;
				break;
			}
			Sleep(100);
			// press A
			Sleep(100);
			// press A
			return;

		case 38: // forge screen
			// press B
			break;
		}
		Sleep(100);
	}
}

void Callbacks::mapCallback(Awesomium::WebView* caller, const Awesomium::JSArray& args)
{
	int mapIndex = args.At(0).ToInteger();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&changeMap, &mapIndex, 0, 0); // this is a temp thing/hack; replace with a proper hook later
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
