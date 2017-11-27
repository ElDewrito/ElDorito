#include "ModuleTime.hpp"
#include <sstream>
#include "../ElDorito.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Server/VariableSynchronization.hpp"

namespace
{
	bool VariableGameSpeedUpdate(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto speed = Modules::ModuleTime::Instance().VarSpeedClient->ValueFloat;
		Pointer gameTimeGlobalsPtr(ElDorito::GetMainTls(GameGlobals::Time::TLSOffset)[0]);
		gameTimeGlobalsPtr(GameGlobals::Time::GameSpeedIndex).Write(speed);

		std::stringstream ss;
		ss << "Game speed set to " << speed;
		returnInfo = ss.str();

		return true;
	}
}

namespace Modules
{
	ModuleTime::ModuleTime() : ModuleBase("Time")
	{
		VarSpeed = AddVariableFloat("GameSpeed", "game_speed", "The game's speed", eCommandFlagsCheat, 1.0f);
		VarSpeed->ValueFloatMin = 0.0f;
		VarSpeed->ValueFloatMax = 10.0f;
		VarSpeedClient = AddVariableFloat("GameSpeedClient", "game_speed_client", "The game's speed", eCommandFlagsInternal, 1.0f, VariableGameSpeedUpdate);
		Server::VariableSynchronization::Synchronize(VarSpeed, VarSpeedClient);
	}
}
