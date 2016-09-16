#include "Voip.hpp"
#include "VoipServer.hpp"
#include "VoipTokenManager.hpp"
#include "../Patches/Network.hpp"

using namespace Voip;

namespace
{
	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState);
}

namespace Voip
{
	void Initialize()
	{
		TokenManager::Initialize();
		Patches::Network::OnLifeCycleStateChanged(LifeCycleStateChanged);
	}
}

namespace
{
	void LifeCycleStateChanged(Blam::Network::LifeCycleState newState)
	{
		if (newState == Blam::Network::eLifeCycleStateNone || newState == Blam::Network::eLifeCycleStateLeaving)
			Server::Stop(); // Stop the server when we leave a game
		else if (newState != Blam::Network::eLifeCycleStateJoining)
			Voip::TokenManager::RequestToken(); // Request an authentication token after joining a game
	}
}