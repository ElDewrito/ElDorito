#include "Menu.hpp"
#include <fstream>
#include "..\Console\GameConsole.hpp"

void Menu::startMenu()
{
	auto& menu = Menu::Instance();

	if (!menu.initAwesomium())
	{
		return;
	}

	if (!menu.initSDL())
	{
		return;
	}

	startLoop();
}

Menu::Menu()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&startMenu, 0, 0, 0);
}

Menu::~Menu()
{
	SDL_FreeSurface(imageSurface);
	SDL_FreeSurface(windowSurface);
	SDL_DestroyWindow(window);
	SDL_Quit();

	menuEnabled = false;
	webView->Destroy();
	Sleep(100);
	Awesomium::WebCore::Shutdown();
}

bool Menu::doesFileExist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

bool Menu::initAwesomium()
{
	webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
	webView = webCore->CreateWebView(Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, 0, Awesomium::kWebViewType_Offscreen);

	char pathToOurDirectory[260];
	GetModuleFileName(NULL, pathToOurDirectory, 260);
	PathRemoveFileSpec(pathToOurDirectory);

	std::string fullPath(pathToOurDirectory);
	fullPath.append("\\mods\\menus\\default\\index.html");

	if (!doesFileExist(fullPath.c_str()))
	{
		GameConsole::Instance().PushLineFromGameToUIQueues("Error: HALO_FOLDER/mods/menus/default/index.html does not exist.");
		return false;
	}

	webView->LoadURL(Awesomium::WebURL(Awesomium::WSLit(fullPath.c_str())));

	bindCallbacks();
	return true;
}

bool Menu::initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		GameConsole::Instance().PushLineFromGameToUIQueues(SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("HTML5 Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, SDL_WINDOW_SHOWN);

	if (!window)
	{
		GameConsole::Instance().PushLineFromGameToUIQueues(SDL_GetError());
		return false;
	}

	windowSurface = SDL_GetWindowSurface(window);
	bitmapSurface = (Awesomium::BitmapSurface*) webView->surface();
	imageSurface = SDL_CreateRGBSurfaceFrom((void*) bitmapSurface->buffer(), Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, 4 * 8, Callbacks::settings->HORIZONTAL_RESOLUTION * 4, 0, 0, 0, 0x000000ff);
	if (!imageSurface)
	{
		GameConsole::Instance().PushLineFromGameToUIQueues(SDL_GetError());
		SDL_HideWindow(window);
		return false;
	}

	ShowWindow(hWnd, SW_HIDE);
	return true;
}

void Menu::bindCallbacks()
{
	Awesomium::JSValue result = webView->CreateGlobalJavascriptObject(Awesomium::WSLit("callbacks"));

	if (result.IsObject())
	{
		Awesomium::JSObject& callbacksObject = result.ToObject();
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("HUDShake"), JSDelegate(&callbacks, &Callbacks::HUDShakeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("playerMarkerColors"), JSDelegate(&callbacks, &Callbacks::playerMarkerColorsCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("fov"), JSDelegate(&callbacks, &Callbacks::fovCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("hostStartTimer"), JSDelegate(&callbacks, &Callbacks::hostStartTimerCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("playerName"), JSDelegate(&callbacks, &Callbacks::playerNameCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("showFPS"), JSDelegate(&callbacks, &Callbacks::showFPSCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("enableDXExtensions"), JSDelegate(&callbacks, &Callbacks::enableDXExtensionsCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("keyboardInput"), JSDelegate(&callbacks, &Callbacks::keyboardInputCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("toggleCrouch"), JSDelegate(&callbacks, &Callbacks::toggleCrouchCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("infantrySens"), JSDelegate(&callbacks, &Callbacks::infantrySensCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("vehicleSens"), JSDelegate(&callbacks, &Callbacks::vehicleSensCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("mouseAcceleration"), JSDelegate(&callbacks, &Callbacks::mouseAccelerationCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("mouseInverted"), JSDelegate(&callbacks, &Callbacks::mouseInvertedCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("centeredCrosshair"), JSDelegate(&callbacks, &Callbacks::centeredCrosshairCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("rawInput"), JSDelegate(&callbacks, &Callbacks::rawInputCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("resolution"), JSDelegate(&callbacks, &Callbacks::resolutionCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("textureResolution"), JSDelegate(&callbacks, &Callbacks::textureResolutionCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("textureFiltering"), JSDelegate(&callbacks, &Callbacks::textureFilteringCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("lightningQuality"), JSDelegate(&callbacks, &Callbacks::lightningQualityCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("effectsQuality"), JSDelegate(&callbacks, &Callbacks::effectsQualityCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("shadowQuality"), JSDelegate(&callbacks, &Callbacks::shadowQualityCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("detailsLevel"), JSDelegate(&callbacks, &Callbacks::detailsLevelCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("postProcessing"), JSDelegate(&callbacks, &Callbacks::postProcessingCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("motionBlur"), JSDelegate(&callbacks, &Callbacks::motionBlurCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("brightness"), JSDelegate(&callbacks, &Callbacks::brightnessCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("fullScreen"), JSDelegate(&callbacks, &Callbacks::fullScreenCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("VSync"), JSDelegate(&callbacks, &Callbacks::VSyncCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("antiAliasing"), JSDelegate(&callbacks, &Callbacks::antiAliasingCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("masterVolume"), JSDelegate(&callbacks, &Callbacks::masterVolumeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("sfxVolume"), JSDelegate(&callbacks, &Callbacks::sfxVolumeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("musicVolume"), JSDelegate(&callbacks, &Callbacks::musicVolumeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("lanEnabled"), JSDelegate(&callbacks, &Callbacks::lanEnabledCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("gameType"), JSDelegate(&callbacks, &Callbacks::gameTypeCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("map"), JSDelegate(&callbacks, &Callbacks::mapCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("forgeMap"), JSDelegate(&callbacks, &Callbacks::forgeMapCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("scoreToWin"), JSDelegate(&callbacks, &Callbacks::scoreToWinCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("startGame"), JSDelegate(&callbacks, &Callbacks::startGameCallback));
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("server"), JSDelegate(&callbacks, &Callbacks::serverCallback));

		// TODO: TEMP: remove
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("connect"), JSDelegate(&callbacks, &Callbacks::connectCallback));
	}

	webView->set_js_method_handler(&methodDispatcher);
}

void Menu::startLoop()
{
	auto& menu = Menu::Instance();
	SDL_Event ev;

	while (menu.menuEnabled)
	{
		while (menu.webView->IsLoading())
		{
			menu.webCore->Update();
		}

		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				menu.menuEnabled = false;
				continue;
			}
			else if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT)
			{
				menu.webView->InjectMouseUp(Awesomium::kMouseButton_Left);
			}
			else if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT)
			{
				menu.webView->InjectMouseDown(Awesomium::kMouseButton_Left);
			}
			else if (ev.type == SDL_MOUSEMOTION)
			{
				menu.webView->InjectMouseMove(ev.button.x, ev.button.y);
			}
		}

		menu.webCore->Update();

		SDL_BlitSurface(menu.imageSurface, 0, menu.windowSurface, 0);
		SDL_UpdateWindowSurface(menu.window);
	}

	ShowWindow(menu.hWnd, SW_SHOW);
}

void Menu::toggleMenu()
{
	menuEnabled = !menuEnabled;
	if (menuEnabled)
	{
		startLoop();
	}
}

void Menu::disableMenu()
{
	menuEnabled = false;
}
