#pragma once
#include "GameConsole.h"
#include <stdint.h>
#include <d3dx9core.h>

class GameConsole;

class DirectXHook {
private:
	CONST D3DCOLOR COLOR_RED = D3DCOLOR_ARGB(255, 255, 000, 000);
	CONST D3DCOLOR COLOR_GREEN = D3DCOLOR_ARGB(255, 127, 255, 000);
	CONST D3DCOLOR COLOR_ORANGE = D3DCOLOR_ARGB(255, 255, 140, 000);
	CONST D3DCOLOR COLOR_BLUE = D3DCOLOR_ARGB(255, 000, 000, 255);
	CONST D3DCOLOR COLOR_YELLOW = D3DCOLOR_ARGB(255, 255, 255, 51);
	CONST D3DCOLOR COLOR_BLACK = D3DCOLOR_ARGB(255, 000, 000, 000);
	CONST D3DCOLOR COLOR_GREY = D3DCOLOR_ARGB(255, 112, 112, 112);
	CONST D3DCOLOR COLOR_GOLD = D3DCOLOR_ARGB(255, 255, 215, 000);
	CONST D3DCOLOR COLOR_PINK = D3DCOLOR_ARGB(255, 255, 192, 203);
	CONST D3DCOLOR COLOR_PURPLE = D3DCOLOR_ARGB(255, 128, 000, 128);
	CONST D3DCOLOR COLOR_CYAN = D3DCOLOR_ARGB(255, 000, 255, 255);
	CONST D3DCOLOR COLOR_MAGNETA = D3DCOLOR_ARGB(255, 255, 000, 255);
	CONST D3DCOLOR COLOR_WHITE = D3DCOLOR_ARGB(255, 255, 255, 249);
	
	uint32_t* horizontalRes = (uint32_t*) 0x19106C0;
	uint32_t* verticalRes = (uint32_t*) 0x19106C4;
	GameConsole* gameConsoleInstance{0};

	LPDIRECT3DDEVICE9 pDevice{0};
	LPD3DXFONT dxFont{0};
	HRESULT(__stdcall * origEndScenePtr)(LPDIRECT3DDEVICE9){0};
	static HRESULT __stdcall hookedEndScene(LPDIRECT3DDEVICE9 device);
	static DirectXHook* thisInstance;
	
	uint32_t* getDirectXVTableMethod1();
	uint32_t* getDirectXVTableMethod2();
	uint32_t* getDirectXVTableMethod3();
	bool hookDirectX();
	void drawText(int x, int y, DWORD color, char* text);
	void drawRect(int x, int y, int width, int height, DWORD Color);
	void drawHorizontalLine(int x, int y, int width, D3DCOLOR Color);
	void drawVerticalLine(int x, int y, int height, D3DCOLOR Color);
	void drawBox(int x, int y, int width, int height, D3DCOLOR BorderColor, D3DCOLOR FillColor);
	void drawChatInterface();

public:
	DirectXHook(GameConsole* gameConsoleInstance);
};