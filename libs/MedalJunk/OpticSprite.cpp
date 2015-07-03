#include "OpticSprite.h"
#include "EnginePointers.h"
#include "EngineTypes.h"
#include "DebugHelper.h"
#include <sstream>

namespace Optic {

//@todo - switch to delegating constructor on VS update
OpticSprite::OpticSprite(LPDIRECT3DTEXTURE9 texture) {	
	animate = false;
	init(texture);
}

OpticSprite::OpticSprite(LPDIRECT3DTEXTURE9 texture, OpticAnimation animation)
	: animation(animation), position(), aniState() {
	animate = true;
	init(texture);
}

void OpticSprite::init(LPDIRECT3DTEXTURE9 texture) {
	texture->GetLevelDesc(0, &surfaceDesc); 
	transformCentre = D3DXVECTOR2(surfaceDesc.Width * 0.5f, surfaceDesc.Height * 0.5f);
	translateCentre = D3DXVECTOR2(0.0f, 0.0f);
	this->texture = texture;
	this->spritesheet = false;
	this->texture = texture;
	this->lastTime = 0;
}

void OpticSprite::setProperties(int rows, int columns, int blanks, int fps, bool loop, int loopBegin) {
	this->spritesheet = true;
	this->blanks = blanks;
	this->fps = fps;
	this->loop = loop;
	this->loopBegin = loopBegin;
	this->played = false;
	this->currFrame = 0;
	this->frameTime = 1000.0 / fps;
	this->currRow = 0;

	/*if(!dimensionChecks()) {
		throw OpticSpriteException("Invalid spritesheet dimensions!");
	}*/
	
	this->rows = rows;
	this->columns = columns;
	this->width = surfaceDesc.Width / this->columns;
	this->height = surfaceDesc.Height / this->rows;
	this->frames = (rows * columns) - blanks;
	transformCentre = D3DXVECTOR2(width * 0.5f, height * 0.5f);
}

bool OpticSprite::dimensionChecks() {
	return surfaceDesc.Width % width != 0 || surfaceDesc.Height % height != 0;
}

bool OpticSprite::advanceFrame(double time, int x, int y) {
	double accumulated = time - lastTime;
	
	while(accumulated >= frameTime) {
		currFrame = (currFrame + 1) % frames;

		if(currFrame % columns == 0) {
			currRow = (currRow + 1) % rows;
		}

		if(currFrame == 0) {
			if(loop) {
				currFrame = loopBegin;
			} else {
				return false;
			}
		}
		
		accumulated -= frameTime;
		lastTime += frameTime;
	}

	return true;
}

int OpticSprite::draw(LPD3DXSPRITE sprite, double time, D3DXCOLOR colour, double offsetX, double offsetY) {
	if(!this->animate) {
		sprite->Draw(texture, NULL, NULL, NULL, colour);
		return 1;
	}

	if(animation.lifetime() > time) {
		animation.updateState(aniState, time);
		colour = D3DXCOLOR(aniState.red, aniState.green, aniState.blue, colour.a < aniState.alpha? colour.a : aniState.alpha);
		float rotation = 6.28318531f * aniState.rotation;
		D3DXMATRIX mat, current;
		D3DXVECTOR2 scaling(aniState.scale_x, aniState.scale_y);
		D3DXVECTOR2 position(floor(((pResolution->width * aniState.position_x) - translateCentre.x) + offsetX),
							 floor(((pResolution->height * aniState.position_y) - translateCentre.y) + offsetY));
		D3DXMatrixTransformation2D(&mat, &transformCentre, 0.0f, &scaling, &transformCentre, rotation, &position);
		sprite->GetTransform(&current);
		mat *= current;
		sprite->SetTransform(&mat);
		
		HRESULT res;

		//Spritesheet rect calculations are slightly iffy thanks to the texture scaling (rounding errors)
		if(this->spritesheet) {
			int x = (width * currFrame) % surfaceDesc.Width;
			
			//Hacky workaround
			int diff = x - surfaceDesc.Width;

			if(abs(diff) <= 10) {
				x = 0;
			}
			//End of hacky workaround

			int y = height * currRow;
			RECT source;
			source.top = y;
			source.left = x;
			source.right = x + width;
			source.bottom = y + height;
			res = sprite->Draw(texture, &source, NULL, NULL, colour);
			sprite->SetTransform(&current);

			if(!advanceFrame(time, x, y)) {
				return 0;
			}
		} else {
			res = sprite->Draw(texture, NULL, NULL, NULL, colour);
			sprite->SetTransform(&current);
		}

		if(res != S_OK) {
			throw OpticSpriteException("Rendering sprite failed!");
		}

		return 1;
	}

	return 0;
}

std::pair<int, int> OpticSprite::dimensions() const {
	if(this->spritesheet) {
		return std::make_pair(this->width, this->height);
	}

	return std::make_pair(surfaceDesc.Width, surfaceDesc.Height);
}

void OpticSprite::setTranslationCentre(float x, float y) {
	translateCentre = D3DXVECTOR2(x, y);
}

void OpticSprite::setTransformationCentre(float x, float y) {
	transformCentre = D3DXVECTOR2(x, y);
}

};