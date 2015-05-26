#pragma once

namespace Patches
{
	namespace Ui
	{
		void EnableCenteredCrosshairPatch(bool enable);

		void ApplyAll();
		void ApplyMapNameFixes();

		void Tick();

		extern bool DialogShow; // todo: put this somewhere better
		extern unsigned int DialogStringId;
		extern int DialogArg1; // todo: figure out a better name for this
		extern int DialogFlags;
		extern unsigned int DialogParentStringId;
	}
}