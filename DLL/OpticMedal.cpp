#include "OpticMedal.h"
#include "EnginePointers.h"
#include "EngineTypes.h"
#include "DebugHelper.h"
#include <sstream>

namespace Optic {

//@todo - switch to delegating constructor on VS update
OpticMedal::OpticMedal(const OpticAnimation& animation) : animation(animation), aniState() {
	this->animate = true;
	init();
}

OpticMedal::OpticMedal(const OpticSprite& sprite) {
	sprites.emplace_back(sprite);
	this->animate = false;
	init(sprite.dimensions().first, sprite.dimensions().second); //@todo typedef dimensions
}


OpticMedal::OpticMedal() {
	this->animate = false;
	init();
}

void OpticMedal::init(int x, int y) {
	tickTimer = false;
	_dimensions = std::make_pair(x, y);
	this->slideAnimation = this->defaultSlideAnimation();
	this->translateCentre = D3DXVECTOR2(0.0f, 0.0f);
	direction = RIGHT;
	pushGroup = OpticMedalGroup();
	spawnOffsetX = 0;
	spawnOffsetY = 0;
	_ending = false;
	_endTime = 0;
}

void OpticMedal::addSprite(OpticSprite sprite) {
	sprites.emplace_back(sprite);
	
	auto& dimensions = sprite.dimensions();

	if(dimensions.first > _dimensions.first) {
		_dimensions.first = sprite.dimensions().first;
	}

	if(dimensions.second > _dimensions.second) {
		_dimensions.second = dimensions.second;
	}

	this->transformCentre = D3DXVECTOR2(_dimensions.first * 0.5f, _dimensions.second * 0.5f);
}

bool OpticMedal::draw(LPD3DXSPRITE sprite) {
	double time;

	if(tickTimer) {
		time = timer.current();
	} else {
		tickTimer = true;
		timer.start();
		time = 0;	
	}
	
	D3DXMATRIX aniMat, currMat;
	sprite->GetTransform(&currMat);
	D3DXMatrixIdentity(&aniMat);

	D3DXCOLOR colour = -1;

	if(this->animate) {
		if(this->animation.lifetime() > time) {
			animation.updateState(aniState, time);
			colour = D3DXCOLOR(aniState.red, aniState.green, aniState.blue, aniState.alpha);
			float rotation = 6.28318531f * aniState.rotation;
			D3DXVECTOR2 scaling(aniState.scale_x, aniState.scale_y);
			D3DXVECTOR2 position(floor(((pResolution->width * aniState.position_x) - translateCentre.x)),
							   	 floor(((pResolution->height * aniState.position_y) - translateCentre.y)));
			D3DXMatrixTransformation2D(&aniMat, &transformCentre, 0.0f, &scaling, &transformCentre, rotation, &position);
		} else {
			return false;
		}
	}

	aniMat *= currMat;

	double offset_x = spawnOffsetX;
	double offset_y = spawnOffsetY;

	for(auto i = sliders.begin(); i != sliders.end(); ++i) {
		OpticAnimation& animation = std::get<0>(*i);
		AnimationState& state = std::get<1>(*i);
		double time = std::get<2>(*i).current();
		animation.updateState(state, time);
		offset_x += this->_dimensions.first  * state.position_x;
		offset_y += this->_dimensions.second * state.position_y;
	}

	if(_endTime > 0 && time > _endTime) {
		if(!_ending) {
			endSlideAnimation = defaultEndSlideAnimation(offset_x, offset_y);
			_ending = true;
			endTimer.start();
		}

		endSlideAnimation.updateState(endState, endTimer.current());
		offset_x = endState.position_x;
		offset_y = endState.position_y;
	}
	
	sprite->SetTransform(&aniMat);

	int keepDrawing = 0;

	/* Faster to erase all of the sprites at once rather than one
	   by one when they've finished drawing */
	for(auto i = sprites.begin(); i != sprites.end(); ++i) {
		keepDrawing |= i->draw(sprite, time, colour, offset_x, offset_y);
	}

	sprite->SetTransform(&currMat);

	return keepDrawing == 1;
}

OpticMedalGroup OpticMedal::group() const {
	return this->pushGroup;
}

void OpticMedal::group(OpticMedalGroup newGroup) {
	this->pushGroup = newGroup;
}

void OpticMedal::pushNotify(std::pair<int, int> dimensions) {
	Benchmark timer;
	timer.start();
	sliders.emplace_back(std::make_tuple(slideAnimation, AnimationState(), timer, dimensions)); 
}

void OpticMedal::spawnOffset(std::pair<int, int> dimensions) {
	if(direction & LEFT) {
		spawnOffsetX -= dimensions.first;
	} else if(direction & RIGHT) {
		spawnOffsetX += dimensions.first;
	}

	if(direction & UP) {
		spawnOffsetY -= dimensions.second;
	} else if(direction & DOWN) {
		spawnOffsetY += dimensions.second;
	}
}

void OpticMedal::shiftDirection(SHIFT_DIRECTION direction) {
	this->direction = direction;
}

OpticAnimation OpticMedal::defaultSlideAnimation() {
	OpticAnimation animation;
	animation.addKeyframe(Keyframe(0, 0.0f), KEYFRAME_POSITION_X);
	animation.addKeyframe(Keyframe(150, 1.0f), KEYFRAME_POSITION_X);
	return animation;
}

OpticAnimation OpticMedal::defaultEndSlideAnimation(double beginX, double beginY) {
	OpticAnimation animation;
	return animation;
}

std::pair<int, int> OpticMedal::dimensions() {
	return this->_dimensions;
}

void OpticMedal::endTime(int endTime) {
	this->_endTime = endTime;
}

bool OpticMedal::ending() {
	return this->_ending;
}

void OpticMedal::setEndSlideAnimation(const OpticAnimation& animation) {
	this->endSlideAnimation = animation;
}

void OpticMedal::setSlideAnimation(const OpticAnimation& animation) {
	this->slideAnimation = animation;
}

void OpticMedal::setTranslationCentre(float x, float y) {
	translateCentre = D3DXVECTOR2(x, y);
}

void OpticMedal::setTransformationCentre(float x, float y) {
	transformCentre = D3DXVECTOR2(x, y);
}

};