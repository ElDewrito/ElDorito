#include "DirectXHook.h"

DirectXHook* DirectXHook::thisInstance = nullptr;

DirectXHook::DirectXHook(GameConsole* gameConsoleInstance) : gameConsoleInstance(gameConsoleInstance)
{
	thisInstance = this;
	if (!hookDirectX())
	{
		OutputDebugString("DirectX Hook for console failed.");
	}
}

HRESULT __stdcall DirectXHook::hookedEndScene(LPDIRECT3DDEVICE9 device)
{
	thisInstance->pDevice = device;
	thisInstance->drawChatInterface();
	return (*thisInstance->origEndScenePtr)(device);
}

void DirectXHook::drawChatInterface() // TODO: get rid of fixed offsets so that it scales better on higher/lower resolutions
{
	if (gameConsoleInstance->getMsSinceLastConsoleOpen() > 10000 && !gameConsoleInstance->isConsoleShown())
	{
		return;
	}

	int x = (int)(0.05 * *horizontalRes);
	int y = (int)(0.55 * *verticalRes);
	int inputTextBoxWidth = (int)(0.4 * *horizontalRes);
	int inputTextBoxHeight = (int)(0.02 * *verticalRes);
	int fontHeight = (int)(0.0185 * *verticalRes);

	if (!dxFont) {
		D3DXCreateFont(pDevice, fontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &dxFont);
		return;
	}

	for (int i = 0; i < gameConsoleInstance->getNumOfLines(); i++)
	{
		drawText(x + 5, y, COLOR_WHITE, (char*)gameConsoleInstance->at(i).c_str());
		y += fontHeight + 2;
	}

	if (gameConsoleInstance->isConsoleShown())
	{
		y += 5;
		drawBox(x, y, inputTextBoxWidth, inputTextBoxHeight, COLOR_WHITE, COLOR_BLACK);
		drawText(x + 5, y + 2, COLOR_WHITE, (char*)gameConsoleInstance->getInputLine().c_str());
	}
}

uint32_t* DirectXHook::getDirectXVTableMethod1()
{
	return (uint32_t*)(((uint8_t*)GetModuleHandle("d3d9.dll")) + 0x869C); // or try d3d9.dll+0x8958 if that fails
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

bool DirectXHook::hookDirectX()
{
	uint32_t* directXVTable = getDirectXVTableMethod3();
	origEndScenePtr = (HRESULT(__stdcall *) (LPDIRECT3DDEVICE9)) directXVTable[42];

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&origEndScenePtr, &DirectXHook::hookedEndScene); // redirect origEndScenePtr to newEndScene

	return DetourTransactionCommit() == NO_ERROR;
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