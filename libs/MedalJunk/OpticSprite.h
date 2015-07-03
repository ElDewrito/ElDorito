#pragma once

#include "OpticException.h"
#include "OpticAnimation.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace Optic {

class OpticSpriteException : public OpticException {
public:
	OpticSpriteException() : OpticException("An unknown exception occured!") { }
	OpticSpriteException(std::string msg) : OpticException(msg) { };
};

class OpticSprite {
	D3DSURFACE_DESC surfaceDesc;
	D3DXVECTOR2 transformCentre, translateCentre;
	D3DXVECTOR3 position;
	AnimationState aniState;
	OpticAnimation animation;
	LPDIRECT3DTEXTURE9 texture;
	bool spritesheet, loop, played, animate;
	int width, height, blanks, fps, loopBegin, frames, currFrame, rows, columns, currRow;
	double lastTime, frameTime;
	
	bool dimensionChecks();
	bool advanceFrame(double time, int x, int y);
	void init(LPDIRECT3DTEXTURE9 texture);

public:
	OpticSprite(LPDIRECT3DTEXTURE9 texture);
	OpticSprite(LPDIRECT3DTEXTURE9 texture, OpticAnimation animation);
	void setProperties(int rows, int columns, int blanks, int fps, bool loop, int loopBegin);
	void setTranslationCentre(float x, float y);
	void setTransformationCentre(float x, float y);
	int draw(LPD3DXSPRITE sprite, double time, D3DXCOLOR colour, double offsetX, double offsetY);
	std::pair<int, int> dimensions() const;
};

};