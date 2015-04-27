#include "Direct3D.h"
#include "Optic.h"
#include <d3dx9.h>

namespace D3DHook {

LPDIRECT3DDEVICE9 pDevice;
tEndScene originalEndScene = NULL;
tReset originalReset = NULL;

void delayed() {
	static bool done = false;

	if(done) {
		return;
	}

	Optic::load("halo3");
	done = true;
}
HRESULT __stdcall endScene(LPDIRECT3DDEVICE9 device) {
	pDevice = device;
	delayed();
	Optic::render();
	return originalEndScene(device);
}

HRESULT __stdcall reset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	pDevice = device;
	Optic::lost();
	auto res = originalReset(device, pPresentationParameters);
	Optic::reset();
	return res;
}

};