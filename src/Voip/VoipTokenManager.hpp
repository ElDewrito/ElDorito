#pragma once

#include "Voip.hpp"

namespace Voip
{
	namespace TokenManager
	{
		// Initializes the token manager subsystem.
		void Initialize();

		// Sends the host a request for a new authentication token.
		// If called from the host, an authentication token will be generated
		// and made available immediately.
		void RequestToken();

		// Gets the currently-available client token.
		// If a token is available, this function will return true and
		// subsequent attempts to retrieve a token will fail until another
		// token is requeted from the server. This function will return false
		// if no token is available.
		bool GetAvailableToken(AuthToken *dest);

		// Checks an authentication token for validity.
		// If the token is valid, it will be invalidated and the peer index
		// corresponding to the token will be returned. Subsequent calls using
		// the same token will fail. This function will return -1 if a token is
		// not valid.
		int CheckToken(const AuthToken token);

		// Invalidates a peer's token.
		// After calling this, any attempts to check the peer's token with
		// CheckToken will fail until a new token is generated for the peer.
		void InvalidateToken(int peerIndex);
	}
}
