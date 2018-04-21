#include "ModuleForge.hpp"
#include "boost\filesystem.hpp"
#include "../ThirdParty/rapidjson/stringbuffer.h"
#include "../ThirdParty/rapidjson/writer.h"
#include "../Forge/Selection.hpp"
#include "../Forge/ForgeUtil.hpp"
#include "../Forge/Prefab.hpp"
#include "../Blam/Tags/TagReference.hpp"
#include "../Blam/Tags/TagBlock.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Objects/Object.hpp"

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
		auto &moduleForge = Modules::ModuleForge::Instance();
		moduleForge.CommandState.MapCanvas = true;
		return true;
	}

	bool CommandDeselectAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::Clear();
		return true;
	}

	bool CommandDeselectAllOf(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::DeselectAllOf();
		return true;
	}

	bool CommandSelectAll(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::SelectAll();
		return true;
	}

	bool CommandSelectAllMaterial(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::SelectAllMaterial();
		return true;
	}

	bool CommandSelectEverything(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::SelectEverything();
		return true;
	}

	bool CommandExpandSelection(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		if (Arguments.size() < 1)
			return false;
		char *endp;
		float radius = strtof(Arguments[0].c_str(), &endp);
		if (endp == Arguments[0].c_str())
			return false;

		Forge::Selection::Expand(radius);
		return true;
	}

	bool CommandInvertSelection(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		Forge::Selection::Invert();
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

		auto &moduleForge = Modules::ModuleForge::Instance();
		moduleForge.CommandState.Prefabs.Path = p.string();
		moduleForge.CommandState.Prefabs.Name = name;
		moduleForge.CommandState.Prefabs.SavePrefab = true;

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

		auto &moduleForge = Modules::ModuleForge::Instance();
		moduleForge.CommandState.Prefabs.Path = p.string();
		moduleForge.CommandState.Prefabs.LoadPrefab = true;

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
				const auto path = canonical(itr->path());
				const auto &ext = path.extension().string();
				const auto &name = path.stem().string();

				if (boost::filesystem::is_regular_file(path) && ext == ".prefab")
				{
					Forge::Prefabs::PrefabHeader header;
					if (!Forge::Prefabs::ReadHeader(path.string(), header))
						continue;

					writer.StartObject();
					writer.Key("filename");
					writer.String(name.c_str());
					writer.Key("name");
					writer.String(header.Name);
					writer.Key("author");
					writer.String(header.Author);
					writer.Key("object_count");
					writer.Int(header.ObjectCount);
					writer.EndObject();
				}			
			}
		}

		writer.EndArray();

		returnInfo = buffer.GetString();
		return true;
	}

	bool CommandDeletePrefab(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		boost::filesystem::path prefabDirectory(PREFAB_DIR);
		auto p = (prefabDirectory / Arguments[0]).replace_extension(PREFAB_EXT);
		if (!boost::filesystem::exists(p))
		{
			returnInfo = "prefab with that name does not exist";
			return false;
		}
		boost::filesystem::remove(p);
		return true;
	}

	bool CommandDumpPalette(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		using namespace Blam::Tags;

		struct SandboxPaletteBlockDefinition
		{
			TagReference Object;
			uint32_t Name;
			int32_t MaxAllowed;
			float Cost;
			uint32_t Unknown1C;
			uint32_t Unknown20;
			uint32_t Unknown24;
			uint32_t Unknown28;
			uint32_t Unknown2c;
		};
		static_assert(sizeof(SandboxPaletteBlockDefinition) == 0x30, "Invalid SandboxPaletteBlockDefinition");

		const auto lanquageId = ((int(*)())(0x0052FC40))();
		static auto UI_GetString = (char*(*)(uint32_t unicTagIndex, uint32_t id, int lanquageId))(0x0051E290);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartArray();
		auto scnrDefinitionPtr = (uint8_t**)0x022AAEB4;
		if (!scnrDefinitionPtr)
			return false;

		auto mapv = Forge::GetMapVariant();
		if (!mapv)
			return false;

		std::unordered_map<uint32_t, Blam::MapVariant::BudgetEntry*> budgetLookup;
		for (auto i = 0; i < mapv->BudgetEntryCount; i++)
		{
			const auto budget = &mapv->Budget[i];
			budgetLookup[budget->TagIndex] = budget;
		}
		
		auto palettes = (TagBlock<SandboxPaletteBlockDefinition>*)((*scnrDefinitionPtr) + 0x20c);
		for (auto i = 0; i < 7; i++)
		{
			auto& palette = palettes[i];
			for (auto& item : palette)
			{
				auto objectDefinition = TagInstance(item.Object.TagIndex).GetDefinition<Objects::Object>();
				if (!objectDefinition || !objectDefinition->MultiplayerProperties.Count)
					continue;
	
				auto& mpObjectProperties = objectDefinition->MultiplayerProperties[0];
				auto engineFlags = uint32_t(mpObjectProperties.EngineFlags);

			
				if (!(engineFlags & 0x3FF) || engineFlags == 0x3FF ||
					mapv->ContentType != 10 && uint16_t(1 << mapv->ContentType) & engineFlags)
				{
					const char* asciiName = UI_GetString(0x1221, item.Name, lanquageId);
					char tagIndexStr[8];
					sprintf_s(tagIndexStr, "0x%04x", item.Object.TagIndex);

					if (budgetLookup.find(item.Object.TagIndex) == budgetLookup.end())
						continue;

					const auto budget = budgetLookup[item.Object.TagIndex];
					const auto maxAllowed = !palette->MaxAllowed ? 255 : palette->MaxAllowed;

					writer.StartObject();
					writer.Key("tagIndex");
					writer.String(tagIndexStr);
					writer.Key("remaining");
					writer.Int(maxAllowed - budget->CountOnMap);
					writer.Key("cost");
					writer.Double(budget->Cost);
					writer.Key("name");
					writer.Key(asciiName);
					writer.EndObject();
				}
			}
		}
		writer.EndArray();

		returnInfo = buffer.GetString();
		return true;
	}

	bool TryParseTagIndex(const std::string& str, uint32_t* value)
	{
		if (str.length() == 0)
			return false;

		auto c_str = str.c_str();
		char* endp;

		*value = std::strtol(c_str, &endp, 16);

		return endp != c_str;
	}

	bool CommandSpawnItem(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		uint32_t tagIndex;
		if (Arguments.size() < 1 || !TryParseTagIndex(Arguments[0], &tagIndex)) {
			returnInfo = "Invalid tag index";
			return false;
		}

		auto &moduleForge = Modules::ModuleForge::Instance();
		moduleForge.CommandState.SpawnItem = tagIndex;
		return true;
	}

	bool CommandPrematchCamera(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		moduleForge.CommandState.SetPrematchCamera = true;
		return true;
	}

	bool CommandResetRuntime(const std::vector<std::string>& Arguments, std::string& returnInfo)
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		moduleForge.CommandState.RuntimeReset = true;
		return true;
	}

	bool CommandForgeBudget(const std::vector<std::string>& Arguments, std::string &returnInfo)
	{
		const auto &quota = Forge::CalculateObjectQuota();

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();
		writer.Key("total_used");
		writer.Int(quota.TotalObjectsUsed);
		writer.Key("total_available");
		writer.Int(quota.TotalObjectsAvailable);
		writer.Key("total_remaining");
		writer.Int(quota.TotalObjectsAvailable - quota.TotalObjectsUsed);	
		writer.EndObject();

		returnInfo = buffer.GetString();
		return true;
	}

	bool CommandMapOptions(const std::vector<std::string> &Arguments, std::string &returnInfo)
	{
		auto &moduleForge = Modules::ModuleForge::Instance();
		moduleForge.CommandState.OpenMapOptions = true;
		return true;
	}
}

namespace Modules
{
	ModuleForge::ModuleForge() : ModuleBase("Forge")
	{
		VarCloneDepth = AddVariableFloat("CloneDepth", "forge_clone_depth", "Depth at which the object will be cloned", eCommandFlagsNone, 1.0f);
		VarCloneMultiplier = AddVariableInt("CloneMultiplier", "forge_clone_multiplier", "Number of consecutive times the object will be cloned", eCommandFlagsNone, 1);
		VarRotationSnap = AddVariableInt("RotationSnap", "forge_rotation_snap", "Angle at which object rotation will be snapped", eCommandFlagsNone, 0);
		VarRotationSnap->ValueIntMin = 0;
		VarRotationSnap->ValueIntMax = 6;
		VarRotationSensitivity = AddVariableFloat("RotationSensitivity", "forge_rotation_sensitivity", "Controls the sensitivity of object rotation", eCommandFlagsArchived, 1.0f);
		VarMonitorSpeed = AddVariableInt("MonitorSpeed", "forge_monitor_speed", "Controls the movement speed of the monitor", eCommandFlagsArchived, 3);
		VarMonitorSpeed->ValueIntMin = 0;
		VarMonitorSpeed->ValueIntMax = 5;
		VarMonitorNoclip = AddVariableInt("MonitorNoclip", "forge_monitor_noclip", "Allows the monitor to pass through bsp and other objects", CommandFlags(eCommandFlagsHostOnly|eCommandFlagsArchived), 0);
		VarSelectionRenderer = AddVariableInt("SelectionRenderer", "forge_selection_renderer", "Set the selection renderer to use", eCommandFlagsArchived, 0);
		VarSelectionRenderer->ValueIntMin = 0;
		VarSelectionRenderer->ValueIntMax = 1;
		VarMagnetsEnabled = AddVariableInt("Magnets", "forge_magnets", "Controls whether magnets are enabled (1) or disabled (0)", eCommandFlagsArchived, 0);
		VarMagnetsEnabled->ValueIntMin = 0;
		VarMagnetsEnabled->ValueIntMax = 1;
		VarMagnetsVisible = AddVariableInt("MagnetsVisible", "forge_magnets_visible", "Controls whether magnets are shown (1) or hidden (0)", eCommandFlagsArchived, 1);
		VarMagnetsVisible->ValueIntMin = 0;
		VarMagnetsVisible->ValueIntMax = 1;
		VarMaxGrabDistance = AddVariableFloat("GrabDistance", "forge_grab_distance", "Controls the maximum distance from which objects can be grabbed", eCommandFlagsArchived, 5.0f);
		VarMagnetsStrength = AddVariableFloat("MagnetsStrength", "forge_magnets_strength", "Controls the minimum distance at which magnets snap", eCommandFlagsArchived, 0.3f);
		VarShowInvisibles = AddVariableInt("ShowInvisibles", "forge_show_invisibles", "When enabled forces invisible materials to be visible", eCommandFlagsNone, 0);
		VarShowInvisibles->ValueIntMin = 0;
		VarShowInvisibles->ValueIntMax = 1;

		AddCommand("DeleteAll", "forge_delete_all", "Delete all objects that are the same as the object under the crosshair", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandDeleteAll);
		AddCommand("Canvas", "forge_canvas", "Delete all objects on the map", CommandFlags(eCommandFlagsHostOnly| eCommandFlagsForge), CommandCanvas);
		AddCommand("ResetRuntime", "forge_reset_runtime", "Respawn any abandoned/despawned objects", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandResetRuntime);
		AddCommand("SelectAll", "forge_select_all", "Select all objects that are the same as the object under the crosshair", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandSelectAll);
		AddCommand("SelectAllMaterial", "forge_select_all_material", "Select all objects with the same material", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandSelectAllMaterial);
		AddCommand("SelectEverything", "forge_select_everything", "Select all objects on the map", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandSelectEverything);
		AddCommand("ExpandSelection", "forge_expand_selection", "Expand the selection by a given radius", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandExpandSelection);
		AddCommand("InvertSelection", "forge_invert_selection", "Invert the current selection", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandInvertSelection);
		AddCommand("DeselectAll", "forge_deselect_all", "Deselect all selected objects", eCommandFlagsForge, CommandDeselectAll);
		AddCommand("DeselectAllOf", "forge_deselect_all_of", "Deselect all selected objects that are the same as the object under the crosshair", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandDeselectAllOf);
		AddCommand("SavePrefab", "forge_prefab_save", "Save prefab to a file", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandSavePrefab);
		AddCommand("LoadPrefab", "forge_prefab_load", "Load prefab from a file", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandLoadPrefab);
		AddCommand("DumpPrefabs", "forge_prefab_dump", "Dump a list of saved prefabs in json", eCommandFlagsForge, CommandDumpPrefabs);
		AddCommand("DeletePrefab", "forge_prefab_delete", "Delete a saved prefab", eCommandFlagsForge, CommandDeletePrefab);
		AddCommand("DumpPalette", "forge_dump_palette", "Dumps the forge palette in json", eCommandFlagsForge, CommandDumpPalette);
		AddCommand("SpawnItem", "forge_spawn", "Spawn an item from the forge palette", eCommandFlagsForge, CommandSpawnItem);
		AddCommand("Budget", "forge_budget", "Displays information about remaining objects", eCommandFlagsForge, CommandForgeBudget);
		AddCommand("MapOptions", "forge_map_options", "Opens map modifier object properties", eCommandFlagsForge, CommandMapOptions);
		AddCommand("SetPrematchCamera", "forge_set_prematch_camera", "Set the position/orientation of the prematch camera", CommandFlags(eCommandFlagsHostOnly | eCommandFlagsForge), CommandPrematchCamera);
	}
}
