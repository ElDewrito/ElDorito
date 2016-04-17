#include "VersionInfo.hpp"

#define V(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#define V1(v) (((v) >> 24) & 0xFF)
#define V2(v) (((v) >> 16) & 0xFF)
#define V3(v) (((v) >> 8) & 0xFF)
#define V4(v) ((v) & 0xFF)

/******************************************************************************
 * ElDewrito Version Information                                              *
 *                                                                            *
 * DO NOT CHANGE THIS UNLESS YOU KNOW _FOR SURE_ WHAT YOU ARE DOING.          *
 * THE GAME HAS A VERSION CHECK IN IT FOR A REASON.                           *
 *                                                                            *
 * Even seemingly-minor changes can have an effect on how your game           *
 * communicates with other players. You should ONLY be playing with other     *
 * people who are on the EXACT SAME build as you. Otherwise, you may run into *
 * problems ranging from minor gameplay glitches to straight-up crashes. Just *
 * because you don't notice any issues doesn't mean that there aren't any.    *
 *                                                                            *
 * Additionally, BYPASSING THE VERSION CHECK IS CONSIDERED CHEATING.          *
 * You will have access to features that others do not have, even if they do  *
 * not give you an obvious advantage over anyone.                             *
 *                                                                            *
 * If you understand the above and STILL for some insane reason think that    *
 * you need to change this version number,                                    *
 *                                                                            *
 * DO NOT BY ANY MEANS TRY TO DISTRIBUTE YOUR CUSTOM BUILD IN PUBLIC.         *
 *                                                                            *
 * YOU MAY BE _BANNED_ FROM THE SUBREDDIT OR THE IRC CHANNEL FOR DISTRIBUTING *
 * CHEATS AND/OR CONFUSING OTHER USERS.                                       *
 *                                                                            *
 * We have considered making ElDewrito a closed-source mod several times now. *
 * You are more than welcome to test new features for us, but please only do  *
 * so with other people who have access to those features. It's not helpful   *
 * for us to receive bug reports from people who bypassed the version check.  *
 *                                                                            *
 * Don't ruin the fun for everyone else.                                      *
 ******************************************************************************/

#define CURRENT_VERSION V(0, 6, 0, 0)

#ifdef _DEBUG
#define VERSION_SUFFIX "-debug"
#elif !defined(ELDEWRITO_RELEASE)
#define VERSION_SUFFIX "-dev"
#else
#define VERSION_SUFFIX ""
#endif

namespace Utils
{
	namespace Version
	{
		uint32_t GetVersionInt()
		{
			return CURRENT_VERSION;
		}

		std::string GetVersionString()
		{
			auto v = CURRENT_VERSION;
			return
				std::to_string(V1(v)) + "." +
				std::to_string(V2(v)) + "." +
				std::to_string(V3(v)) + "." +
				std::to_string(V4(v)) + VERSION_SUFFIX;
		}
	}
}