#include "Menu.hpp"

void Menu::startAwesomiumLoop()
{
	auto& menu = Menu::Instance();

	menu.initAwesomium();
	menu.initSDL();

	while (true)
	{
		if (!menu.menuEnabled)
		{
			Sleep(1000);
			continue;
		}

		while (menu.webView->IsLoading())
		{
			menu.webCore->Update();
		}

		menu.handleMouseInput();
		menu.webCore->Update();

		SDL_BlitSurface(menu.imageSurface, 0, menu.windowSurface, 0);
		SDL_UpdateWindowSurface(menu.window);
	}
}

Menu::Menu()
{
	GetCursorPos(&oldCursorLocation);
	ScreenToClient(hWnd, &oldCursorLocation);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&startAwesomiumLoop, 0, 0, 0);
}

Menu::~Menu()
{
	menuEnabled = false;
	webView->Destroy();
	Sleep(100);
	Awesomium::WebCore::Shutdown();
}

void Menu::handleMouseInput()
{
	GetCursorPos(&newCursorLocation);
	ScreenToClient(hWnd, &newCursorLocation);
	if (oldCursorLocation.x != newCursorLocation.x || oldCursorLocation.y != newCursorLocation.y)
	{
		webView->InjectMouseMove(newCursorLocation.x, newCursorLocation.y);
		oldCursorLocation = newCursorLocation;
	}

	if (GetKeyState(VK_LBUTTON) < 0 && GetTickCount() - lastLeftClick > 200) {
		webView->InjectMouseDown(Awesomium::kMouseButton_Left);
		webView->InjectMouseUp(Awesomium::kMouseButton_Left);
		lastLeftClick = GetTickCount();
	}
}

void Menu::initAwesomium()
{
	webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
	webView = webCore->CreateWebView(Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, 0, Awesomium::kWebViewType_Offscreen);

	char path[260];
	GetModuleFileName(NULL, path, 260);
	PathRemoveFileSpec(path);
	webView->LoadURL(Awesomium::WebURL(Awesomium::WSLit(std::string(path).append("\\mods\\menus\\default\\index.html").c_str())));

	bindCallbacks();
}

void Menu::initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		OutputDebugString(SDL_GetError());
	}

	window = SDL_CreateWindowFrom(hWnd);

	if (!window)
	{
		OutputDebugString(SDL_GetError());
	}

	windowSurface = SDL_GetWindowSurface(window);
	bitmapSurface = (Awesomium::BitmapSurface*) webView->surface();
	imageSurface = SDL_CreateRGBSurfaceFrom((void*) bitmapSurface->buffer(), Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, 4 * 8, Callbacks::settings->HORIZONTAL_RESOLUTION * 4, 0, 0, 0, 0x000000ff);
	if (!imageSurface)
	{
		OutputDebugString(SDL_GetError());
	}
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
