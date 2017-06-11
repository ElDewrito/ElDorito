#include "ModuleForge.hpp"
#include "../Patches/Forge.hpp"
#include "boost\filesystem.hpp"

namespace
{
	const char PREFAB_DIR[] = "mods/prefabs/";
	const char PREFAB_EXT[] = ".prefab";

	bool CommandDeleteAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Forge::DeleteAll();
		return true;
	}

	bool CommandCanvas(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Forge::CanvasMap();
		return true;
	}

	bool CommandDeselectAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Forge::DeselectAll();
		return true;
	}

	bool CommandSelectAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Patches::Forge::SelectAll();
		return true;
	}

	bool CommandSavePrefab(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.empty())
		{
			returnInfo = "expected prefab name";
			return false;
		}

		auto& name = Arguments[0];

		boost::filesystem::path prefabDirectory(PREFAB_DIR);
		boost::filesystem::create_directories(prefabDirectory);

		auto p = (prefabDirectory / name).replace_extension(PREFAB_EXT);
		if (boost::filesystem::exists(p))
		{
			returnInfo = "prefab with that name already exists";
			return false;
		}

		if (!Patches::Forge::SavePrefab(name, p.string()))
		{
			returnInfo = "failed to save prefab";
			return false;
		}

		returnInfo = "prefab saved";
		return true;
	}

	bool CommandLoadPrefab(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.empty())
		{
			returnInfo = "expected prefab name";
			return false;
		}

		boost::filesystem::path prefabDirectory(PREFAB_DIR);

		auto p = (prefabDirectory / Arguments[0]).replace_extension(PREFAB_EXT);
		if (!boost::filesystem::exists(p))
		{
			returnInfo = "prefab with that name does not exist";
			return false;
		}

		if (!Patches::Forge::LoadPrefab(p.string()))
		{
			returnInfo = "failed to load prefab";
			return false;
		}

		returnInfo = "prefab loaded";
		return true;
	}
}

namespace Modules
{
	ModuleForge::ModuleForge() : ModuleBase("Forge")
	{
		VarCloneDepth = AddVariableFloat("CloneDepth", "clone_depth", "Depth at which the object will be cloned", eCommandFlagsNone, 1.0f);
		VarCloneMultiplier = AddVariableInt("CloneMultiplier", "clone_multiplier", "Number of consecutive times the object will be cloned", eCommandFlagsNone, 1);
		VarRotationSnap = AddVariableFloat("RotationSnap", "rotation_snap", "Angle in degrees at which object rotation will be snapped", eCommandFlagsNone, 0);
		VarRotationSensitivity = AddVariableFloat("RotationSensitivity", "rotation_sensitivity", "Controls the sensitivity of object rotation", eCommandFlagsArchived, 1.0f);
		VarMonitorSpeed = AddVariableFloat("MonitorSpeed", "monitor_speed", "Controls the movement speed of the monitor", eCommandFlagsArchived, 1.0f);

		AddCommand("DeleteAll", "delete_all", "Delete all objects that are the same as the object under the crosshair", eCommandFlagsHostOnly, CommandDeleteAll);
		AddCommand("Canvas", "canvas", "Delete all objects on the map", eCommandFlagsHostOnly, CommandCanvas);
		AddCommand("SelectAll", "select_all", "Select all objects that are the same as the object under the crosshair", eCommandFlagsNone, CommandSelectAll);
		AddCommand("DeselectAll", "deselect_all", "Deselect all selected objects", eCommandFlagsNone, CommandDeselectAll);
		AddCommand("SavePrefab", "prefab_save", "Save prefab to a file", eCommandFlagsNone, CommandSavePrefab);
		AddCommand("LoadPrefab", "prefab_load", "Load prefab from a file", eCommandFlagsNone, CommandLoadPrefab);
	}
}