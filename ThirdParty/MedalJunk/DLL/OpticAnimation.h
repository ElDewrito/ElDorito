#pragma once

#include "OpticException.h"
#include "OpticBezier.h"
#include <deque>
#include <vector>
#include <map>
#include <tuple>

namespace Optic {

class OpticAnimationException : public OpticException {
public:
	OpticAnimationException() : OpticException("An unknown exception occured!") { }
	OpticAnimationException(std::string msg) : OpticException(msg) { };
};

typedef std::pair<short, float> Keyframe;
typedef std::vector<double> Points;
typedef std::deque<std::pair<Keyframe, Points>> Keyframes;

enum TRANSFORM {
	KEYFRAME_SCALE,
	KEYFRAME_SCALE_X,
	KEYFRAME_SCALE_Y,
	KEYFRAME_ROTATION,
	KEYFRAME_OPACITY,
	KEYFRAME_RED,
	KEYFRAME_GREEN,
	KEYFRAME_BLUE,
	KEYFRAME_POSITION_X,
	KEYFRAME_POSITION_Y
};

enum TWEEN {
	LINEAR,
	EASE_IN,
	EASE_OUT,
	EASE_IN_OUT
};

struct AnimationState {
	AnimationState() {
		this->alpha = 1.0f;
		this->scale_x = 1.0f;
		this->scale_y = 1.0f;
		this->red = 1.0f;
		this->blue = 1.0f;
		this->green = 1.0f;
		this->position_x = 0.0f;
		this->position_y = 0.0f;
		this->rotation = 0.0f;
	}
	float alpha;
	float scale_x;
	float scale_y;
	float rotation;
	float position_x;
	float position_y;
	float red;
	float green;
	float blue;
};

class OpticAnimation {
	int animationTime;
	std::map<TRANSFORM, int> lastFrameTimes;
	Keyframes scale_x, scale_y, position_x, position_y, alpha, rotation, red, green, blue;

	bool validTime(int time, TRANSFORM type);
	Points generatePoints(OpticBezier bezier);
	double lerp(double y1, double y2, double x);
	double percentage(double y1, double y2, double x);
	float calculateState(Keyframes& frames, double time, float previous);

public:
	int lifetime();
	OpticAnimation();
	void updateState(AnimationState& state, double time);
	void addKeyframe(const Keyframe& frame, TRANSFORM type, TWEEN tween = LINEAR);
	void addKeyframe(const Keyframe& frame, TRANSFORM type, const OpticBezier& bezier);
	static OpticAnimation defaultAnimation();
};

};