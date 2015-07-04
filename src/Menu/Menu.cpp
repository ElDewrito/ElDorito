#include "Menu.hpp"
#include "..\Console\GameConsole.hpp"
#include <fstream>
#include <Awesomium\WebPreferences.h>

void Menu::startMenu()
{
	auto& menu = Menu::Instance();

	if (!menu.initAwesomium())
	{
		return;
	}

	SDL_Event ev;

	while (true)
	{
		if (!menu.menuEnabled)
		{
			if (!menu.switchedBackToGame)
			{
				ShowWindow(menu.hWnd, SW_SHOW);
				SDL_HideWindow(menu.window);
				menu.switchedBackToGame = true;
			}
			Sleep(1000);
			continue;
		}

		menu.switchedBackToGame = false;

		while (menu.webView->IsLoading())
		{
			menu.webCore->Update();
		}

		if (!menu.sdlInit)
		{
			menu.sdlInit = true;
			if (!menu.initSDL())
			{
				return;
			}
		}

		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				menu.disableMenu();
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
			else if (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_F11)
			{
				menu.disableMenu();
			}
			else if (ev.type == SDL_MOUSEWHEEL)
			{
				if (ev.wheel.y > 0)
				{
					menu.webView->InjectMouseWheel(15, 0);
				}
				else
				{
					menu.webView->InjectMouseWheel(-15, 0);
				}
			}
		}

		menu.webCore->Update();

		SDL_BlitSurface(menu.imageSurface, 0, menu.windowSurface, 0);
		SDL_UpdateWindowSurface(menu.window);
	}
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

bool Menu::doesDirExist(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

bool Menu::initAwesomium()
{
	webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
	webView = webCore->CreateWebView(Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, 0, Awesomium::kWebViewType_Offscreen);
	webView->LoadURL(Awesomium::WebURL(Awesomium::WSLit("http://vicelio.github.io/menu/")));

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
		methodDispatcher.Bind(callbacksObject, Awesomium::WSLit("connect"), JSDelegate(&callbacks, &Callbacks::connectCallback));
	}

	webView->set_js_method_handler(&methodDispatcher);
}

void Menu::toggleMenu()
{
	menuEnabled = !menuEnabled;
	if (menuEnabled)
	{
		ShowWindow(hWnd, SW_HIDE);
		while (!window)
		{
			Sleep(100);
		}
		SDL_ShowWindow(window);
	}
}

void Menu::disableMenu()
{
	menuEnabled = false;
}
