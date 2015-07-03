#pragma once

#include "OpticAnimation.h"
#include "OpticSprite.h"
#include "Benchmark.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <tuple>

namespace Optic {

const int DEFAULT_END_TIME = 3500;

struct OpticMedalGroup {
	bool immune;
	int group;
	int priority;
};

enum SHIFT_DIRECTION {
	UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8
};

class OpticMedal {
	typedef std::tuple<OpticAnimation, AnimationState, Benchmark, std::pair<int, int>> Slider;
	
	Benchmark timer, endTimer;
	AnimationState aniState, endState;
	OpticAnimation animation;
	SHIFT_DIRECTION direction;
	OpticAnimation slideAnimation, endSlideAnimation;
	D3DXVECTOR2 transformCentre, translateCentre;
	std::pair<int, int> _dimensions;
	std::vector<OpticSprite> sprites;
	bool tickTimer, _ending, animate;
	std::vector<Slider> sliders;
	void init(int x = 0, int y = 0);
	int spawnOffsetX, spawnOffsetY, _endTime;
	OpticAnimation defaultSlideAnimation();
	OpticAnimation defaultEndSlideAnimation(double x, double y);

public:
	OpticMedal();
	OpticMedal(const OpticAnimation& animation);
	OpticMedal(const OpticSprite& sprite);
	OpticMedalGroup pushGroup;
	void addSprite(OpticSprite sprite);
	bool draw(LPD3DXSPRITE sprite);
	void group(OpticMedalGroup group);
	OpticMedalGroup group() const;
	void shiftDirection(SHIFT_DIRECTION direction);
	void setShiftOffset(float x, float y);
	void pushNotify(std::pair<int, int> dimensions);
	void pushInNotify(std::pair<int, int> dimensions);
	void spawnOffset(std::pair<int, int> dimensions);
	std::pair<int, int> dimensions();
	void endTime(int endTime);
	bool ending();
	void setSlideAnimation(const OpticAnimation& animation);
	void setEndSlideAnimation(const OpticAnimation& animation);
	void setTranslationCentre(float x, float y);
	void setTransformationCentre(float x, float y);
};

};