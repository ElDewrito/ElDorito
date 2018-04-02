#include "DirectXHook.hpp"
#include <Windows.h>
#include <detours.h>
#include <d3d9.h>
#include "../Web/WebRenderer.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"
#include "../Modules/ModuleGame.hpp"

// Disable warnings about possible data loss
#pragma warning (disable : 4244)

namespace
{
	LPDIRECT3DDEVICE9 pDevice;
	typedef void(__cdecl *Video_CallsD3DEndScene_ptr)(void);
	Video_CallsD3DEndScene_ptr Video_CallsD3DEndSceneOrginal;
	HRESULT(__stdcall *origResetPtr)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	HRESULT(__stdcall *origPresentPtr)(LPDIRECT3DDEVICE9, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);
	HRESULT(__stdcall *origEndScenePtr)(LPDIRECT3DDEVICE9);

	bool CreateDeviceHook(bool windowless, bool nullRefDevice);
	HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 device);
	void Video_CallsEndSceneHook();
	HRESULT __stdcall ResetHook(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS *params);
	void HandleTakeScreenshotHook();

	LARGE_INTEGER start, end, elapsed, freq;
}

namespace DirectXHook
{
	void ApplyAll()
	{
		Hook(0x620386, CreateDeviceHook, HookFlags::IsCall).Apply();
		Hook(0x62152B, HandleTakeScreenshotHook, HookFlags::IsCall).Apply();
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
		origEndScenePtr = reinterpret_cast<decltype(origEndScenePtr)>(directXVTable[42]);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		//DetourAttach((PVOID*)&Video_CallsD3DEndSceneOrginal, &Video_CallsEndSceneHook); // redirect Video_CallsD3DEndSceneOrginal to Video_CallsEndSceneHook
		DetourAttach((PVOID*)&origResetPtr, &ResetHook); // redirect DrawIndexedPrimitive to newDrawIndexedPrimitive
		DetourAttach((PVOID*)&origEndScenePtr, &EndSceneHook);
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);

		if (DetourTransactionCommit() != NO_ERROR)
		{
			OutputDebugString("DirectX hooks failed.");
			return false;
		}

		auto webRenderer = WebRenderer::GetInstance();
		auto webDebugging = ElDorito::Instance().IsWebDebuggingEnabled();
		return webRenderer->InitRenderer(device) && webRenderer->Init("dew://ui/", webDebugging);
	}

	HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 device)
	{

		//Fixes the viewport if the game is in fullscreen with an incorrect aspect ratio.
		auto *windowResolution = reinterpret_cast<int *>(0x19106E4);
		D3DVIEWPORT9 viewport;
		device->GetViewport(&viewport);
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = windowResolution[0];
		viewport.Height = windowResolution[1];
		device->SetViewport(&viewport);

		// Update the web renderer
		auto webRenderer = WebRenderer::GetInstance();
		if (webRenderer->Initialized() && webRenderer->IsRendering())
			webRenderer->Render(device);

		if(Modules::ModuleGame::Instance().VarFpsLimiter->ValueInt == 0)
			return (*origEndScenePtr)(device);

		QueryPerformanceCounter(&end);

		elapsed.QuadPart = end.QuadPart - start.QuadPart;
		elapsed.QuadPart *= 100000000.0;
		elapsed.QuadPart /= freq.QuadPart;

		while (elapsed.QuadPart < 1666666.66)
		{
			QueryPerformanceCounter(&end);
			elapsed.QuadPart = end.QuadPart - start.QuadPart;
			elapsed.QuadPart *= 100000000.0;
			elapsed.QuadPart /= freq.QuadPart;
		}

		QueryPerformanceCounter(&start);

		return (*origEndScenePtr)(device);
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

	void HandleTakeScreenshotHook()
	{
		const auto write_jpeg = (bool(*)(BYTE *pBits, int width, int height, wchar_t *path))(0x0060F5D0);

		auto &takeScreenshot = *(uint8_t*)0x0244DEE8;
		if (!takeScreenshot)
			return;
		takeScreenshot = 0;

		IDirect3DSurface9 *backBuffer, *tmpBuffer;
		D3DSURFACE_DESC desc;
		D3DLOCKED_RECT lockedRect;

		auto device = *(IDirect3DDevice9**)0x050DADDC;

		if (SUCCEEDED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
		{
			if (SUCCEEDED(backBuffer->GetDesc(&desc)) && (desc.Format == D3DFMT_A8R8G8B8 || desc.Format == D3DFMT_X8R8G8B8) &&
				SUCCEEDED(device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &tmpBuffer, nullptr)))
			{
				if (SUCCEEDED(device->GetRenderTargetData(backBuffer, tmpBuffer)) &&
					SUCCEEDED(tmpBuffer->LockRect(&lockedRect, nullptr, D3DLOCK_READONLY)))
				{
					write_jpeg((BYTE*)lockedRect.pBits, desc.Width, desc.Height, (wchar_t*)0x0244DFC0);
					tmpBuffer->UnlockRect();
				}
				tmpBuffer->Release();
			}

			backBuffer->Release();
		}
	}
}
