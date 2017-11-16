#include <discord-rpc.h>
#include "../Utils/Singleton.hpp"
#include <string>
namespace Discord
{
	class DiscordRPC : public Utils::Singleton<DiscordRPC>
	{
	public:
		DiscordRichPresence discordPresence;
		std::string detailString;

		void Update();
		void UpdatePresence();
		void ReplyToJoinRequest(const char* userId, int reply);
		DiscordRPC();
	};
}
