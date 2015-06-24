#include "DirectXHook.h"

DirectXHook* DirectXHook::thisInstance = nullptr;

DirectXHook::DirectXHook()
{
	thisInstance = this;

	GetCursorPos(&oldCursorLocation);
	ScreenToClient(hWnd, &oldCursorLocation);

	initAwesomium();

	if (!hookDirectX())
	{
		OutputDebugString("DirectX Hook for console failed.");
	}

	while (true)
	{
		if (!thisInstance->menuEnabled)
		{
			Sleep(1000);
			continue;
		}

		while (thisInstance->webView->IsLoading())
		{
			thisInstance->webCore->Update();
		}

		awesomiumReady = true;
		thisInstance->handleMouseInput();
		thisInstance->webCore->Update();
	}
}

DirectXHook::~DirectXHook()
{
	menuEnabled = false;
	webView->Destroy();
	Sleep(100);
	Awesomium::WebCore::Shutdown();
}

HRESULT __stdcall DirectXHook::hookedEndScene(LPDIRECT3DDEVICE9 device)
{
	thisInstance->pDevice = device;

	if (!thisInstance->menuEnabled || !thisInstance->awesomiumReady)
	{
		return (*thisInstance->origEndScenePtr)(device);
	}

	if (!thisInstance->texture)
	{
		thisInstance->initDirectX();
	}

	thisInstance->pDevice->SetTexture(0, thisInstance->texture); // This needs to be called every EndScene call for some reason

	thisInstance->texture->LockRect(0, &thisInstance->lockRect, 0, 0);
	memcpy(thisInstance->lockRect.pBits, ((Awesomium::BitmapSurface*) thisInstance->webView->surface())->buffer(), 4 * Callbacks::settings->HORIZONTAL_RESOLUTION * Callbacks::settings->VERTICAL_RESOLUTION);
	thisInstance->texture->UnlockRect(0);

	thisInstance->pDevice->SetStreamSource(0, thisInstance->quadVertexBuffer, 0, sizeof(Vertex));
	thisInstance->pDevice->SetFVF(Vertex::FVF);
	thisInstance->pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 4);

	return (*thisInstance->origEndScenePtr)(device);
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

void DirectXHook::handleMouseInput()
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

void DirectXHook::initDirectX()
{
	pDevice->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_DEFAULT, &quadVertexBuffer, 0);
	Vertex* v;
	quadVertexBuffer->Lock(0, 0, (void**)&v, 0);

	v[0] = { -1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f };
	v[1] = { -1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f };
	v[2] = { 1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f };
	v[3] = { -1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f };
	v[4] = { 1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f };
	v[5] = { 1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f };
	quadVertexBuffer->Unlock();

	D3DXCreateTexture(pDevice, Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, 0, D3DUSAGE_DYNAMIC, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &texture);

	pDevice->SetTexture(0, texture);
}

void DirectXHook::initAwesomium()
{
	webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
	webView = webCore->CreateWebView(Callbacks::settings->HORIZONTAL_RESOLUTION, Callbacks::settings->VERTICAL_RESOLUTION, 0, Awesomium::kWebViewType_Offscreen);

	char path[260];
	GetModuleFileName(NULL, path, 260);
	PathRemoveFileSpec(path);
	webView->LoadURL(Awesomium::WebURL(Awesomium::WSLit(std::string(path).append("\\mods\\menus\\default\\index.html").c_str())));

	bindCallbacks();
}

void DirectXHook::bindCallbacks()
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
	}

	webView->set_js_method_handler(&methodDispatcher);
}