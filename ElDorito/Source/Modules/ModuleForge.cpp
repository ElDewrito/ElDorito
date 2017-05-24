#include "ModuleForge.hpp"

namespace Modules
{
	ModuleForge::ModuleForge() : ModuleBase("Forge")
	{
		VarCloneDepth = AddVariableFloat("CloneDepth", "clone_depth", "Depth at which the object will be cloned", eCommandFlagsNone, 1.0f);
		VarCloneMultiplier = AddVariableInt("CloneMultiplier", "clone_multiplier", "Number of consecutive times the object will be cloned", eCommandFlagsNone, 1);
		VarRotationSnap = AddVariableFloat("RotationSnap", "rotation_snap", "Angle in adegrees at which object rotation will be snapped", eCommandFlagsNone, 0);
	}
}