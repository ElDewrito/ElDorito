#include "DirectXHook.hpp"
#include <Windows.h>
#include <detours.h>
#include <d3d9.h>
#include "../Web/WebRenderer.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"

namespace
{
	LPDIRECT3DDEVICE9 pDevice;
	HRESULT(__stdcall *origEndScenePtr)(LPDIRECT3DDEVICE9);
	HRESULT(__stdcall *origResetPtr)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	HRESULT(__stdcall *origPresentPtr)(LPDIRECT3DDEVICE9, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

	bool CreateDeviceHook(bool windowless, bool nullRefDevice);
	HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 device);
	HRESULT __stdcall ResetHook(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS *params);
}

namespace DirectXHook
{
	void ApplyAll()
	{
		Hook(0x620386, CreateDeviceHook, HookFlags::IsCall).Apply();
	}
}

namespace
{
	using namespace Anvil::Client::Rendering;

	bool HookDirectX(LPDIRECT3DDEVICE9 device)
	{
		auto directXVTable = *((uint32_t**)device);	// d3d9 interface ptr
		origEndScenePtr = reinterpret_cast<decltype(origEndScenePtr)>(directXVTable[42]);
		origResetPtr = reinterpret_cast<decltype(origResetPtr)>(directXVTable[16]);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach((PVOID*)&origEndScenePtr, &EndSceneHook); // redirect origEndScenePtr to newEndScene
		DetourAttach((PVOID*)&origResetPtr, &ResetHook); // redirect DrawIndexedPrimitive to newDrawIndexedPrimitive

		if (DetourTransactionCommit() != NO_ERROR)
		{
			OutputDebugString("DirectX hooks failed.");
			return false;
		}

		auto webRenderer = WebRenderer::GetInstance();
		auto webDebugging = ElDorito::Instance().IsWebDebuggingEnabled();
		return webRenderer->InitRenderer(device) && webRenderer->Init("dew://ui/", webDebugging);
	}

	bool CreateDeviceHook(bool windowless, bool nullRefDevice)
	{
		typedef bool(*CreateDevicePtr)(bool windowless, bool nullRefDevice);
		auto CreateDevice = reinterpret_cast<CreateDevicePtr>(0xA21B40);
		if (!CreateDevice(windowless, nullRefDevice))
			return false;
		return HookDirectX(*reinterpret_cast<LPDIRECT3DDEVICE9*>(0x50DADDC));
	}

	HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 device)
	{
		pDevice = device;

		//Fixes the viewport if the game is in fullscreen with an incorrect aspect ratio.
		auto *windowResolution = reinterpret_cast<int *>(0x19106E4);
		D3DVIEWPORT9 viewport;
		pDevice->GetViewport(&viewport);
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = windowResolution[0];
		viewport.Height = windowResolution[1];
		pDevice->SetViewport(&viewport);

		// Update the web renderer
		auto webRenderer = WebRenderer::GetInstance();
		if (webRenderer->Initialized() && webRenderer->IsRendering())
			webRenderer->Render(device);

		return (*origEndScenePtr)(device);
	}

	HRESULT __stdcall ResetHook(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS *params)
	{
		auto webRenderer = WebRenderer::GetInstance();
		webRenderer->PreReset();
		auto result = origResetPtr(device, params);
		webRenderer->PostReset();
		return result;
	}
}
