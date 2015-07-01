#include "DirectXHook.hpp"
#include "Console/GameConsole.hpp"
#include <detours.h>
#include "VoIP/MemberList.hpp"
#include "VoIP/TeamspeakClient.hpp"
#include "Modules/ModuleVoIP.hpp"
#include <teamspeak/public_definitions.h>
#include <teamspeak/public_errors.h>
#include <teamspeak/clientlib_publicdefinitions.h>
#include <teamspeak/clientlib.h>
#include "Menu/Menu.hpp"

uint32_t* DirectXHook::horizontalRes = 0;
uint32_t* DirectXHook::verticalRes = 0;
int DirectXHook::normalSizeCurrentFontHeight = 0;
int DirectXHook::largeSizeCurrentFontHeight = 0;
int DirectXHook::largeSizeFontHeight = 0;
int DirectXHook::normalSizeFontHeight = 0;

LPDIRECT3DDEVICE9 DirectXHook::pDevice = 0;
LPD3DXFONT DirectXHook::normalSizeFont = 0;
LPD3DXFONT DirectXHook::largeSizeFont = 0;
bool DirectXHook::drawVoIPSettings = false;

HRESULT(__stdcall * DirectXHook::origBeginScenePtr)(LPDIRECT3DDEVICE9) = 0;
HRESULT(__stdcall * DirectXHook::origEndScenePtr)(LPDIRECT3DDEVICE9) = 0;

HRESULT __stdcall DirectXHook::hookedBeginScene(LPDIRECT3DDEVICE9 device)
{
	if (Menu::Instance().menuEnabled)
	{
		return D3D_OK;
	}

	return (*DirectXHook::origBeginScenePtr)(device);
}

HRESULT __stdcall DirectXHook::hookedEndScene(LPDIRECT3DDEVICE9 device)
{
	if (Menu::Instance().menuEnabled)
	{
		return D3D_OK;
	}

	DirectXHook::pDevice = device;

	initFontsIfRequired();

	DirectXHook::drawVoipMembers();
	DirectXHook::drawChatInterface();
	if (drawVoIPSettings)
	{
		DirectXHook::drawVoipSettings();
	}

	return (*DirectXHook::origEndScenePtr)(device);
}

int DirectXHook::getTextWidth(const char *szText, LPD3DXFONT pFont)
{
	RECT rcRect = { 0, 0, 0, 0 };
	if (pFont)
	{
		pFont->DrawText(NULL, szText, strlen(szText), &rcRect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
	}
	int width = rcRect.right - rcRect.left;
	std::string text(szText);
	std::reverse(text.begin(), text.end());

	text = text.substr(0, text.find_first_not_of(' ') != std::string::npos ? text.find_first_not_of(' ') : 0);
	for(char c : text)
	{
		width += getSpaceCharacterWidth(pFont);
	}
	return width;
}

int DirectXHook::getSpaceCharacterWidth(LPD3DXFONT pFont)
{
	return getTextWidth("i i", pFont) - ((getTextWidth("i", pFont)) * 2);
}

void DirectXHook::drawVoipSettings()
{
	int x = (int)(0.28375 * *horizontalRes);
	int y = (int)(0.25 * *verticalRes);
	int width = (int)(0.5 * *horizontalRes);
	int height = (int)(0.3 * *verticalRes);
	int verticalSpacingBetweenEachLine = (int)(1.3 * largeSizeFontHeight);

	auto& voipvars = Modules::ModuleVoIP::Instance();
	drawBox(x-1, y-1, width+2, height+2, COLOR_WHITE, COLOR_WHITE);
	drawBox(x, y, width, height, COLOR_BLACK, COLOR_BLACK);
	drawText(centerTextHorizontally("ElDewrito VoIP Settings", x, width, largeSizeFont), y, COLOR_WHITE, "ElDewrito VoIP Settings", largeSizeFont);

	y += verticalSpacingBetweenEachLine;
	if (voipvars.VarVoIPPushToTalk->ValueInt == 0){
		drawText(centerTextHorizontally("Voice activation detection is enabled.", x, width, largeSizeFont), y, COLOR_WHITE, "Voice activation detection is enabled.", largeSizeFont);
		y += verticalSpacingBetweenEachLine;
		drawText(centerTextHorizontally("Change VoIP.PushToTalk to 1 for push to talk.", x, width, largeSizeFont), y, COLOR_WHITE, "Change VoIP.PushToTalk to 1 for push to talk.", largeSizeFont);
	}
	else
	{
		drawText(centerTextHorizontally("VoIP.PushToTalk is enabled. Hold Caps-Lock to talk", x, width, largeSizeFont), y, COLOR_WHITE, "VoIP.PushToTalk is enabled. Hold Caps-Lock to talk", largeSizeFont);
		y += verticalSpacingBetweenEachLine;
		drawText(centerTextHorizontally("Change VoIP.PushToTalk to 0 for Voice Activation", x, width, largeSizeFont), y, COLOR_WHITE, "Change VoIP.PushToTalk to 0 for Voice Activation", largeSizeFont);
	}
	y += verticalSpacingBetweenEachLine;

	unsigned int error;
	float result;
	uint64 vadTestscHandlerID = VoIPGetVadHandlerID();
	int vadTestTalkStatus = VoIPGetTalkStatus();

	if ((error = ts3client_getPreProcessorInfoValueFloat(vadTestscHandlerID, "decibel_last_period", &result)) != ERROR_ok) {
		drawText(centerTextHorizontally("Error getting vad level", x, width, largeSizeFont), y, COLOR_RED, "Error getting vad level", largeSizeFont);
	}
	else
	{
		drawText(centerTextHorizontally(("%.2f - %s", result, (vadTestTalkStatus == STATUS_TALKING ? "talking" : "not talking")), x, width, largeSizeFont), y, (vadTestTalkStatus == STATUS_TALKING ? COLOR_GREEN : COLOR_RED), ("%.2f - %s", result, (vadTestTalkStatus == STATUS_TALKING ? "talking" : "not talking")), largeSizeFont);
	}

	y += verticalSpacingBetweenEachLine * 2;

	drawText(centerTextHorizontally("Voice activation threshold: -50", x, width, largeSizeFont), y, COLOR_WHITE, "Voice activation threshold: -50", largeSizeFont);

	y += verticalSpacingBetweenEachLine;

	drawText(centerTextHorizontally("Push to talk key: Caps Lock", x, width, largeSizeFont), y, COLOR_WHITE, "Push to talk key: Caps Lock", largeSizeFont);
}

void DirectXHook::drawVoipMembers()
{
	auto& memberList = MemberList::Instance();

	int x = (int) (0.88 * *horizontalRes);
	int y = (int) (0.27 * *verticalRes);
	int fontHeight = (int)(0.017 * *verticalRes);
	int verticalSpacingBetweenEachLine = (int)(0.5 * fontHeight);

	for (size_t i = 0; i < memberList.memberList.size(); i++)
	{
		//TODO: If player is on red team, display red text. Blue, show blue text.
		//TODO: If game is slayer, or "no team" just display white text.
		drawText(x, y, COLOR_WHITE, memberList.memberList.at(i).c_str(), normalSizeFont);
		y += fontHeight + verticalSpacingBetweenEachLine;
	}
}

void DirectXHook::drawChatInterface()
{
	auto& console = GameConsole::Instance();
	if ((console.getMsSinceLastConsoleOpen() > 10000 && !console.showChat && !console.showConsole) || (GetAsyncKeyState(VK_TAB) & 0x8000 && !console.showChat && !console.showConsole))
	{
		return;
	}

	int x = (int)(0.05 * *horizontalRes);
	int y = (int)(0.65 * *verticalRes);
	int inputTextBoxWidth = (int)(0.5 * *horizontalRes);
	int inputTextBoxHeight = normalSizeFontHeight + (int)(0.769 * normalSizeFontHeight);
	int horizontalSpacing = (int)(0.012 * inputTextBoxWidth);
	int verticalSpacingBetweenEachLine = (int)(0.154 * normalSizeFontHeight);
	int verticalSpacingBetweenLinesAndInputBox = (int)(1.8 * normalSizeFontHeight);
	int verticalSpacingBetweenTopOfInputBoxAndFont = (inputTextBoxHeight - normalSizeFontHeight) / 2;
	size_t maxCharsPerLine = 105;

	if (console.showChat)
	{
		int tempX = x;

		drawBox(tempX, y, getTextWidth("Global Chat", normalSizeFont) + 2 * horizontalSpacing, inputTextBoxHeight, console.globalChatQueue.color, COLOR_BLACK);
		drawText(tempX + horizontalSpacing, y + verticalSpacingBetweenTopOfInputBoxAndFont, console.globalChatQueue.color, "Global Chat", normalSizeFont);
		tempX += getTextWidth("Global Chat", normalSizeFont) + 2 * horizontalSpacing;

		drawBox(tempX, y, getTextWidth("Game Chat", normalSizeFont) + 2 * horizontalSpacing, inputTextBoxHeight, console.gameChatQueue.color, COLOR_BLACK);
		drawText(tempX + horizontalSpacing, y + verticalSpacingBetweenTopOfInputBoxAndFont, console.gameChatQueue.color, "Game Chat", normalSizeFont);
		tempX += getTextWidth("Game Chat", normalSizeFont) + 2 * horizontalSpacing;
	}

	y -= verticalSpacingBetweenLinesAndInputBox;

	if (console.showChat || console.showConsole)
	{
		// Display current input
		drawBox(x, y, inputTextBoxWidth, inputTextBoxHeight, COLOR_WHITE, COLOR_BLACK);
		drawText(x + horizontalSpacing, y + verticalSpacingBetweenTopOfInputBoxAndFont, COLOR_WHITE, console.currentInput.currentInput.c_str(), normalSizeFont);

		// START: Line showing where the user currently is in the input field.
		if (console.getMsSinceLastConsoleBlink() > 300)
		{
			console.consoleBlinking = !console.consoleBlinking;
			console.lastTimeConsoleBlink = GetTickCount();
		}
		if (!console.consoleBlinking)
		{
			std::string currentInput = console.currentInput.currentInput;
			char currentChar;
			int width = 0;
			if (currentInput.length() > 0) {
				currentChar = currentInput[console.currentInput.currentPointerIndex];
				width = getTextWidth(currentInput.substr(0, console.currentInput.currentPointerIndex).c_str(), normalSizeFont) - 3;
			}
			else
			{
				width = -3;
			}
			drawText(x + horizontalSpacing + width, y + verticalSpacingBetweenTopOfInputBoxAndFont, COLOR_WHITE, "|", normalSizeFont);
		}
		// END: Line showing where the user currently is in the input field.
	}

	y -= verticalSpacingBetweenLinesAndInputBox;

	// Draw text from chat or console
	for (int i = console.selectedQueue->startIndexForScrolling; i < console.selectedQueue->numOfLinesToShow + console.selectedQueue->startIndexForScrolling; i++)
	{
		std::string line = console.selectedQueue->queue.at(i);

		if (line.size() > maxCharsPerLine)
		{
			std::vector<std::string> linesWrapped = std::vector < std::string > {};

			for (size_t i = 0; i < line.size(); i += maxCharsPerLine)
			{
				linesWrapped.push_back(line.substr(i, maxCharsPerLine));
			}

			for (int i = linesWrapped.size() - 1; i >= 0; i--)
			{
				drawText(x, y, COLOR_WHITE, linesWrapped.at(i).c_str(), normalSizeFont);
				y -= normalSizeFontHeight + verticalSpacingBetweenEachLine;
			}
		}
		else
		{
			drawText(x, y, COLOR_WHITE, line.c_str(), normalSizeFont);
			y -= normalSizeFontHeight + verticalSpacingBetweenEachLine;
		}
	}
}

uint32_t* DirectXHook::getDirectXVTable()
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
	HWND hWnd = *((HWND*)0x199C014);
	hRes = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &d3dpp, &ppReturnedDeviceInterface);

	return *((uint32_t**)ppReturnedDeviceInterface); // returns an array of pointers
}

void DirectXHook::hookDirectX()
{
	horizontalRes = (uint32_t*)0x2301D08;
	verticalRes = (uint32_t*)0x2301D0C;

	uint32_t* directXVTable = getDirectXVTable();
	origBeginScenePtr = (HRESULT(__stdcall *) (LPDIRECT3DDEVICE9)) directXVTable[41];
	origEndScenePtr = (HRESULT(__stdcall *) (LPDIRECT3DDEVICE9)) directXVTable[42];

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&origBeginScenePtr, &DirectXHook::hookedBeginScene); // redirect origEndScenePtr to newEndScene
	DetourAttach((PVOID*)&origEndScenePtr, &DirectXHook::hookedEndScene); // redirect origEndScenePtr to newEndScene

	if (DetourTransactionCommit() != NO_ERROR)
	{
		OutputDebugString("DirectX Hook for console failed.");
	}
}

void DirectXHook::drawText(int x, int y, DWORD color, const char* text, LPD3DXFONT pFont)
{
	RECT shadow1;
	RECT shadow2;
	RECT shadow3;
	RECT shadow4;
	SetRect(&shadow1, x + 1, y + 1, x + 1, y + 1);
	SetRect(&shadow2, x - 1, y + 1, x - 1, y + 1);
	SetRect(&shadow3, x + 1, y - 1, x + 1, y - 1);
	SetRect(&shadow4, x - 1, y - 1, x - 1, y - 1);
	pFont->DrawText(NULL, text, -1, &shadow1, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	pFont->DrawText(NULL, text, -1, &shadow2, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	pFont->DrawText(NULL, text, -1, &shadow3, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
	pFont->DrawText(NULL, text, -1, &shadow4, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

	RECT rect;
	SetRect(&rect, x, y, x, y);
	pFont->DrawText(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
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

int DirectXHook::centerTextHorizontally(const char* text, int x, int width, LPD3DXFONT pFont)
{
	return x + (width - getTextWidth(text, pFont)) / 2;
}

void DirectXHook::initFontsIfRequired()
{
	normalSizeFontHeight = (int)(0.017 * *verticalRes);
	largeSizeFontHeight = (int)(0.034 * *verticalRes); 

	if (!normalSizeFont || normalSizeFontHeight != normalSizeCurrentFontHeight) {
		if (normalSizeFont)
		{
			normalSizeFont->Release();
		}

		D3DXCreateFont(pDevice, normalSizeFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &normalSizeFont);
		normalSizeCurrentFontHeight = normalSizeFontHeight;
	}

	if (!largeSizeFont || largeSizeFontHeight != largeSizeCurrentFontHeight) {
		if (largeSizeFont)
		{
			largeSizeFont->Release();
		}

		D3DXCreateFont(pDevice, largeSizeFontHeight, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &largeSizeFont);
		largeSizeCurrentFontHeight = largeSizeFontHeight;
	}
}