#include "OpticPlayback.h"
#include "EnginePointers.h"
#include <string>

namespace Optic {

OpticPlayback::OpticPlayback() {
	player = libZPlay::CreateZPlay();
	playing = false;
}

OpticPlayback::~OpticPlayback() {
	player->Release();
}

void OpticPlayback::play(OpticAudio audio) {
	player->SetPlayerVolume(*master_volume, *master_volume);
	
	if(!playing) {
		player->OpenStream(0, 1, audio.second.get(), audio.first, libZPlay::sfMp3);
		player->Play();
		playing = true;
	} else {
		player->PushDataToStream(audio.second.get(), audio.first);
	}
}

bool OpticPlayback::idle() {
	libZPlay::TStreamLoadInfo load;
	player->GetDynamicStreamLoad(&load);
	return load.NumberOfBytes == 0;
}

};