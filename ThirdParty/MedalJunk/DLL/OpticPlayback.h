#pragma once

#define LIB_ZPLAY_STATIC

#include "libzplay.h"
#include "OpticPack.h"
#include <concurrent_queue.h>
#include <memory>

namespace Optic {

class OpticPlayback {
	libZPlay::ZPlay* player;
	bool playing;

	OpticPlayback& operator=(const OpticPlayback&);
	OpticPlayback(const OpticPlayback&);

public:
	bool idle();
	OpticPlayback();
	~OpticPlayback();
	void play(OpticAudio audio);
};


};