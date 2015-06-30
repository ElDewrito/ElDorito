#pragma once
#include "Callbacks.hpp"
#include "MethodDispatcher.hpp"
#include <windows.h>
#include <detours.h>
#include <d3dx9.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/JSValue.h>
#include <shlwapi.h>
#include "../Utils/Singleton.hpp"

#pragma comment(lib, "Shlwapi.lib")

class Menu : public Utils::Singleton<Menu>
{
private:
	HWND hWnd = *((HWND*)0x199C014);

	IDirect3DVertexBuffer9* quadVertexBuffer = nullptr;
	IDirect3DTexture9* texture = nullptr;
	D3DLOCKED_RECT lockRect;

	MethodDispatcher methodDispatcher;
	Callbacks callbacks{};

	bool awesomiumReady = false;
	Awesomium::WebCore* webCore;
	Awesomium::WebView* webView;
	POINT oldCursorLocation;
	POINT newCursorLocation;
	DWORD lastLeftClick = GetTickCount();

	static void startAwesomiumLoop();
	void handleMouseInput();
	void initAwesomium();
	void bindCallbacks();
public:
	bool menuEnabled = false; // TODO: change back to true

	Menu();
	~Menu();
	void drawMenu(LPDIRECT3DDEVICE9 device);
};

struct Vertex
{
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v; // texture coordinates
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};