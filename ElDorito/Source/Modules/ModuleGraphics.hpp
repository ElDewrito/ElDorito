#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleGraphics : public Utils::Singleton<ModuleGraphics>, public ModuleBase
	{
	public:
		Command* VarSaturation;

		// TODO: possibly refactor into single #RRGGBB command or provide such functionality as a separate all-in-one command
		Command* VarRedHue;
		Command* VarGreenHue;
		Command* VarBlueHue;

		Command* VarBloom;
		Command* VarDepthOfField;
		Command* VarLetterbox;

		Command* VarCustomHUDColorsEnabled;
		Command* VarCustomHUDColorsPrimary;
		Command* VarCustomHUDColorsSecondary;

		Command* VarUIScaling;

		ModuleGraphics();
	};
}