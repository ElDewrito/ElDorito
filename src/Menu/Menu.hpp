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
	HWND hWnd = *((HWND*)0x199C014);
	SDL_Window* window = nullptr;
	SDL_Surface* windowSurface = nullptr;
	SDL_Surface* imageSurface = nullptr;
	Awesomium::BitmapSurface* bitmapSurface = nullptr;

	MethodDispatcher methodDispatcher;
	Callbacks callbacks{};

	Awesomium::WebCore* webCore;
	Awesomium::WebView* webView;
	POINT oldCursorLocation;
	POINT newCursorLocation;
	DWORD lastLeftClick = GetTickCount();

	static void startAwesomiumLoop();
	void handleMouseInput();
	void initAwesomium();
	void initSDL();
	void bindCallbacks();

public:
	bool menuEnabled = false; // TODO: change back to true

	Menu();
	~Menu();
};