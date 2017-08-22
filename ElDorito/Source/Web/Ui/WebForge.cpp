#include "WebForge.hpp"
#include "../../Blam/Tags/TagInstance.hpp"
#include "../../Blam/Tags/TagBlock.hpp"
#include "../../Blam/Tags/TagReference.hpp"
#include "../../Blam/Math/RealVector3D.hpp"
#include "../../Blam/BlamPlayers.hpp"
#include "../../Blam/BlamObjects.hpp"
#include "../../Blam/BlamTypes.hpp"
#include "../../Patches/Input.hpp"
#include "../../ThirdParty/rapidjson/writer.h"
#include "../../ThirdParty/rapidjson/document.h"
#include "../../ThirdParty/rapidjson/stringbuffer.h"
#include "../../Forge/ForgeUtil.hpp"
#include "ScreenLayer.hpp"
#include "../../ElDorito.hpp"
#include "../Bridge/WebRendererQuery.hpp"
#include "../../Pointer.hpp"
#include "../../Forge/Selection.hpp"
#include "../../Forge/ObjectSet.hpp"
#include <unordered_map>

using namespace Blam::Math;
using namespace Anvil::Client::Rendering::Bridge;

namespace
{
	class ObjectPropertySink
	{
	public:
		ObjectPropertySink(const Blam::MapVariant::VariantProperties& properties, const Blam::MapVariant::BudgetEntry& budget)
			: m_Properties(properties), m_Budget(budget), m_RespawnRequired(false), m_SyncRequired(false), m_BudgetDirty(false) {}

		bool RespawnRequired() const
		{
			return m_RespawnRequired;
		}

		bool SyncRequired() const
		{
			return m_SyncRequired;
		}

		void SetOnMapAtStart(int value)
		{
			if (!value)
				m_Properties.ObjectFlags |= 2u;
			else
				m_Properties.ObjectFlags &= ~2u;
		}

		void SetSymmetry(int value)
		{
			switch (value)
			{
			case 2:
				m_Properties.ObjectFlags = m_Properties.ObjectFlags & ~4u | 8u;
				break;
			case 1:
				m_Properties.ObjectFlags = m_Properties.ObjectFlags & ~8u | 4u;
				break;
			case 0:
				m_Properties.ObjectFlags |= 0xCu;
				break;
			}
		}

		void SetRespawnRate(int respawnRate)
		{
			m_Properties.RespawnTime = respawnRate;
		}

		void SetSpareClips(int value)
		{
			m_Properties.SharedStorage = value;
		}

		void SetSpawnOrder(int value)
		{
			m_Properties.SharedStorage = value;
		}

		void SetTeam(int value)
		{
			m_Properties.TeamAffilation = value;
		}

		void SetTeleporterChannel(int value)
		{
			m_Properties.SharedStorage = value;
		}

		void SetShapeType(float value)
		{
			m_Properties.ZoneShape = value;
			m_SyncRequired = true;
		}

		void SetShapeRadius(float value)
		{
			m_Properties.ZoneRadiusWidth = value;
		}

		void SetShapeWidth(float value)
		{
			m_Properties.ZoneRadiusWidth = value;
		}

		void SetShapeTop(float value)
		{
			m_Properties.ZoneTop = value;
		}

		void SetShapeBottom(float value)
		{
			m_Properties.ZoneBottom = value;
		}

		void SetShapeDepth(float value)
		{
			m_Properties.ZoneDepth = value;
		}

		void SetMaterial(int value)
		{
			m_Properties.SharedStorage = value;
		}

		void SetBudgetMinimum(int value)
		{
			m_Budget.RuntimeMin = value;
			m_BudgetDirty = true;
		}

		void SetBudgetMaximum(int value)
		{
			m_Budget.RuntimeMax = value;
			m_BudgetDirty = true;
		}

		void Apply(uint32_t playerIndex, int16_t placementIndex)
		{
			static auto Forge_SetPlacementVariantProperties = (void(*)(uint32_t playerIndex,
				int placementIndex, Blam::MapVariant::VariantProperties *properties))(0x0059B720);

			Forge_SetPlacementVariantProperties(playerIndex, placementIndex, &m_Properties);

			if (m_BudgetDirty)
			{
				Forge::ForgeMessage msg;
				msg.Type = 6;
				msg.PlayerIndex = playerIndex;
				msg.TagIndex = m_Budget.TagIndex;
				msg.QuotaMin = m_Budget.RuntimeMin;
				msg.QuotaMax = m_Budget.RuntimeMax;

				static auto Forge_SendMessage = (void(*)(Forge::ForgeMessage*))(0x004735D0);
				Forge_SendMessage(&msg);
			}
		}

	private:
		Blam::MapVariant::VariantProperties m_Properties;
		Blam::MapVariant::BudgetEntry m_Budget;
		bool m_Valid;
		bool m_RespawnRequired;
		bool m_SyncRequired;
		bool m_BudgetDirty;
	};

	std::string SerializeObjectProperties(int16_t placementIndex);
	void DeserializeObjectProperties(const rapidjson::Value &json, ObjectPropertySink& Setor);

	uint32_t s_CurrentObjectIndex = -1;
}

namespace Web::Ui::WebForge
{
	void ShowObjectProperties(uint32_t objectIndex)
	{
		auto currentObject = Blam::Objects::Get(objectIndex);
		if (currentObject && currentObject->PlacementIndex != 0xFFFF)
		{
			auto mapv = Forge::GetMapVariant();
			auto placement = mapv->Placements[currentObject->PlacementIndex];

			s_CurrentObjectIndex = objectIndex;

			Web::Ui::ScreenLayer::Show("forge_object_properties", SerializeObjectProperties(currentObject->PlacementIndex));
		}
	}

	QueryError ProcessAction(const rapidjson::Value &p_Args, std::string *p_Result)
	{
		auto type = p_Args.FindMember("type");
		auto data = p_Args.FindMember("data");
		if (type == p_Args.MemberEnd() || !type->value.IsNumber())
		{
			*p_Result = "Bad query : A \"type\" argument is required and must be a number";
			return QueryError_BadQuery;
		}
		if (data == p_Args.MemberEnd() || !data->value.IsObject())
		{
			*p_Result = "Bad query : \"data\" argument is required and must be a object";
			return QueryError_BadQuery;
		}

		switch (type->value.GetInt())
		{
		case 1:
		{
			auto currentObject = Blam::Objects::Get(s_CurrentObjectIndex);
			if (currentObject && currentObject->PlacementIndex != 0xFFFF)
			{
				auto mapv = Forge::GetMapVariant();
				auto placement = mapv->Placements[currentObject->PlacementIndex];
				auto playerIndex = Blam::Players::GetLocalPlayer(0);

				ObjectPropertySink sink(placement.Properties, mapv->Budget[placement.BudgetIndex]);
				DeserializeObjectProperties(data->value, sink);

				sink.Apply(playerIndex, currentObject->PlacementIndex);
			}
		}
		break;
		}

		return QueryError_Ok;
	}
}

namespace
{

	bool TryParseInt(const std::string& str, int* value)
	{
		if (str.length() == 0)
			return false;

		auto c_str = str.c_str();
		char* endp;

		*value = std::strtol(c_str, &endp, 10);

		return endp != c_str;
	}

	bool TryParseByte(const std::string& str, uint8_t* value)
	{
		int intval;
		if (!TryParseInt(str, &intval))
			return false;

		*value = static_cast<uint8_t>(intval);
		return true;
	}

	bool TryParseFloat(const std::string& str, float* value)
	{
		const auto c_str = str.c_str();
		char* endp;
		*value = static_cast<float>(std::strtod(c_str, &endp));
		return c_str != endp;
	}

	bool CanThemeObject()
	{
		const auto FIRST_THEMEABLE_SHADER_TAG_INDEX = 0x3ab0;

		auto object = Blam::Objects::Get(s_CurrentObjectIndex);
		if (!object)
			return false;

		auto objectDef = Blam::Tags::TagInstance(object->TagIndex).GetDefinition<uint8_t>();
		if (!objectDef)
			return false;
		auto hlmtDef = Blam::Tags::TagInstance(*(uint32_t*)(objectDef + 0x40)).GetDefinition<uint8_t>();
		if (!hlmtDef)
			return false;
		auto modeTagIndex = *(uint32_t*)(hlmtDef + 0xC);

		const auto modeDefinitionPtr = Pointer(Blam::Tags::TagInstance(modeTagIndex).GetDefinition<uint8_t>());
		if (!modeDefinitionPtr)
			return false;

		const auto materialCount = modeDefinitionPtr(0x48).Read<int32_t>();
		const auto& firstMaterialShaderTagRef = modeDefinitionPtr(0x4c)[0].Read<Blam::Tags::TagReference>();
		if (!materialCount || firstMaterialShaderTagRef.TagIndex != FIRST_THEMEABLE_SHADER_TAG_INDEX)
			return false;

		return true;
	}

	uint32_t RespawnObject(uint32_t objectIndex)
	{
		auto object = Blam::Objects::Get(objectIndex);
		if (!object || object->PlacementIndex == -1)
			return -1;

		auto mapv = Forge::GetMapVariant();
		const auto& placement = mapv->Placements[object->PlacementIndex];

		auto newObjectIndex = Forge::SpawnObject(mapv, object->TagIndex, -1, -1,
			&placement.Position, &placement.RightVector, &placement.UpVector,
			-1, -1, &placement.Properties, placement.PlacementFlags);

		if (newObjectIndex != -1)
		{
			Forge::DeleteObject(Blam::Players::GetLocalPlayer(0).Index(), object->PlacementIndex);
		}

		return newObjectIndex;
	}

	void RespawnObject()
	{
		static auto Object_Dispose = (void(*)(uint32_t objectIndex))(0x00B2CD10);

		if (s_CurrentObjectIndex == -1)
			return;

		auto object = Blam::Objects::Get(s_CurrentObjectIndex);
		if (!object || object->PlacementIndex == -1)
			return;

		auto mapv = Forge::GetMapVariant();
		const auto& placement = mapv->Placements[object->PlacementIndex];

		Forge::ObjectSet newSelection;

		auto& selection = Forge::Selection::GetSelection();
		if (selection.Contains(s_CurrentObjectIndex))
		{
			auto placementCount = mapv->UsedPlacementsCount;
			for (auto i = 0; i < placementCount; i++)
			{
				const auto placement = mapv->Placements[i];

				if (!selection.Contains(placement.ObjectIndex))
					continue;

				auto objectIndex = placement.ObjectIndex;
				auto newObjectIndex = RespawnObject(placement.ObjectIndex);

				if (newObjectIndex != -1)
					newSelection.Add(newObjectIndex);
			}

			selection = newSelection;
		}
		else
		{
			s_CurrentObjectIndex = RespawnObject(placement.ObjectIndex);
		}
	}

	void SerializeProperty(rapidjson::Writer<rapidjson::StringBuffer>& writer, const char* key, int value) { writer.Key(key); writer.Int(value); };
	void SerializeProperty(rapidjson::Writer<rapidjson::StringBuffer>& writer, const char* key, float value) { writer.Key(key); writer.Double(value); };
	void SerializeProperty(rapidjson::Writer<rapidjson::StringBuffer>& writer, const char* key, bool value) { writer.Key(key); writer.Bool(value); };

	std::string SerializeObjectProperties(int16_t placementIndex)
	{
		static auto Weapon_HasSpareClips = (bool(*)(uint32_t tagIndex))(0x00B624E0);

		auto mapv = Forge::GetMapVariant();
		auto placement = mapv->Placements[placementIndex];
		const auto& properties = placement.Properties;
		const auto& budget = mapv->Budget[placement.BudgetIndex];

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		writer.StartObject();
		SerializeProperty(writer, "object_type_mp", properties.ObjectType);
		SerializeProperty(writer, "has_material", CanThemeObject());
		SerializeProperty(writer, "has_spare_clips", properties.ObjectType == 1 && !Weapon_HasSpareClips(budget.TagIndex));

		writer.Key("properties");
		writer.StartObject();
		SerializeProperty(writer, "on_map_at_start", ((properties.ObjectFlags >> 1) & 1) == 0 ? 1 : 0);

		auto symmetry = 0;
		if (properties.ObjectFlags & 4)
		{
			if (!(properties.ObjectFlags & 8))
				symmetry = 1;
		}
		else
			symmetry = 2;

		SerializeProperty(writer, "symmetry", symmetry);
		SerializeProperty(writer, "respawn_rate", properties.RespawnTime);
		SerializeProperty(writer, "spare_clips", properties.SharedStorage);
		SerializeProperty(writer, "team_affiliation", properties.TeamAffilation);
		SerializeProperty(writer, "teleporter_channel", properties.SharedStorage);
		SerializeProperty(writer, "shape_type", properties.ZoneShape);
		SerializeProperty(writer, "shape_radius", properties.ZoneRadiusWidth);
		SerializeProperty(writer, "shape_top", properties.ZoneTop);
		SerializeProperty(writer, "shape_bottom", properties.ZoneBottom);
		SerializeProperty(writer, "shape_width", properties.ZoneRadiusWidth);
		SerializeProperty(writer, "shape_depth", properties.ZoneDepth);
		SerializeProperty(writer, "appearance_material", properties.SharedStorage);
		writer.EndObject();

		writer.Key("budget");
		writer.StartObject();
		SerializeProperty(writer, "summary_placed_on_map", budget.CountOnMap);
		SerializeProperty(writer, "summary_total_cost", budget.Cost * budget.CountOnMap);
		SerializeProperty(writer, "summary_maximum_allowed", budget.DesignTimeMax);
		SerializeProperty(writer, "summary_runtime_minimum", budget.RuntimeMin);
		SerializeProperty(writer, "summary_runtime_maximum", budget.RuntimeMax);
		writer.EndObject();

		writer.EndObject();
		return buffer.GetString();
	}

	struct ObjectPropertyData
	{
		Blam::MapVariant::VariantProperties Proprties;
		Blam::MapVariant::BudgetEntry Budget;
	};

	void DeserializeObjectProperties(const rapidjson::Value &json, ObjectPropertySink& sink)
	{
		using PropertySetterFunc = void(*)(const rapidjson::Value&, ObjectPropertySink&);

		static std::unordered_map<std::string, PropertySetterFunc> s_Setters =
		{
			{ "on_map_at_start",		 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetOnMapAtStart(value.GetInt()); } },
			{ "symmetry",				 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetSymmetry(value.GetInt()); } },
			{ "respawn_rate",			 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetRespawnRate(value.GetInt()); } },
			{ "spare_clips",			 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetSpareClips(value.GetInt()); } },
			{ "spawn_order",			 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetSpawnOrder(value.GetInt()); } },
			{ "team_affiliation",		 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetTeam(value.GetInt()); } },
			{ "teleporter_channel",		 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetTeleporterChannel(value.GetInt()); } },
			{ "shape_type",				 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetShapeType(value.GetInt()); } },
			{ "shape_radius",			 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetShapeRadius(value.GetDouble()); } },
			{ "shape_width",			 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetShapeWidth(value.GetDouble()); } },
			{ "shape_top",				 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetShapeTop(value.GetDouble()); } },
			{ "shape_bottom",			 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetShapeBottom(value.GetDouble()); } },
			{ "shape_depth",			 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetShapeDepth(value.GetDouble()); } },
			{ "appearance_material",	 [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetMaterial(value.GetInt()); } },
			{ "summary_runtime_minimum", [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetBudgetMinimum(value.GetInt()); } },
			{ "summary_runtime_maximum", [](const rapidjson::Value& value, ObjectPropertySink& sink) { sink.SetBudgetMaximum(value.GetInt()); } }
		};

		for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it)
		{
			const auto& name = it->name.GetString();
			if (s_Setters.find(name) == s_Setters.end())
				continue;
			auto setter = s_Setters[name];
			setter(it->value, sink);
		}
	}
}