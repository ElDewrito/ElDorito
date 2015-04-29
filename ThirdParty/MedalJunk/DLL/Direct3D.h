#pragma once

#include <d3d9.h>
#include <cstdint>

namespace D3DHook {

typedef HRESULT (WINAPI* tEndScene) (LPDIRECT3DDEVICE9);
typedef HRESULT (WINAPI* tReset) (LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);

extern LPDIRECT3DDEVICE9 pDevice;
extern tEndScene originalEndScene;
extern tReset originalReset;

HRESULT __stdcall endScene(LPDIRECT3DDEVICE9 pDevice);
HRESULT __stdcall reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

};