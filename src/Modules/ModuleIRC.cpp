#include "ModuleIRC.hpp"
#include <sstream>
#include "../ElDorito.hpp"

namespace Modules
{
	ModuleIRC::ModuleIRC() : ModuleBase("IRC")
	{
		VarIRCServer = AddVariableString("Server", "irc_server", "The IRC server for the global and game chats", eCommandFlagsArchived, "irc.snoonet.org");

		VarIRCServerPort = AddVariableInt("ServerPort", "irc_serverport", "The IRC server port", eCommandFlagsArchived, 6667);
		VarIRCServerPort->ValueIntMin = 1;
		VarIRCServerPort->ValueIntMax = 0xFFFF;
	}
}
