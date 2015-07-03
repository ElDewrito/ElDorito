#pragma once
#include "Callbacks.hpp"
#include "MethodDispatcher.hpp"
#include <windows.h>
#include <detours.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/JSValue.h>
#include <shlwapi.h>
#include <SDL.h>
#include "../Utils/Singleton.hpp"

#pragma comment(lib, "Shlwapi.lib")

class Menu : public Utils::Singleton<Menu>
{
private:
	bool menuEnabled = false; // TODO: change back to true
	HWND hWnd = *((HWND*)0x199C014);
	SDL_Window* window = nullptr;
	SDL_Surface* windowSurface = nullptr;
	SDL_Surface* imageSurface = nullptr;

	MethodDispatcher methodDispatcher;
	Callbacks callbacks{};

	Awesomium::WebCore* webCore;
	Awesomium::WebView* webView;
	Awesomium::BitmapSurface* bitmapSurface = nullptr;

	static void startMenu();
	static void startLoop();
	bool initAwesomium();
	bool initSDL();
	void bindCallbacks();
	bool doesFileExist(const char *fileName);

public:
	Menu();
	~Menu();
	void toggleMenu();
	void disableMenu();
};