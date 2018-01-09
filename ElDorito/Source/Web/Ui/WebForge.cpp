#include "WebForge.hpp"
#include "../../Blam/Tags/TagInstance.hpp"
#include "../../Blam/Tags/TagBlock.hpp"
#include "../../Blam/Tags/Objects/Object.hpp"
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
	enum class PropertyTarget
	{
		General_OnMapAtStart,
		General_Symmetry,
		General_RespawnRate,
		General_SpareClips,
		General_SpawnOrder,
		General_Team,
		General_TeleporterChannel,
		General_ShapeType,
		General_ShapeRadius,
		General_ShapeWidth,
		General_ShapeTop,
		General_ShapeBottom,
		General_ShapeDepth,
		General_Material,
		General_Material_ColorR,
		General_Material_ColorG,
		General_Material_ColorB,
		General_Physics,
		General_EngineFlags,

		Budget_Minimum,
		Budget_Maximum,

		Light_ColorR,
		Light_ColorG,
		Light_ColorB,
		Light_Intensity,
		Light_Radius,

		Fx_Range,
		Fx_LightIntensity,
		Fx_Hue,
		Fx_Saturation,
		Fx_Desaturation,
		Fx_GammaIncrease,
		Fx_GammaDecrease,
		Fx_ColorFilterR,
		Fx_ColorFilterG,
		Fx_ColorFilterB,
		Fx_ColorFloorR,
		Fx_ColorFloorG,
		Fx_ColorFloorB,
		Fx_Tracing,

		GarbageVolume_CollectDeadBiped,
		GarbageVolume_CollectWeapons,
		GarbageVolume_CollectObjectives,
		GarbageVolume_CollectGrenades,
		GarbageVolume_CollectEquipment,
		GarbageVolume_CollectVehicles,
		GarbageVolume_Interval,

		KillVolume_AlwaysVisible,
		KillVolume_DestroyVehicles,

		Map_DisablePushBarrier,
		Map_DisableDeathBarrier,

		CameraFx_Exposure,
		CameraFx_LightIntensity,
		CameraFx_Bloom
	};

	enum class PropertyDataType
	{
		Int = 0,
		Float
	};

	struct PropertyValue
	{
		union
		{
			float ValueFloat;
			int ValueInt;
		};
	};

	struct PropertyInfo
	{
		PropertyDataType Type;
		PropertyTarget Target;
	};

	class ObjectPropertySetter
	{
	public:
		ObjectPropertySetter(const Blam::MapVariant::VariantProperties& properties, const Blam::MapVariant::BudgetEntry& budget)
			: m_Properties(properties), m_Budget(budget), m_RespawnRequired(false), m_BudgetDirty(false) {}

		void SetProperty(PropertyTarget target, PropertyValue value)
		{
			auto garbageVolumeProperties = reinterpret_cast<Forge::ForgeGarbageVolumeProperties*>(&m_Properties.SharedStorage);
			auto killVolumeProperties = reinterpret_cast<Forge::ForgeKillVolumeProperties*>(&m_Properties.SharedStorage);
			auto mapModifierProperties = reinterpret_cast<Forge::ForgeMapModifierProperties*>(&m_Properties.ZoneRadiusWidth);

			switch (target)
			{
			case PropertyTarget::General_OnMapAtStart:
				SetOnMapAtStart(value.ValueInt);
				break;
			case PropertyTarget::General_Symmetry:
				SetSymmetry(value.ValueInt);
				break;
			case PropertyTarget::General_RespawnRate:
				m_Properties.RespawnTime = value.ValueInt;
				break;
			case PropertyTarget::General_SpareClips:
				m_Properties.SharedStorage = value.ValueInt;
				break;
			case PropertyTarget::General_SpawnOrder:
				m_Properties.SharedStorage = value.ValueInt;
				break;
			case PropertyTarget::General_Team:
				m_Properties.TeamAffilation = value.ValueInt;
				break;
			case PropertyTarget::General_TeleporterChannel:
				m_Properties.SharedStorage = value.ValueInt;
				break;
			case PropertyTarget::General_ShapeType:
				m_Properties.ZoneShape = value.ValueInt;
				break;
			case PropertyTarget::General_ShapeRadius:
				m_Properties.ZoneRadiusWidth = value.ValueFloat;
				break;
			case PropertyTarget::General_ShapeWidth:
				m_Properties.ZoneRadiusWidth = value.ValueFloat;
				break;
			case PropertyTarget::General_ShapeTop:
				m_Properties.ZoneTop = value.ValueFloat;
				break;
			case PropertyTarget::General_ShapeBottom:
				m_Properties.ZoneBottom = value.ValueFloat;
				break;
			case PropertyTarget::General_ShapeDepth:
				m_Properties.ZoneDepth = value.ValueFloat;
				break;
			case PropertyTarget::General_Material:
				m_Properties.SharedStorage = value.ValueInt;
				break;
			case PropertyTarget::General_Material_ColorR:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->ColorR = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::General_Material_ColorG:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->ColorG = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::General_Material_ColorB:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->ColorB = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::General_Physics:
				SetPhysics(value.ValueInt);
				break;
			case PropertyTarget::General_EngineFlags:
				m_Properties.EngineFlags = value.ValueInt;
				break;
			case PropertyTarget::Budget_Minimum:
				SetBudgetMinimum(value.ValueInt);
				break;
			case PropertyTarget::Budget_Maximum:
				SetBudgetMaximum(value.ValueInt);
				break;
			case PropertyTarget::Light_ColorR:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->ColorR = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Light_ColorG:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->ColorG = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Light_ColorB:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->ColorB = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Light_Intensity:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->Intensity = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Light_Radius:
				reinterpret_cast<Forge::ForgeLightProperties*>(&m_Properties.ZoneRadiusWidth)->Range = value.ValueFloat;
				break;
			case PropertyTarget::Fx_ColorFilterR:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->ColorFilterR = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_ColorFilterG:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->ColorFilterG = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_ColorFilterB:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->ColorFilterB = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_ColorFloorR:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->ColorFloorR = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_ColorFloorG:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->ColorFloorG = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_ColorFloorB:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->ColorFloorB = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_Hue:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->Hue = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_LightIntensity:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->LightIntensity = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_Saturation:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->Saturation = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_Desaturation:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->Desaturation = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_Range:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->MaximumDistance = value.ValueInt;
				break;
			case PropertyTarget::Fx_Tracing:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->Tracing = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_GammaIncrease:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->GammaIncrease = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::Fx_GammaDecrease:
				reinterpret_cast<Forge::ForgeScreenFxProperties*>(&m_Properties.ZoneRadiusWidth)->GammaDecrease = int(value.ValueFloat * 255);
				break;
			case PropertyTarget::GarbageVolume_CollectDeadBiped:
			{
				garbageVolumeProperties->Flags &= ~Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectDeadBipeds;
				if (value.ValueInt)
					garbageVolumeProperties->Flags |= Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectDeadBipeds;
				break;
			}
			case PropertyTarget::GarbageVolume_CollectWeapons:
			{
				garbageVolumeProperties->Flags &= ~Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectWeapons;
				if (value.ValueInt)
					garbageVolumeProperties->Flags |= Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectWeapons;
				break;
			}
			case PropertyTarget::GarbageVolume_CollectObjectives:
			{
				garbageVolumeProperties->Flags &= ~Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectObjectives;
				if (value.ValueInt)
					garbageVolumeProperties->Flags |= Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectObjectives;
				break;
			}
			case PropertyTarget::GarbageVolume_CollectGrenades:
			{
				garbageVolumeProperties->Flags &= ~Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectGrenades;
				if (value.ValueInt)
					garbageVolumeProperties->Flags |= Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectGrenades;
				break;
			}
			case PropertyTarget::GarbageVolume_CollectEquipment:
			{
				garbageVolumeProperties->Flags &= ~Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectEquipment;
				if (value.ValueInt)
					garbageVolumeProperties->Flags |= Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectEquipment;
				break;
			}
			case PropertyTarget::GarbageVolume_CollectVehicles:
			{
				garbageVolumeProperties->Flags &= ~Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectVehicles;
				if (value.ValueInt)
					garbageVolumeProperties->Flags |= Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectVehicles;
				break;
			}
			case PropertyTarget::GarbageVolume_Interval:
				garbageVolumeProperties->Interval = value.ValueInt & 0x3;
				break;
			case PropertyTarget::KillVolume_AlwaysVisible:
				killVolumeProperties->Flags &= ~Forge::ForgeKillVolumeProperties::eKillVolumeFlags_AlwaysVisible;
				if (value.ValueInt)
					killVolumeProperties->Flags |= Forge::ForgeKillVolumeProperties::eKillVolumeFlags_AlwaysVisible;
				break;
			case PropertyTarget::KillVolume_DestroyVehicles:
				killVolumeProperties->Flags &= ~Forge::ForgeKillVolumeProperties::eKillVolumeFlags_DestroyVehicles;
				if (value.ValueInt)
					killVolumeProperties->Flags |= Forge::ForgeKillVolumeProperties::eKillVolumeFlags_DestroyVehicles;
				break;
			case PropertyTarget::Map_DisableDeathBarrier:
			{
				auto &flags = reinterpret_cast<Forge::ForgeMapModifierProperties*>(&m_Properties.ZoneRadiusWidth)->Flags;
				flags &= ~Forge::ForgeMapModifierProperties::eMapModifierFlags_DisableDeathBarrier;
				if (value.ValueInt)
					flags |= Forge::ForgeMapModifierProperties::eMapModifierFlags_DisableDeathBarrier;
			}
			break;
			case PropertyTarget::Map_DisablePushBarrier:
			{
				auto &flags = reinterpret_cast<Forge::ForgeMapModifierProperties*>(&m_Properties.ZoneRadiusWidth)->Flags;
				flags &= ~Forge::ForgeMapModifierProperties::eMapModifierFlags_DisablePushBarrier;
				if (value.ValueInt)
					flags |= Forge::ForgeMapModifierProperties::eMapModifierFlags_DisablePushBarrier;
			}
			break;
			case PropertyTarget::CameraFx_Exposure:
				mapModifierProperties->CameraFxExposure = int(value.ValueFloat * 255.0f);
				break;
			case PropertyTarget::CameraFx_LightIntensity:
				mapModifierProperties->CameraFxLightIntensity = int(value.ValueFloat * 255.0f);
				break;
			case PropertyTarget::CameraFx_Bloom:
				mapModifierProperties->CameraFxBloom = int(value.ValueFloat * 255.0f);
				break;

			}
		}

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

		void SetPhysics(int value)
		{
			switch (value)
			{
			case 0:
				m_Properties.ZoneShape = 0;
				break;
			case 1:
				m_Properties.ZoneShape = 4;
				break;
			}

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

	struct IObjectPropertySink
	{
		virtual void SetProperty(PropertyTarget target, PropertyValue value) = 0;
		virtual ~IObjectPropertySink() {}
	};

	class DeferedPropertySink : public IObjectPropertySink
	{
	public:
		void SetProperty(PropertyTarget target, PropertyValue value) override
		{
			m_Values.emplace_back(target, value);
		}

		void Apply(uint32_t playerIndex, int16_t placementIndex)
		{
			auto mapv = Forge::GetMapVariant();
			if (!mapv || placementIndex == -1)
				return;

			const auto &placement = mapv->Placements[placementIndex];
			ObjectPropertySetter propertySetter(placement.Properties, mapv->Budget[placement.BudgetIndex]);
			for (auto& v : m_Values)
				propertySetter.SetProperty(std::get<0>(v), std::get<1>(v));

			propertySetter.Apply(playerIndex, placementIndex);
			m_Values.clear();
		}

	private:
		std::vector<std::tuple<PropertyTarget, PropertyValue>> m_Values;
	};

	class ImmediatePropertySink : public IObjectPropertySink
	{
	public:

		ImmediatePropertySink(uint32_t playerIndex, int16_t placementIndex) :
			m_PlayerIndex(playerIndex), m_PlacementIndex(placementIndex) {}

		void SetProperty(PropertyTarget target, PropertyValue value) override
		{
			auto mapv = Forge::GetMapVariant();
			if (!mapv)
				return;

			const auto &placement = mapv->Placements[m_PlacementIndex];
			ObjectPropertySetter propertySetter(placement.Properties, mapv->Budget[placement.BudgetIndex]);
			propertySetter.SetProperty(target, value);
			propertySetter.Apply(m_PlayerIndex, m_PlacementIndex);
		}

	private:
		uint32_t m_PlayerIndex;
		int16_t m_PlacementIndex;
	};

	std::string SerializeObjectProperties(int16_t placementIndex);
	void DeserializeObjectProperties(const rapidjson::Value &json, IObjectPropertySink& sink);

	void SerializeProperty(rapidjson::Writer<rapidjson::StringBuffer>& writer, const char* key, int value) { writer.Key(key); writer.Int(value); };
	void SerializeProperty(rapidjson::Writer<rapidjson::StringBuffer>& writer, const char* key, float value) 
	{
		if (!std::isnan(value) && !std::isinf(value))
		{
			writer.Key(key);
			writer.Double(value);
		}
	};
	void SerializeProperty(rapidjson::Writer<rapidjson::StringBuffer>& writer, const char* key, bool value) { writer.Key(key); writer.Bool(value); };

	uint32_t s_CurrentObjectIndex = -1;
	DeferedPropertySink s_ItemSpawnProperties;
}

namespace Web::Ui::WebForge
{
	void OnItemSpawned(uint32_t objectIndex)
	{
		auto object = Blam::Objects::Get(objectIndex);
		if (!object || object->PlacementIndex == -1)
			return;

		s_ItemSpawnProperties.Apply(Blam::Players::GetLocalPlayer(0), object->PlacementIndex);
	}

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

	void ShowObjectCreation()
	{
		const auto mapv = Forge::GetMapVariant();
		if (!mapv)
			return;

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		char buff[256];

		writer.StartObject();
		writer.Key("budget");
		writer.StartArray();
		for (auto i = 0; i < mapv->BudgetEntryCount; i++)
		{
			const auto& itemBudget = mapv->Budget[i];

			sprintf_s(buff, 256, "0x%x", itemBudget.TagIndex);

			writer.StartObject();
			writer.Key("tagindex");
			writer.String(buff);
			SerializeProperty(writer, "max_allowed", itemBudget.DesignTimeMax);
			SerializeProperty(writer, "count_on_map", itemBudget.CountOnMap);
			SerializeProperty(writer, "runtime_min", itemBudget.RuntimeMin);
			SerializeProperty(writer, "runtime_max", itemBudget.RuntimeMax);
			writer.EndObject();
		}
		writer.EndArray();
		writer.EndObject();

		Web::Ui::ScreenLayer::Show("forge_object_creation", buffer.GetString());
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
		case 1: // set immediate properties
		{
			auto currentObject = Blam::Objects::Get(s_CurrentObjectIndex);
			if (currentObject && currentObject->PlacementIndex != 0xFFFF)
			{
				auto mapv = Forge::GetMapVariant();
				auto playerIndex = Blam::Players::GetLocalPlayer(0);
				DeferedPropertySink sink;
				DeserializeObjectProperties(data->value, sink);
				sink.Apply(playerIndex, currentObject->PlacementIndex);
			}
		}
		break;
		case 2: // set defered properties
			DeserializeObjectProperties(data->value, s_ItemSpawnProperties);
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

	bool IsForgeLight(uint32_t objectIndex)
	{
		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;
		auto objectDef = Blam::Tags::TagInstance(object->TagIndex).GetDefinition<Blam::Tags::Objects::Object>();
		if (!objectDef)
			return false;

		for (const auto& attachment : objectDef->Attachments)
		{
			if (attachment.Attached.GroupTag != 'ligh')
				continue;

			auto lightFlags = *(uint32_t*)Blam::Tags::TagInstance(attachment.Attached.TagIndex).GetDefinition<uint8_t>();
			if (lightFlags & (1 << 31))
				return true;
		}

		return false;
	}

	bool IsForgeScreenEffectObject(uint32_t objectIndex)
	{
		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;
		auto objectDef = Blam::Tags::TagInstance(object->TagIndex).GetDefinition<Blam::Tags::Objects::Object>();
		if (!objectDef)
			return false;

		for (const auto& attachment : objectDef->Attachments)
		{
			if (attachment.Attached.GroupTag != 'effe')
				continue;

			if (attachment.Marker == 0x13BB)
				return true;
		}

		return false;
	}

	std::string SerializeObjectProperties(int16_t placementIndex)
	{
		static auto Weapon_HasSpareClips = (bool(*)(uint32_t tagIndex))(0x00B624E0);

		auto mapv = Forge::GetMapVariant();
		auto placement = mapv->Placements[placementIndex];
		const auto& properties = placement.Properties;
		const auto& budget = mapv->Budget[placement.BudgetIndex];

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

		auto symmetry = 0;
		if (properties.ObjectFlags & 4)
		{
			if (!(properties.ObjectFlags & 8))
				symmetry = 1;
		}
		else
			symmetry = 2;

		auto lightProperties = reinterpret_cast<const Forge::ForgeLightProperties*>(&properties.ZoneRadiusWidth);
		auto screenFxProperties = reinterpret_cast<const Forge::ForgeScreenFxProperties*>(&properties.ZoneRadiusWidth);
		auto reforgeProperties = reinterpret_cast<const Forge::ReforgeObjectProperties*>(&properties.ZoneRadiusWidth);
		auto mapModifierProperties = reinterpret_cast<const Forge::ForgeMapModifierProperties*>(&properties.ZoneRadiusWidth);
		auto garbageVolumeProperties = reinterpret_cast<const Forge::ForgeGarbageVolumeProperties*>(&properties.SharedStorage);
		auto killVolumeProperties = reinterpret_cast<const Forge::ForgeKillVolumeProperties*>(&properties.SharedStorage);

		writer.StartObject();
		SerializeProperty(writer, "tag_index", int(budget.TagIndex));
		SerializeProperty(writer, "object_index", int(s_CurrentObjectIndex));
		SerializeProperty(writer, "object_type_mp", properties.ObjectType);
		SerializeProperty(writer, "has_material", CanThemeObject());
		SerializeProperty(writer, "has_spare_clips", properties.ObjectType == 1 && !Weapon_HasSpareClips(budget.TagIndex));
		SerializeProperty(writer, "is_selected", Forge::Selection::GetSelection().Contains(placement.ObjectIndex));
		SerializeProperty(writer, "is_light", IsForgeLight(placement.ObjectIndex));
		SerializeProperty(writer, "is_screenfx", IsForgeScreenEffectObject(placement.ObjectIndex));

		writer.Key("properties");
		writer.StartObject();
		SerializeProperty(writer, "on_map_at_start", ((properties.ObjectFlags >> 1) & 1) == 0 ? 1 : 0);
		SerializeProperty(writer, "symmetry", symmetry);
		SerializeProperty(writer, "respawn_rate", properties.RespawnTime);
		SerializeProperty(writer, "spawn_order", properties.SharedStorage);
		SerializeProperty(writer, "spare_clips", properties.SharedStorage);
		SerializeProperty(writer, "team_affiliation", properties.TeamAffilation);
		SerializeProperty(writer, "engine_flags", properties.EngineFlags);
		SerializeProperty(writer, "teleporter_channel", properties.SharedStorage);
		SerializeProperty(writer, "shape_type", properties.ZoneShape);
		SerializeProperty(writer, "shape_radius", properties.ZoneRadiusWidth);
		SerializeProperty(writer, "shape_top", properties.ZoneTop);
		SerializeProperty(writer, "shape_bottom", properties.ZoneBottom);
		SerializeProperty(writer, "shape_width", properties.ZoneRadiusWidth);
		SerializeProperty(writer, "shape_depth", properties.ZoneDepth);
		SerializeProperty(writer, "appearance_material", properties.SharedStorage);
		SerializeProperty(writer, "appearance_material_color_r", reforgeProperties->ColorR / 255.0f);
		SerializeProperty(writer, "appearance_material_color_g", reforgeProperties->ColorG / 255.0f);
		SerializeProperty(writer, "appearance_material_color_b", reforgeProperties->ColorB / 255.0f);
		SerializeProperty(writer, "physics", properties.ZoneShape == 4 ? 1 : 0);
		SerializeProperty(writer, "light_color_b", lightProperties->ColorB / 255.0f);
		SerializeProperty(writer, "light_color_g", lightProperties->ColorG / 255.0f);
		SerializeProperty(writer, "light_color_r", lightProperties->ColorR / 255.0f);
		SerializeProperty(writer, "light_intensity", lightProperties->Intensity / 255.0f);
		SerializeProperty(writer, "light_radius", lightProperties->Range);
		SerializeProperty(writer, "fx_color_filter_r", screenFxProperties->ColorFilterR / 255.0f);
		SerializeProperty(writer, "fx_color_filter_g", screenFxProperties->ColorFilterG / 255.0f);
		SerializeProperty(writer, "fx_color_filter_b", screenFxProperties->ColorFilterB / 255.0f);
		SerializeProperty(writer, "fx_color_floor_r", screenFxProperties->ColorFloorR / 255.0f);
		SerializeProperty(writer, "fx_color_floor_g", screenFxProperties->ColorFloorG / 255.0f);
		SerializeProperty(writer, "fx_color_floor_b", screenFxProperties->ColorFloorB / 255.0f);
		SerializeProperty(writer, "fx_hue", screenFxProperties->Hue / 255.0f);
		SerializeProperty(writer, "fx_saturation", screenFxProperties->Saturation / 255.0f);
		SerializeProperty(writer, "fx_desaturation", screenFxProperties->Desaturation / 255.0f);
		SerializeProperty(writer, "fx_light_intensity", screenFxProperties->LightIntensity / 255.0f);
		SerializeProperty(writer, "fx_gamma_inc", screenFxProperties->GammaIncrease / 255.0f);
		SerializeProperty(writer, "fx_gamma_dec", screenFxProperties->GammaDecrease / 255.0f);
		SerializeProperty(writer, "fx_range", screenFxProperties->MaximumDistance);
		SerializeProperty(writer, "fx_tracing", screenFxProperties->Tracing / 255.0f);
		SerializeProperty(writer, "map_disable_push_barrier", (int)((mapModifierProperties->Flags & Forge::ForgeMapModifierProperties::eMapModifierFlags_DisablePushBarrier) != 0));
		SerializeProperty(writer, "map_disable_death_barrier", (int)((mapModifierProperties->Flags & Forge::ForgeMapModifierProperties::eMapModifierFlags_DisableDeathBarrier) != 0));

		SerializeProperty(writer, "camera_fx_exposure", mapModifierProperties->CameraFxExposure / 255.0f);
		SerializeProperty(writer, "camera_fx_bloom", mapModifierProperties->CameraFxBloom / 255.0f);
		SerializeProperty(writer, "camera_fx_light_intensity", mapModifierProperties->CameraFxLightIntensity / 255.0f);

		SerializeProperty(writer, "garbage_volume_collect_dead_biped", (int)((garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectDeadBipeds) != 0));
		SerializeProperty(writer, "garbage_volume_collect_weapons", (int)((garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectWeapons) != 0));
		SerializeProperty(writer, "garbage_volume_collect_objectives", (int)((garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectObjectives) != 0));
		SerializeProperty(writer, "garbage_volume_collect_grenades", (int)((garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectGrenades) != 0));
		SerializeProperty(writer, "garbage_volume_collect_equipment", (int)((garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectEquipment) != 0));
		SerializeProperty(writer, "garbage_volume_collect_vehicles", (int)((garbageVolumeProperties->Flags & Forge::ForgeGarbageVolumeProperties::eGarbageVolumeFlags_CollectVehicles) != 0));
		SerializeProperty(writer, "garbage_volume_interval", (int)(garbageVolumeProperties->Interval & 0x3));

		SerializeProperty(writer, "kill_volume_destroy_vehicles", (int)((killVolumeProperties->Flags & Forge::ForgeKillVolumeProperties::eKillVolumeFlags_DestroyVehicles) != 0));
		SerializeProperty(writer, "kill_volume_always_visible", (int)((killVolumeProperties->Flags & Forge::ForgeKillVolumeProperties::eKillVolumeFlags_AlwaysVisible) != 0));

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

	void DeserializeObjectProperties(const rapidjson::Value &json, IObjectPropertySink& sink)
	{
		const static std::unordered_map<std::string, PropertyInfo> s_PropertyTypeLookup =
		{
			{ "on_map_at_start",{ PropertyDataType::Int, PropertyTarget::General_OnMapAtStart } },
			{ "symmetry",{ PropertyDataType::Int, PropertyTarget::General_Symmetry } },
			{ "respawn_rate",{ PropertyDataType::Int, PropertyTarget::General_RespawnRate } },
			{ "spare_clips",{ PropertyDataType::Int, PropertyTarget::General_SpareClips } },
			{ "spawn_order",{ PropertyDataType::Int, PropertyTarget::General_SpawnOrder } },
			{ "team_affiliation",{ PropertyDataType::Int, PropertyTarget::General_Team } },
			{ "engine_flags", {PropertyDataType::Int, PropertyTarget::General_EngineFlags } },
			{ "physics",{ PropertyDataType::Int, PropertyTarget::General_Physics } },
			{ "appearance_material",{ PropertyDataType::Int, PropertyTarget::General_Material } },
			{ "appearance_material_color_r",{ PropertyDataType::Float, PropertyTarget::General_Material_ColorR } },
			{ "appearance_material_color_g",{ PropertyDataType::Float, PropertyTarget::General_Material_ColorG } },
			{ "appearance_material_color_b",{ PropertyDataType::Float, PropertyTarget::General_Material_ColorB } },
			{ "teleporter_channel",{ PropertyDataType::Int, PropertyTarget::General_TeleporterChannel } },
			{ "shape_type",{ PropertyDataType::Int, PropertyTarget::General_ShapeType } },
			{ "shape_radius",{ PropertyDataType::Float, PropertyTarget::General_ShapeRadius } },
			{ "shape_width",{ PropertyDataType::Float, PropertyTarget::General_ShapeWidth } },
			{ "shape_top",{ PropertyDataType::Float, PropertyTarget::General_ShapeTop } },
			{ "shape_bottom",{ PropertyDataType::Float, PropertyTarget::General_ShapeBottom } },
			{ "shape_depth",{ PropertyDataType::Float, PropertyTarget::General_ShapeDepth } },
			{ "light_color_r",{ PropertyDataType::Float, PropertyTarget::Light_ColorR } },
			{ "light_color_g",{ PropertyDataType::Float, PropertyTarget::Light_ColorG } },
			{ "light_color_b",{ PropertyDataType::Float, PropertyTarget::Light_ColorB } },
			{ "light_intensity",{ PropertyDataType::Float, PropertyTarget::Light_Intensity } },
			{ "light_radius",{ PropertyDataType::Float, PropertyTarget::Light_Radius } },

			{ "fx_range",{ PropertyDataType::Int, PropertyTarget::Fx_Range } },
			{ "fx_hue",{ PropertyDataType::Float, PropertyTarget::Fx_Hue } },
			{ "fx_light_intensity",{ PropertyDataType::Float, PropertyTarget::Fx_LightIntensity } },
			{ "fx_saturation",{ PropertyDataType::Float, PropertyTarget::Fx_Saturation } },
			{ "fx_desaturation",{ PropertyDataType::Float, PropertyTarget::Fx_Desaturation } },
			{ "fx_color_filter_r",{ PropertyDataType::Float, PropertyTarget::Fx_ColorFilterR } },
			{ "fx_color_filter_g",{ PropertyDataType::Float, PropertyTarget::Fx_ColorFilterG } },
			{ "fx_color_filter_b",{ PropertyDataType::Float, PropertyTarget::Fx_ColorFilterB } },
			{ "fx_color_floor_r",{ PropertyDataType::Float, PropertyTarget::Fx_ColorFloorR } },
			{ "fx_color_floor_g",{ PropertyDataType::Float, PropertyTarget::Fx_ColorFloorG } },
			{ "fx_color_floor_b",{ PropertyDataType::Float, PropertyTarget::Fx_ColorFloorB } },
			{ "fx_gamma_inc",{ PropertyDataType::Float, PropertyTarget::Fx_GammaIncrease } },
			{ "fx_gamma_dec",{ PropertyDataType::Float, PropertyTarget::Fx_GammaDecrease } },
			{ "fx_tracing",{ PropertyDataType::Float, PropertyTarget::Fx_Tracing } },

			{ "garbage_volume_collect_dead_biped",{ PropertyDataType::Int, PropertyTarget::GarbageVolume_CollectDeadBiped } },
			{ "garbage_volume_collect_weapons",{ PropertyDataType::Int, PropertyTarget::GarbageVolume_CollectWeapons } },
			{ "garbage_volume_collect_objectives",{ PropertyDataType::Int, PropertyTarget::GarbageVolume_CollectObjectives } },
			{ "garbage_volume_collect_grenades",{ PropertyDataType::Int, PropertyTarget::GarbageVolume_CollectGrenades } },
			{ "garbage_volume_collect_equipment",{ PropertyDataType::Int, PropertyTarget::GarbageVolume_CollectEquipment } },
			{ "garbage_volume_collect_vehicles",{ PropertyDataType::Int, PropertyTarget::GarbageVolume_CollectVehicles } },
			{ "garbage_volume_interval",{ PropertyDataType::Int, PropertyTarget::GarbageVolume_Interval } },

			{ "kill_volume_always_visible",{ PropertyDataType::Int, PropertyTarget::KillVolume_AlwaysVisible } },
			{ "kill_volume_destroy_vehicles",{ PropertyDataType::Int, PropertyTarget::KillVolume_DestroyVehicles } },

			{ "map_disable_push_barrier",{ PropertyDataType::Int, PropertyTarget::Map_DisablePushBarrier } },
			{ "map_disable_death_barrier",{ PropertyDataType::Int, PropertyTarget::Map_DisableDeathBarrier } },

			{ "camera_fx_exposure",{ PropertyDataType::Float, PropertyTarget::CameraFx_Exposure } },
			{ "camera_fx_light_intensity",{ PropertyDataType::Float, PropertyTarget::CameraFx_LightIntensity } },
			{ "camera_fx_bloom",{ PropertyDataType::Float, PropertyTarget::CameraFx_Bloom } },

			{ "summary_runtime_minimum",{ PropertyDataType::Int, PropertyTarget::Budget_Minimum } },
			{ "summary_runtime_maximum",{ PropertyDataType::Int, PropertyTarget::Budget_Maximum } },
		};

		for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it)
		{
			const auto& name = it->name.GetString();

			auto typeIt = s_PropertyTypeLookup.find(name);
			if (typeIt == s_PropertyTypeLookup.end())
				continue;

			auto info = typeIt->second;

			PropertyValue value;
			switch (info.Type)
			{
			case PropertyDataType::Int:
				value.ValueInt = it->value.GetInt();
				break;
			case PropertyDataType::Float:
				value.ValueFloat = static_cast<float>(it->value.GetDouble());
				break;
			}

			sink.SetProperty(info.Target, value);
		}
	}
}