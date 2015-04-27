#pragma once

#include "OpticPack.h"
#include "OpticAnimation.h"
#include "OpticSprite.h"
#include "OpticMedal.h"
#include "OpticException.h"
#include "OpticPlayback.h"
#include <string>
#include <map>
#include <deque>
#include <d3d9.h>
#include <d3dx9.h>
#include <Windows.h>

namespace Optic {

class OpticRenderException : public OpticException {
public:
	OpticRenderException() : OpticException("An unknown exception occured!") { }
	OpticRenderException(std::string msg) : OpticException(msg) { };
};

class OpticRender {
	LPD3DXSPRITE sprite;
	std::deque<OpticMedal> activeMedals;
	std::map<std::string, OpticMedal> medals;
	std::map<std::string, OpticSprite> sprites;
	std::map<std::string, OpticAnimation> animations;
	std::map<OpticImage, LPDIRECT3DTEXTURE9> textures;
	CRITICAL_SECTION dequeProtect;
	OpticPlayback player;
	bool redirect;

	OpticRender& operator=(const OpticRender&);
	OpticRender(const OpticRender&);

	void queueMedal(OpticMedal& medal);
	LPDIRECT3DTEXTURE9 locateTexture(const OpticImage& image);

public:
	OpticRender();
	~OpticRender();

	void draw();
	void reset();
	void lost();
	void createMedal(const std::string& name);
	void createMedal(const std::string& name, const std::string& animation);
	void createAnimation(const std::string& animation);
	LPDIRECT3DTEXTURE9 createTexture(const OpticImage& image);
	void renderMedal(const std::string& name);
	void renderMedal(const OpticImage& image, const std::string& animation = "default");
	void attachSprite(const std::string& medalName, const std::string& spriteName);
	void addKeyframe(const std::string& name, const Keyframe& keyframe, TRANSFORM transform,
		TWEEN tween);
	void addKeyframe(const std::string& name, const Keyframe& keyframe, TRANSFORM transform,
		const OpticBezier& curve);
	void createSprite(const std::string& spriteName, const OpticImage& image);
	void createSprite(const std::string& spriteName, const OpticImage& image, const std::string& animation);
	void setSpriteProperties(const std::string& name, bool loop, int rows, int columns, int fps,
		int blanks, int loopFrame);
	void setSlideAnimation(const std::string& medalName, const std::string& animationName);
	void setEndTime(const std::string& medal, int endTime);
	bool medalExists(const std::string& name);
	void setMedalProperties(const std::string& name, int group, int priority, bool immune, int endTime);
	void queueAudio(const OpticAudio& audio, bool lowPriority = false);
	void displayText(const std::string& message);
};

};