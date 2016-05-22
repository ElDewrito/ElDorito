#include "Gravity.hpp"
#include "../Patch.hpp"
#include "../ElDorito.hpp"

namespace
{
	bool GravityEnabled = true;
}

namespace Patches
{
	namespace Gravity
	{
		void Enable(bool enabled)
		{
			GravityEnabled = enabled;
			if (enabled)
			{
				auto &gravityPtr = ElDorito::GetMainTls(GameGlobals::Physics::TLSOffset)[0];
				if (&gravityPtr)
					gravityPtr.Write(GameGlobals::Physics::DefaultGravity);
			}

			else
			{
				auto &gravityPtr = ElDorito::GetMainTls(GameGlobals::Physics::TLSOffset)[0];
				if (&gravityPtr)
					gravityPtr(GameGlobals::Physics::GravityIndex).Write<float>(0);
			}
		}

		void Tick()
		{
			//OnEvent (Not Needed)
		}
	}
}
