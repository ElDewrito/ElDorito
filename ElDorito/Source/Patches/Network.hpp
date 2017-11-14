#pragma once

#include <functional>
#include "../Blam/BlamNetwork.hpp"

#define WM_INFOSERVER WM_USER + 1337

namespace Patches::Network
{
	void ApplyAll();
	void ForceDedicated();

	bool StartInfoServer();
	bool StopInfoServer();

	bool IsInfoSocketOpen();

	// Callback for a pong handler function.
	// from - The address the pong was received from
	// timestamp - The timestamp on the original ping (from timeGetTime)
	// id - The ID on the original ping
	// latency - Round-trip time in milliseconds
	typedef std::function<void(const Blam::Network::NetworkAddress &from, uint32_t timestamp, uint16_t id, uint32_t latency)> PongCallback;

	// Registers a function to be called when a pong is received.
	// Returns a unique ID that pings should be sent with.
	uint16_t OnPong(PongCallback callback);

	// Callback for a lifecycle state change handler function.
	// newState - The new lifecycle state.
	typedef std::function<void(Blam::Network::LifeCycleState newState)> LifeCycleStateChangedCallback;

	// Registers a function to be called when the lifecycle state is
	// changed.
	void OnLifeCycleStateChanged(LifeCycleStateChangedCallback callback);

	typedef std::function<void(Blam::MapVariant*)> MapVariantRequestChangeCallback;
	void OnMapVariantRequestChange(MapVariantRequestChangeCallback callback);
}