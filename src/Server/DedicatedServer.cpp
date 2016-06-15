#include "DedicatedServer.hpp"
#include "../Blam/BlamEvents.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Patches/Events.hpp"
#include "../Modules/ModuleServer.hpp"

namespace Server
{
	namespace DedicatedServer
	{
		//We are waiting a couple of seconds after calling Lobbytype before we call Server.Mode.
		time_t setModeStartTime = 0;

		//If we send stats right when the game ends, some of the team scores arent updated yet. 
		//If we wait for the submit-stats lifecycle state to fire, some of the scores are already reset to 0. 
		time_t sendStatsTime = 0;

		//leaving actual stats sending code out for now
		DWORD WINAPI CommandServerAnnounceStats_Thread(LPVOID lpParam)
		{
			return true;
		}

		void OnEvent(Blam::DatumIndex player, const Blam::Events::Event *event, const Blam::Events::EventDefinition *definition)
		{

			if (event->NameStringId == 262221) //Game Ended event
			{
				time(&sendStatsTime);

			}
		}
		
	}
}


namespace Server
{
	namespace DedicatedServer
	{

		void Init()
		{
			Patches::Events::OnEvent(OnEvent);
			if (Modules::ModuleServer::Instance().VarServerAutoHost->ValueInt == 1)
			{
				Modules::CommandMap::Instance().ExecuteCommand("Server.Lobbytype 2");
				time(&setModeStartTime);
			}
				

		}
		void Tick()
		{
			auto* session = Blam::Network::GetActiveSession();
			if (!session || !session->IsEstablished() || !session->IsHost())
				return;

			time_t curTime1;
			time(&curTime1);

			if (setModeStartTime != 0)
			{
				auto elapsed = curTime1 - setModeStartTime;
				if (elapsed > 2)
				{
					Modules::CommandMap::Instance().ExecuteCommand("Server.Mode 3");
					setModeStartTime = 0;
				}
			}

			if (sendStatsTime != 0)
			{

				//If we send stats right when the game ends, some of the team scores arent updated yet. 
				//If we wait for the submit-stats lifecycle state to fire, some of the scores are already reset to 0. 
				auto elapsed = curTime1 - sendStatsTime;
				if (elapsed > 1)
				{
					//uncommenting for now.
					//auto thread = CreateThread(NULL, 0, CommandServerAnnounceStats_Thread, (LPVOID)"", 0, NULL);
					sendStatsTime = 0;

				}

			}
		}
	}
}