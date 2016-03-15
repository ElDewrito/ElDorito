#include "DirectXHook.hpp"
#include <detours.h>
#include "Web/WebRenderer.hpp"
#include "Patch.hpp"

uint32_t* DirectXHook::horizontalRes = 0;
uint32_t* DirectXHook::verticalRes = 0;

LPDIRECT3DDEVICE9 DirectXHook::pDevice;
HRESULT(__stdcall * DirectXHook::origEndScenePtr)(LPDIRECT3DDEVICE9);
HRESULT(__stdcall * DirectXHook::origResetPtr)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);

using namespace Anvil::Client::Rendering;

HRESULT __stdcall DirectXHook::hookedEndScene(LPDIRECT3DDEVICE9 device)
{
	pDevice = device;

	//Fixes the viewport if the game is in fullscreen with an incorrect aspect ratio.
	D3DVIEWPORT9 viewport;
	pDevice->GetViewport(&viewport);
	viewport.X = 0;
	pDevice->SetViewport(&viewport);

	// Update the web renderer
	auto webRenderer = WebRenderer::GetInstance();
	if (webRenderer->Initialized() && webRenderer->IsRendering())
		webRenderer->Render(device);

	return (*origEndScenePtr)(device);
}

HRESULT __stdcall DirectXHook::hookedReset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS *params)
{
	auto webRenderer = WebRenderer::GetInstance();
	webRenderer->PreReset();
	auto result = origResetPtr(device, params);
	webRenderer->PostReset();
	return result;
}

void DirectXHook::applyPatches()
{
	Hook(0x620386, createDeviceHook, HookFlags::IsCall).Apply();
}

bool DirectXHook::createDeviceHook(bool windowless, bool nullRefDevice)
{
	typedef bool(*CreateDevicePtr)(bool windowless, bool nullRefDevice);
	auto CreateDevice = reinterpret_cast<CreateDevicePtr>(0xA21B40);
	if (!CreateDevice(windowless, nullRefDevice))
		return false;
	return hookDirectX(*reinterpret_cast<LPDIRECT3DDEVICE9*>(0x50DADDC));
}

bool DirectXHook::hookDirectX(LPDIRECT3DDEVICE9 device)
{
	horizontalRes = (uint32_t*)0x2301D08;
	verticalRes = (uint32_t*)0x2301D0C;

	auto directXVTable = *((uint32_t**)device);	// d3d9 interface ptr
	origEndScenePtr = reinterpret_cast<decltype(origEndScenePtr)>(directXVTable[42]);
	origResetPtr = reinterpret_cast<decltype(origResetPtr)>(directXVTable[16]);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&origEndScenePtr, &DirectXHook::hookedEndScene); // redirect origEndScenePtr to newEndScene
	DetourAttach((PVOID*)&origResetPtr, &DirectXHook::hookedReset); // redirect DrawIndexedPrimitive to newDrawIndexedPrimitive
	if (DetourTransactionCommit() != NO_ERROR)
	{
		OutputDebugString("DirectX hooks failed.");
		return false;
	}

	auto webRenderer = WebRenderer::GetInstance();
	if (!webRenderer->InitRenderer(device) || !webRenderer->Init("dew://ui/"))
		return false;
	return true;
}