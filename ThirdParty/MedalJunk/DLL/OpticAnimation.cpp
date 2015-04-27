#include "OpticAnimation.h"
#include "DebugHelper.h"
#include  <sstream>

namespace Optic {

OpticAnimation::OpticAnimation() {
	animationTime = 0;
}

void OpticAnimation::addKeyframe(const Keyframe& frame, TRANSFORM type, const OpticBezier& bezier) {
	if(!validTime(frame.first, type)) {
		throw OpticAnimationException("Attempted to add a keyframe with an invalid time index (must be > last index)!");
	}

	Points points = generatePoints(bezier);

	switch(type) {
		case KEYFRAME_SCALE_X:
			scale_y.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_SCALE_Y:
			scale_y.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_SCALE:
			scale_x.emplace_back(make_pair(frame, points));
			scale_y.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_ROTATION:
			rotation.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_OPACITY:
			alpha.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_POSITION_X:
			position_x.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_POSITION_Y:
			position_y.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_RED:
			red.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_GREEN:
			green.emplace_back(make_pair(frame, points));
			break;
		case KEYFRAME_BLUE:
			blue.emplace_back(make_pair(frame, points));
			break;
		default:
			throw OpticAnimationException("Invalid animation attribute provided!");
	}

	lastFrameTimes[type] = frame.first;
	
	if(frame.first > animationTime) {
		animationTime = frame.first;
	}
}

int OpticAnimation::lifetime() {
	return animationTime;
}

void OpticAnimation::addKeyframe(const Keyframe& frame, TRANSFORM type, TWEEN tween) {
	switch(tween) {
		case LINEAR:
			addKeyframe(frame, type, OpticBezier(0.0f, 0.0f, 1.0f, 1.0f));
			break;
		case EASE_IN:
			addKeyframe(frame, type, OpticBezier(0.4f, 0.0f, 1.0f, 1.0f));
			break;
		case EASE_OUT:
			addKeyframe(frame, type, OpticBezier(0.0f, 0.0f, 0.6f, 1.0f));
			break;
		case EASE_IN_OUT:
			addKeyframe(frame, type, OpticBezier(0.4f, 0.0f, 0.6f, 1.0f));
			break;
		default:
			throw OpticAnimationException("Invalid animation tween provided!");
	}
}

Points OpticAnimation::generatePoints(OpticBezier bezier) {
	Points points;

	for(double i = 0; i < 1.0; i += 0.002) {
		points.emplace_back(bezier.get(i));
	}
	
	return points;
}

void OpticAnimation::updateState(AnimationState& state, double time) {
	state.alpha = calculateState(alpha, time, state.alpha);
	state.scale_x = calculateState(scale_x, time, state.scale_x);
	state.scale_y = calculateState(scale_y, time, state.scale_y);
	state.position_x = calculateState(position_x, time, state.position_x);
	state.position_y = calculateState(position_y, time, state.position_y);
	state.rotation = calculateState(rotation, time, state.rotation);
	state.red = calculateState(red, time, state.red);
	state.green = calculateState(green, time, state.green);
	state.blue = calculateState(blue, time, state.blue);
}


double OpticAnimation::lerp(double y1, double y2, double t) {
	return y1 + ((y2 - y1) / 100.0) * t;
}

double OpticAnimation::percentage(double y1, double y2, double t) {
	return (t - y1) / (y2 - y1);
}

float OpticAnimation::calculateState(Keyframes& frames, double time, float previous) {
	if(frames.empty()) {
		return previous;
	}

	std::pair<Keyframe, Points>* start = &frames.front();

	/* The < 2? checks here are to handle the case where only a single keyframe was used
	   to set the state for the entire duration of the animation. An alternative would
	   be to add a dummy keyframe at the end. */
	std::pair<Keyframe, Points>* finish = frames.size() < 2? start : &frames[1];
	Keyframe finishKey(frames.size() < 2? Keyframe(animationTime, finish->first.second) 
		: Keyframe(frames[1].first.first, frames[1].first.second));

	double progressX = percentage(start->first.first, finishKey.first, time);

	if(progressX > 1.0) {
		if(frames.size() == 1) {
			previous = static_cast<float>(frames.back().second.back());
		}
		frames.pop_front();
		return calculateState(frames, time, previous);
	}

	//Select best index for the precalculated curve points
	double ideal = start->second.size() * progressX;
	double mod = floor(ideal);
	double remainder = fmod(ideal, mod < 1.0? 1.0 : mod);
	int first = static_cast<int>(floor(ideal));
	
	double progressY = lerp(start->second[first], start->second[first + 1], remainder);

	//Map the current state (0 - 1) to the keyframe range (any two min/max between 0 and 1)
	float final = static_cast<float>(start->first.second + progressY * (finishKey.second - start->first.second));

	return final;
}

bool OpticAnimation::validTime(int time, TRANSFORM type) {
	return time >= lastFrameTimes[type];
}

OpticAnimation OpticAnimation::defaultAnimation() { //@todo move to different class
	OpticAnimation animation;
	animation.addKeyframe(Keyframe(0, 0.065f), KEYFRAME_POSITION_X);
	animation.addKeyframe(Keyframe(0, 0.62f), KEYFRAME_POSITION_Y);
	animation.addKeyframe(Keyframe(0, 0.0f), KEYFRAME_OPACITY);
	animation.addKeyframe(Keyframe(300, 1.0f), KEYFRAME_OPACITY);
	animation.addKeyframe(Keyframe(3500, 1.0f), KEYFRAME_OPACITY);
	animation.addKeyframe(Keyframe(3700, 0.0f), KEYFRAME_OPACITY);
	animation.addKeyframe(Keyframe(0, 0.0f), KEYFRAME_SCALE);
	animation.addKeyframe(Keyframe(300, 1.0f), KEYFRAME_SCALE);
	animation.addKeyframe(Keyframe(465, 1.0f), KEYFRAME_SCALE);
	animation.addKeyframe(Keyframe(565, 1.5f), KEYFRAME_SCALE, EASE_IN);
	animation.addKeyframe(Keyframe(665, 1.0f), KEYFRAME_SCALE);
	animation.addKeyframe(Keyframe(0, 0.3f), KEYFRAME_ROTATION);
	animation.addKeyframe(Keyframe(300, 0.0f), KEYFRAME_ROTATION);
	return animation;
}

};