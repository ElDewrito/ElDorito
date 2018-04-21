#include <discord-rpc.h>
#include "../Utils/Singleton.hpp"
#include <string>
#include <mutex>
namespace Discord
{
	class DiscordRPC : public Utils::Singleton<DiscordRPC>
	{
	public:
		DiscordRichPresence discordPresence;
		std::string detailString;
		std::string mapString;
		std::string gameTypeString;
		std::string matchSecret; //random for now

		std::mutex joinMtx;
		std::string joinString;
		void SetJoinString(std::string join);
		
		void Update();
		void UpdatePresence();
		void UpdatePresence(int networkMode);
		void ReplyToJoinRequest(const char* userId, int reply);
		void Shutdown();
		DiscordRPC();
	private:
		time_t lastUpdate = 0;
		const time_t discordUpdateTime = 20;
	};
}
