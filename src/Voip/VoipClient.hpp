#pragma once

#include "Voip.hpp"

namespace Voip
{
	enum class VoiceMode
	{
		PushToTalk, // The player must hold down a key to talk
		Activation  // Talking is automatically activated based on voice level
	};

	namespace Client
	{
		// Connects to a VoIP server using an authentication token.
		void Connect(const char *ip, AuthToken token);

		// Disconnects from the server.
		void Disconnect();

		// Returns true if a server connection is open.
		bool IsConnected();

		// Requests to kick a peer from the server.
		// Only available for hosts. Returns true if successful.
		bool KickPeer(int peerIndex);

		// Mutes a player locally. Returns true if successful.
		bool MutePeer(const char *name);

		// Sets the voice mode to use to determine when the player is talking.
		void SetVoiceMode(VoiceMode mode);

		// If the voice mode is set to PushToTalk, sets whether or not the
		// push-to-talk key is held down.
		bool SetTalking(bool talk);

		// If the voice mode is set to Activation, sets the voice activation level.
		void SetVoiceActivationLevel(float level);

		// Sets the volume of other players.
		void SetVolume(float volume);

		// Enables or disables automatic gain control.
		void SetAutomaticGainControl(bool enabled);

		// Enables or disables echo cancellation.
		void SetEchoCancellation(bool enabled);
	}
}