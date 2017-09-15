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
	typedef void(__cdecl *Video_CallsD3DEndScene_ptr)(void);
	Video_CallsD3DEndScene_ptr Video_CallsD3DEndSceneOrginal;
	HRESULT(__stdcall *origResetPtr)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	HRESULT(__stdcall *origPresentPtr)(LPDIRECT3DDEVICE9, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

	bool CreateDeviceHook(bool windowless, bool nullRefDevice);
	void Video_CallsEndSceneHook();
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
		Video_CallsD3DEndSceneOrginal = reinterpret_cast<Video_CallsD3DEndScene_ptr>(0xA21510);
		origResetPtr = reinterpret_cast<decltype(origResetPtr)>(directXVTable[16]);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach((PVOID*)&Video_CallsD3DEndSceneOrginal, &Video_CallsEndSceneHook); // redirect origEndScenePtr to newEndScene
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

	void Video_CallsEndSceneHook()
	{
		pDevice = *reinterpret_cast<LPDIRECT3DDEVICE9*>(0x50DADDC);

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
			webRenderer->Render(pDevice);

		Video_CallsD3DEndSceneOrginal();
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
