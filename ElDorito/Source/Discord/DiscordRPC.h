#include <discord-rpc.h>
#include "../Utils/Singleton.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Blam/BlamNetwork.hpp"

namespace Discord
{
	class DiscordRPC : public Utils::Singleton<DiscordRPC>
	{
	public:
		DiscordRichPresence discordPresence;

		void Update();
		void UpdatePresence();
		void ReplyToJoinRequest(const char* userId, int reply);
		DiscordRPC();
	};
}
