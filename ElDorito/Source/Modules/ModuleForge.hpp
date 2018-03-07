#pragma once

#include "ModuleBase.hpp"

namespace Modules
{
	class ModuleForge : public Utils::Singleton<ModuleForge>, public ModuleBase
	{
	public:
		Command* VarCloneDepth;
		Command* VarCloneMultiplier;
		Command* VarRotationSnap;
		Command* VarRotationSensitivity;
		Command* VarMonitorSpeed;
		Command* VarMonitorNoclip;
		Command* VarSelectionRenderer;
		Command* VarMagnetsEnabled;
		Command* VarMagnetsVisible;
		Command* VarMaxGrabDistance;
		Command* VarMagnetsStrength;
		Command* VarShowInvisibles;

		struct 
		{
			bool MapCanvas;
			bool RuntimeReset;
			bool SetPrematchCamera;
			bool OpenMapOptions;
			uint32_t SpawnItem;
			struct
			{
				bool SavePrefab;
				bool LoadPrefab;
				std::string Path;
				std::string Name;
			} Prefabs;

		} CommandState = { 0 };

		ModuleForge();
	};
}
