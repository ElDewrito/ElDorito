#pragma once
#include <stdint.h>
#include <d3dx9core.h>

class DirectXHook {
private:
	static uint32_t* horizontalRes;
	static uint32_t* verticalRes;

	static LPDIRECT3DDEVICE9 pDevice;
	static HRESULT(__stdcall * origEndScenePtr)(LPDIRECT3DDEVICE9);
	static HRESULT __stdcall hookedEndScene(LPDIRECT3DDEVICE9 device);
	static HRESULT(__stdcall * DirectXHook::origResetPtr)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	static HRESULT __stdcall hookedReset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS *params);

	static bool createDeviceHook(bool windowless, bool nullRefDevice);

public:
	static void applyPatches();
	static bool hookDirectX(LPDIRECT3DDEVICE9 device);
};