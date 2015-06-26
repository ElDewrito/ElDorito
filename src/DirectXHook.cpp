#include "DirectXHook.hpp"
#include "Console/GameConsole.hpp"
#include <detours.h>
#include "ElDorito.hpp"

uint32_t* DirectXHook::horizontalRes = 0;
uint32_t* DirectXHook::verticalRes = 0;
int DirectXHook::currentFontHeight = 0;

LPDIRECT3DDEVICE9 DirectXHook::pDevice = 0;
LPD3DXFONT DirectXHook::dxFont = 0;
HRESULT(__stdcall * DirectXHook::origEndScenePtr)(LPDIRECT3DDEVICE9) = 0;

HRESULT __stdcall DirectXHook::hookedEndScene(LPDIRECT3DDEVICE9 device)
{
	DirectXHook::pDevice = device;
	DirectXHook::drawChatInterface();
	return (*DirectXHook::origEndScenePtr)(device);
}

int DirectXHook::getTextWidth(const char *szText, LPD3DXFONT pFont)
{
	RECT rcRect = { 0, 0, 0, 0 };
	if (pFont)
	{
		// calculate required rect
		pFont->DrawText(NULL, szText, strlen(szText), &rcRect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
	}

	// return width
	return rcRect.right - rcRect.left;
}

void DirectXHook::drawChatInterface()
{
	auto& console = GameConsole::Instance();
	if ((console.getMsSinceLastConsoleOpen() > 10000 && !console.isConsoleShown()) || *((uint16_t*) 0x244D24A) == 16256) // 0x244D24A = 16256 means that tab is pressed in game (shows player k/d ratios)
	{
		return;
	}

	int x = (int)(0.05 * *horizontalRes);
	int y = (int)(0.65 * *verticalRes);
	int fontHeight = (int)(0.017 * *verticalRes);
	int inputTextBoxWidth = (int)(0.4 * *horizontalRes);
	int inputTextBoxHeight = fontHeight + (int) (0.769 * fontHeight);
	int horizontalSpacing = (int) (0.012 * inputTextBoxWidth);
	int verticalSpacingBetweenEachLine = (int)(0.154 * fontHeight);
	int verticalSpacingBetweenLinesAndInputBox = (int)(1.8 * fontHeight);

	if (!dxFont || fontHeight != currentFontHeight) {
		if (dxFont)
		{
			dxFont->Release();
		}

		D3DXCreateFont(pDevice, fontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &dxFont);
		currentFontHeight = fontHeight;
		return;
	}

	if (console.isConsoleShown())
	{
		int tempX = x;

		drawBox(tempX, y, getTextWidth("Console [F1]", dxFont) + 2 * horizontalSpacing, inputTextBoxHeight, console.consoleQueue.color, COLOR_BLACK);
		drawText(tempX + horizontalSpacing, y + (inputTextBoxHeight - fontHeight) / 2, console.consoleQueue.color, "Console [F1]");
		tempX += getTextWidth("Console [F1]", dxFont) + 2 * horizontalSpacing;

		drawBox(tempX, y, getTextWidth("Global Chat [F2]", dxFont) + 2 * horizontalSpacing, inputTextBoxHeight, console.globalChatQueue.color, COLOR_BLACK);
		drawText(tempX + horizontalSpacing, y + (inputTextBoxHeight - fontHeight) / 2, console.globalChatQueue.color, "Global Chat [F2]");
		tempX += getTextWidth("Global Chat [F2]", dxFont) + 2 * horizontalSpacing;

		drawBox(tempX, y, getTextWidth("Game Chat [F3]", dxFont) + 2 * horizontalSpacing, inputTextBoxHeight, console.gameChatQueue.color, COLOR_BLACK);
		drawText(tempX + horizontalSpacing, y + (inputTextBoxHeight - fontHeight) / 2, console.gameChatQueue.color, "Game Chat [F3]");
		tempX += getTextWidth("Game Chat [F3]", dxFont) + 2 * horizontalSpacing;
	}

	y -= verticalSpacingBetweenLinesAndInputBox;

	if (console.isConsoleShown())
	{
		drawBox(x, y, inputTextBoxWidth, inputTextBoxHeight, COLOR_WHITE, COLOR_BLACK);
		drawText(x + horizontalSpacing, y + (inputTextBoxHeight - fontHeight) / 2, COLOR_WHITE, (char*)console.inputLine.c_str());
	}

	y -= verticalSpacingBetweenLinesAndInputBox;

	for (int i = console.selectedQueue->startIndexForUI; i < console.selectedQueue->numOfLinesToShow + console.selectedQueue->startIndexForUI; i++)
	{
		drawText(x, y, COLOR_WHITE, (char*)console.selectedQueue->queue.at(i).c_str());
		y -= fontHeight + verticalSpacingBetweenEachLine;
	}
}

uint32_t* DirectXHook::getDirectXVTableMethod1()
{
	return (uint32_t*)(((uint8_t*)GetModuleHandle("d3d9.dll")) + 0x4E08);
}

uint32_t* DirectXHook::getDirectXVTableMethod2()
{
	// new sig: 30 9D -1 -1 80 95 -1 -1 40 95 -1 -1 10 7F -1 -1 A0 F4 -1 -1 10 FE -1 -1 70 1C -1 -1 A0 B5 -1 -1 40 F5 -1 -1 60 07 -1 -1 F0 11 -1 -1 20 11 -1 -1 20 F5
	// search for new sig gives 0 results even though it gives multiple results on cheat engine
	return 0;
}

uint32_t* DirectXHook::getDirectXVTableMethod3()
{
	HMODULE hDLL = GetModuleHandle("d3d9");
	LPDIRECT3D9(__stdcall*pDirect3DCreate9)(UINT) = (LPDIRECT3D9(__stdcall*)(UINT))GetProcAddress(hDLL, "Direct3DCreate9");
	LPDIRECT3D9 pD3D = pDirect3DCreate9(D3D_SDK_VERSION);
	D3DDISPLAYMODE d3ddm;
	HRESULT hRes = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3ddm.Format;
	IDirect3DDevice9 * ppReturnedDeviceInterface;	// interface IDirect3DDevice9 (pointer to array of pointers)
	HWND hWnd = *((HWND*)0x199C014); // HWND hWnd = FindWindowA(NULL, "HaloOnline cert_ms23_release_106708_0");
	hRes = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &d3dpp, &ppReturnedDeviceInterface);

	return *((uint32_t**)ppReturnedDeviceInterface); // returns an array of pointers
}

void DirectXHook::hookDirectX()
{
	horizontalRes = (uint32_t*)0x2301D08;
	verticalRes = (uint32_t*)0x2301D0C;

	uint32_t* directXVTable = getDirectXVTableMethod3();
	origEndScenePtr = (HRESULT(__stdcall *) (LPDIRECT3DDEVICE9)) directXVTable[42];

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&origEndScenePtr, &DirectXHook::hookedEndScene); // redirect origEndScenePtr to newEndScene

	if (DetourTransactionCommit() != NO_ERROR)
	{
		OutputDebugString("DirectX Hook for console failed.");
	}
}

void DirectXHook::drawText(int x, int y, DWORD color, char* text)
{
	RECT rect;
	SetRect(&rect, x, y, x, y);
	dxFont->DrawText(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
}

void DirectXHook::drawRect(int x, int y, int width, int height, DWORD Color)
{
	D3DRECT rec = { x, y, x + width, y + height };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, Color, 0, 0);
}

void DirectXHook::drawHorizontalLine(int x, int y, int width, D3DCOLOR Color)
{
	drawRect(x, y, width, 1, Color);
}

void DirectXHook::drawVerticalLine(int x, int y, int height, D3DCOLOR Color)
{
	drawRect(x, y, 1, height, Color);
}

void DirectXHook::drawBox(int x, int y, int width, int height, D3DCOLOR BorderColor, D3DCOLOR FillColor)
{
	drawRect(x, y, width, height, FillColor);
	drawHorizontalLine(x, y, width, BorderColor);
	drawVerticalLine(x, y, height, BorderColor);
	drawVerticalLine(x + width, y, height, BorderColor);
	drawHorizontalLine(x, y + height, width, BorderColor);
}