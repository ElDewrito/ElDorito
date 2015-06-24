#pragma once
#include "Callbacks.h"
#include "MethodDispatcher.h"
#include <windows.h>
#include <detours.h>
#include <d3dx9.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/JSValue.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

class DirectXHook {
private:
	HWND hWnd = *((HWND*)0x199C014);

	LPDIRECT3DDEVICE9 pDevice{ 0 };
	HRESULT(__stdcall * origEndScenePtr)(LPDIRECT3DDEVICE9){ 0 };
	static HRESULT __stdcall hookedEndScene(LPDIRECT3DDEVICE9 device);
	static DirectXHook* thisInstance;

	IDirect3DVertexBuffer9* quadVertexBuffer = nullptr;
	IDirect3DTexture9* texture = nullptr;
	D3DLOCKED_RECT lockRect;

	MethodDispatcher methodDispatcher;
	Callbacks callbacks{};

	bool menuEnabled = true;
	bool awesomiumReady = false;
	Awesomium::WebCore* webCore;
	Awesomium::WebView* webView;
	POINT oldCursorLocation;
	POINT newCursorLocation;
	DWORD lastLeftClick = GetTickCount();

	uint32_t* getDirectXVTableMethod1();
	uint32_t* getDirectXVTableMethod2();
	uint32_t* getDirectXVTableMethod3();
	bool hookDirectX();
	void initDirectX();
	void handleMouseInput();
	void initAwesomium();
	void bindCallbacks();
public:
	DirectXHook();
	~DirectXHook();
};

struct Vertex
{
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v; // texture coordinates
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};