#include "OpticRender.h"
#include "OpticPack.h"
#include "Direct3D.h"
#include "EnginePointers.h"
#include "EngineTypes.h"
#include <algorithm>
#include <sstream>
#include "DebugHelper.h"
#include <mutex>

std::mutex hacky_fix;

namespace Optic {

OpticRender::OpticRender() {
	this->redirect = true;
	sprite = NULL;
	InitializeCriticalSectionAndSpinCount(&dequeProtect, 1024);
	if(D3DXCreateSprite(D3DHook::pDevice, &sprite) != S_OK) {
		throw OpticRenderException("Direct3D failed to create sprite!");
	}
}

OpticRender::~OpticRender() {
	for(auto i = textures.begin(); i != textures.end(); ++i) {
		(*i).second->Release();
	}
	
	sprite->Release();
	DeleteCriticalSection(&dequeProtect);
}

void OpticRender::reset() {
	sprite->OnResetDevice();
}

void OpticRender::lost() {
	sprite->OnLostDevice();
}

void OpticRender::draw() {
	if(sprite->Begin(D3DXSPRITE_ALPHABLEND) != S_OK) {
		throw OpticRenderException("Direct3D failed to draw sprite!");
	}

	D3DXMATRIX mat, oldMat;
	D3DXMatrixIdentity(&mat);
	mat._12 = -0.050f;

	sprite->GetTransform(&oldMat);
	sprite->SetTransform(&mat);

	EnterCriticalSection(&dequeProtect);

	for(auto i = activeMedals.begin(); i != activeMedals.end();) {
		if(i->draw(sprite)) {
			++i;
		} else {
			i = activeMedals.erase(i);
		}
	}

	LeaveCriticalSection(&dequeProtect);
	
	sprite->SetTransform(&oldMat);

	if(sprite->End() != S_OK) {
		throw OpticRenderException("Direct3D failed to draw sprite!");
	}
}

LPDIRECT3DTEXTURE9 OpticRender::createTexture(const OpticImage& image) {
	LPDIRECT3DTEXTURE9 texture = NULL;
	D3DXIMAGE_INFO info;
	
	if(D3DXGetImageInfoFromFileInMemory(image.second.get(), image.first, &info) != D3D_OK) {
		throw OpticRenderException("Direct3D was unable to get file information");
	}
	
	double scaleFactorX = pResolution->width / 2820.0;
	double scaleFactorY = pResolution->height / 1586.0;
	double scaleFactor = scaleFactorX > scaleFactorY? scaleFactorY : scaleFactorX;
	info.Width = static_cast<UINT>(info.Width * scaleFactor);
	info.Height = static_cast<UINT>(info.Height * scaleFactor);
	
	HRESULT res = D3DXCreateTextureFromFileInMemoryEx(D3DHook::pDevice, image.second.get(), image.first, info.Width, info.Height,
	                                                  D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT,
	                                                  D3DX_DEFAULT, 0, NULL, NULL, &texture);
	if(res != D3D_OK) {
		std::stringstream ss;
		ss << "Direct3D was unable to create texture. Error code: " << res;
		throw OpticRenderException(ss.str());
	}

	textures[image] = texture;
	return texture;
}

void OpticRender::renderMedal(const OpticImage& image, const std::string& animationName) {
	try {
		OpticAnimation animation = animationName == "default"? OpticAnimation::defaultAnimation() :
		                                                       animations.at(animationName);
		LPDIRECT3DTEXTURE9 texture = locateTexture(image);
		OpticMedal medal(OpticSprite(texture, OpticAnimation::defaultAnimation()));
		medal.endTime(DEFAULT_END_TIME);
		queueMedal(medal);
	} catch(std::out_of_range) {
		std::string error("No such animation name defined - " + animationName);
		throw OpticRenderException(error);
	}
}

void OpticRender::renderMedal(const std::string& name) {
	try {
		queueMedal(medals.at(name));
	} catch(std::out_of_range) {
		throw OpticRenderException("Attempted to render a medal that hasn't been defined!");
	}
}

void OpticRender::displayText(const std::string& text) {
	// gone
}

void OpticRender::queueMedal(OpticMedal& medal) {
	EnterCriticalSection(&dequeProtect);
	
	std::for_each(activeMedals.begin(), activeMedals.end(), 
		[&medal](OpticMedal& active) {
			if(medal.group().group == active.group().group && !active.group().immune && !active.ending()) {
				if(medal.group().priority >= active.group().priority) {
					active.pushNotify(medal.dimensions()); 
				} else {
					medal.spawnOffset(active.dimensions());
				}
			}
	});

	activeMedals.emplace_back(medal);
	
	LeaveCriticalSection(&dequeProtect);
}

LPDIRECT3DTEXTURE9 OpticRender::locateTexture(const OpticImage& image) {
	auto textIt = textures.find(image);

	if((textIt == textures.end())) {
		throw std::runtime_error("Unable to find texture - erp?");
	}

	return textIt->second;
}

void OpticRender::attachSprite(const std::string& medalName, const std::string& spriteName) {
	try {
		OpticMedal& medal = medals.at(medalName);
		OpticSprite sprite = sprites.at(spriteName);
		medal.addSprite(sprite);
	} catch(std::out_of_range) {
		throw OpticRenderException("No such sprite and/or medal names defined!");
	}
}

void OpticRender::setSpriteProperties(const std::string& name, bool loop, int rows,
                                      int columns, int fps, int blanks, int loopBegin) {
	try {
		OpticSprite& sprite = sprites.at(name);
		sprite.setProperties(rows, columns, blanks, fps, loop, loopBegin);
	} catch(std::out_of_range) {
		std::string error("No such sprite name defined - " + name);
		throw OpticRenderException(error);
	}
}

void OpticRender::createMedal(const std::string& name) {
	medals.insert(std::pair<std::string, OpticMedal>(name, OpticMedal()));
}

void OpticRender::createMedal(const std::string& name, const std::string& animationName) {
	try {
		OpticAnimation animation = (animationName == "default")? OpticAnimation::defaultAnimation() : animations.at(animationName);
		OpticMedal medal(animation);
		medals.insert(std::pair<std::string, OpticMedal>(name, medal));
	} catch(std::out_of_range) {
		std::string error("No such animation name defined - " + animationName);
		throw OpticRenderException(error);
	}
}

void OpticRender::createSprite(const std::string& name, const OpticImage& image,
                               const std::string& animationName) {
	try {
		OpticAnimation animation = (animationName == "default")? 
									OpticAnimation::defaultAnimation() : animations.at(animationName);
		LPDIRECT3DTEXTURE9 texture = locateTexture(image);
		sprites.insert(std::pair<std::string, OpticSprite>(name, OpticSprite(texture, animation)));
	} catch(std::out_of_range) {
		std::string error("No such animation name defined - " + animationName);
		throw OpticRenderException(error);
	}
}

void OpticRender::createSprite(const std::string& name, const OpticImage& image) {
	LPDIRECT3DTEXTURE9 texture = locateTexture(image);
	sprites.insert(std::pair<std::string, OpticSprite>(name, OpticSprite(texture)));
}

void OpticRender::addKeyframe(const std::string& name, const Keyframe& keyframe,
                              TRANSFORM transform, TWEEN tween) {
	try {
		OpticAnimation& animation = animations.at(name);
		animation.addKeyframe(keyframe, transform, tween);
	} catch(std::out_of_range) {
		std::string error("No such animation name defined - " + name);
		throw OpticRenderException(error);
	}
}

void OpticRender::addKeyframe(const std::string& name, const Keyframe& keyframe, TRANSFORM transform,
                              const OpticBezier& bezier) {
	try {
		OpticAnimation& animation = animations.at(name);
		animation.addKeyframe(keyframe, transform, bezier);
	} catch(std::out_of_range) {
		std::string error("No such animation name defined - " + name);
		throw OpticRenderException(error);
	}
}

bool OpticRender::medalExists(const std::string& name) {
	auto medalIt = medals.find(name);
	return medalIt != medals.end();
}

void OpticRender::createAnimation(const std::string& name) {
	if(name == "default") {
		throw OpticRenderException("Cannot override default animation");
	}

	animations.insert(std::pair<std::string, OpticAnimation>(name, OpticAnimation()));
}

void OpticRender::setSlideAnimation(const std::string& medalName, const std::string& animationName) {
	try {
		medals.at(medalName).setSlideAnimation(animations.at(animationName));
	} catch(std::out_of_range) {
		throw OpticRenderException("No such medal or animation defined.");
	}
}

void OpticRender::setEndTime(const std::string& name, int endTime) {
	try {
		medals.at(name).endTime(endTime);
	} catch(std::out_of_range) {
		std::string error("No such medal defined - " + name);
		throw OpticRenderException(error);
	}
}

void OpticRender::setMedalProperties(const std::string& name, int group, int priority,
                                    bool immune, int endTime) try {
	OpticMedal& medal = medals.at(name);
	medal.pushGroup.group = group;
	medal.pushGroup.immune = immune;
	medal.pushGroup.priority = priority;
	medal.endTime(endTime);
} catch(std::out_of_range) {
	std::string error("No such medal defined - " + name);
	throw OpticRenderException(error);
}

void OpticRender::queueAudio(const OpticAudio& audio, bool lowPriority) {
	if(lowPriority) {
		if(player.idle()) {
			player.play(audio);
		}
	} else {
		player.play(audio);
	}
}

};