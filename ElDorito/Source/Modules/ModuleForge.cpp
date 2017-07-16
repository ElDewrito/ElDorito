#include <locale>
#include <codecvt>
#include "ModuleForge.hpp"
#include "../Patches/Forge.hpp"
#include "boost\filesystem.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Forge/Selection.hpp"
#include "../Forge/ForgeUtil.hpp"

namespace
{
	const char PREFAB_DIR[] = "mods/prefabs/";
	const char PREFAB_EXT[] = ".prefab";

	bool CommandDeleteAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::Delete();
		return true;
	}

	bool CommandCanvas(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::CanvasMap();
		return true;
	}

	bool CommandDeselectAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::Clear();
		return true;
	}

	bool CommandSelectAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::SelectAll();
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

	bool CommandDumpPrefabs(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		boost::filesystem::path p(PREFAB_DIR);
		boost::filesystem::directory_iterator end_itr;

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartArray();

		if (boost::filesystem::exists(p))
		{	
			for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
			{
				const auto& path = itr->path();
				const auto& ext = path.extension().string();
				const auto& name = path.stem().string();

				if (boost::filesystem::is_regular_file(path) && ext == ".prefab")
					writer.String(name.c_str());
			}
		}

		writer.EndArray();

		returnInfo = buffer.GetString();
		return true;
	}
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> string_converter;
	bool CommandSetName(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.empty())
		{
			returnInfo = "expected varient name";
			return false;
		}

		std::wstring name = string_converter.from_bytes(Arguments[0]);
		Patches::Forge::ForgeVariant_Set_Name(name);
		returnInfo = "Varient name set to " + Arguments[0];
		return true;
	}

	bool CommandSetDescription(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.empty())
		{
			returnInfo = "expected varient description";
			return false;
		}

		std::string description = Arguments[0];
		Patches::Forge::ForgeVariant_Set_Description(description);
		returnInfo = "Varient description set to " + description;
		return true;
	}
}

namespace Modules
{
	ModuleForge::ModuleForge() : ModuleBase("Forge")
	{
		VarCloneDepth = AddVariableFloat("CloneDepth", "forge_clone_depth", "Depth at which the object will be cloned", eCommandFlagsNone, 1.0f);
		VarCloneMultiplier = AddVariableInt("CloneMultiplier", "forge_clone_multiplier", "Number of consecutive times the object will be cloned", eCommandFlagsNone, 1);
		VarRotationSnap = AddVariableFloat("RotationSnap", "forge_rotation_snap", "Angle in degrees at which object rotation will be snapped", eCommandFlagsNone, 0);
		VarRotationSensitivity = AddVariableFloat("RotationSensitivity", "forge_rotation_sensitivity", "Controls the sensitivity of object rotation", eCommandFlagsArchived, 1.0f);
		VarMonitorSpeed = AddVariableFloat("MonitorSpeed", "forge_monitor_speed", "Controls the movement speed of the monitor", eCommandFlagsArchived, 1.0f);
		VarSelectionRenderer = AddVariableInt("SelectionRenderer", "forge_selection_renderer", "Set the selection renderer to use", eCommandFlagsArchived, 0);
		VarSelectionRenderer->ValueIntMin = 0;
		VarSelectionRenderer->ValueIntMax = 1;

		AddCommand("DeleteAll", "forge_delete_all", "Delete all objects that are the same as the object under the crosshair", eCommandFlagsHostOnly, CommandDeleteAll);
		AddCommand("Canvas", "forge_canvas", "Delete all objects on the map", eCommandFlagsHostOnly, CommandCanvas);
		AddCommand("SelectAll", "forge_select_all", "Select all objects that are the same as the object under the crosshair", eCommandFlagsNone, CommandSelectAll);
		AddCommand("DeselectAll", "forge_deselect_all", "Deselect all selected objects", eCommandFlagsNone, CommandDeselectAll);
		AddCommand("SavePrefab", "forge_prefab_save", "Save prefab to a file", eCommandFlagsNone, CommandSavePrefab);
		AddCommand("LoadPrefab", "forge_prefab_load", "Load prefab from a file", eCommandFlagsNone, CommandLoadPrefab);
		AddCommand("DumpPrefabs", "forge_prefab_dump", "Dump a list of saved prefabs in json", eCommandFlagsNone, CommandDumpPrefabs);
		AddCommand("SetName", "forge_set_name", "Set the name of the current map varient", eCommandFlagsNone, CommandSetName);
		AddCommand("SetDescription", "forge_set_description", "Set the description that will be used when the current map varrient", eCommandFlagsNone, CommandSetDescription);
	}
}
