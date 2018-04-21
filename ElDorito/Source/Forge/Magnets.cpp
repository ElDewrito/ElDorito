#include "Magnets.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/Math/RealQuaternion.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/Tags/Objects/Object.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Blam/Cache/StringIdCache.hpp"
#include "../Utils/Logger.hpp"
#include "../Modules/ModuleForge.hpp"
#include "../ThirdParty/rapidjson/document.h"
#include "../ThirdParty/rapidjson/error/en.h"
#include "ObjectSet.hpp"
#include "ForgeUtil.hpp"
#include "Selection.hpp"
#include "Geoemetry.hpp"
#include <vector>
#include <sstream>
#include <fstream>
#include <memory>
#include <unordered_map>

using namespace Forge;
using namespace Blam;
using namespace Blam::Math;
using namespace Magnets;

namespace
{
	const auto MAX_SOURCE_MAGNETS = 512;
	const auto MAX_DEST_MAGNETS = 2048;
	const auto MARKER_STORE_PATH = "./mods/forge/magnets.json";

	class JsonMarkerStore
	{
	public:
		void Load(const std::string& path);
		int GetMarkers(uint32_t name, std::vector<Blam::Math::RealVector3D> &markers);

	private:
		std::unordered_map<uint32_t, std::vector<Blam::Math::RealVector3D>> m_Markers;
	};

	class MagnetManager
	{
	public:
		MagnetManager() : m_LastCheck(0), m_NumSourceMagnets(0), m_NumDestMagnets(0)
		{
			m_MarkerStore.Load(MARKER_STORE_PATH);
		}

		void Update();
		void Render();
		void RenderMagnet(const Magnet& magnet);
		const MagnetPair& GetMagnetPair() const { return m_MagnetPairing; }

	private:
		uint32_t m_LastCheck;
		int m_NumSourceMagnets;
		int m_NumDestMagnets;
		MagnetPair m_MagnetPairing;
		Magnet m_SourceMagnets[MAX_SOURCE_MAGNETS];
		Magnet m_DestMagnets[MAX_DEST_MAGNETS];
		JsonMarkerStore m_MarkerStore;

		int GetObjectMarkers(uint32_t tagIndex, std::vector<RealVector3D> &markers);
		void AddSourceObject(uint32_t objectIndex);
		void AddDestObject(uint32_t objectIndex);
		void PerformPairing();
	};

	std::unique_ptr<MagnetManager> s_MagnetManager;
}

namespace
{
	bool MagnetsEnabled()
	{
		return Modules::ModuleForge::Instance().VarMagnetsEnabled->ValueInt;
	}

	bool MagnetsVisible()
	{
		return Modules::ModuleForge::Instance().VarMagnetsVisible->ValueInt;
	}
}

namespace Forge
{
	bool Magnets::Initialize()
	{
		if (!s_MagnetManager)
		{
			s_MagnetManager = std::make_unique<MagnetManager>();
			if (!s_MagnetManager)
			{
				Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "Failed to initialize magnets. Out of memory");
				return false;
			}
		}
		return true;
	}

	void Magnets::Shutdown()
	{
		s_MagnetManager.reset();
	}

	void Magnets::Update()
	{
		if (!MagnetsEnabled())
			return;

		if (s_MagnetManager)
			s_MagnetManager->Update();
	}

	void Magnets::Render()
	{
		if (!MagnetsEnabled() || !MagnetsVisible())
			return;

		if (s_MagnetManager)
			s_MagnetManager->Render();
	}

	const MagnetPair& Magnets::GetMagnetPair()
	{
		static MagnetPair nullMagnetPair = {};
		if (!s_MagnetManager || !MagnetsEnabled())
			return nullMagnetPair;

		return s_MagnetManager->GetMagnetPair();
	}
}

namespace
{
	void MagnetManager::Update()
	{
		static auto Objects_GetObjectsInCluster = (uint16_t(*)(int a1, uint32_t objectTypeMask, int16_t* pClusterIndex,
			RealVector3D *center, float radius, uint32_t* result, uint16_t maxObjects))(0x00B35B60);

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		if (playerIndex == DatumHandle::Null)
			return;

		uint32_t heldObjectIndex;
		if (!Forge::GetEditorModeState(playerIndex, &heldObjectIndex, nullptr) || heldObjectIndex == -1)
		{
			m_NumSourceMagnets = 0;
			m_NumDestMagnets = 0;
			return;
		}

		const auto& selection = Selection::GetSelection();

		m_NumSourceMagnets = 0;

		AddSourceObject(heldObjectIndex);

		if (selection.Contains(heldObjectIndex))
		{
			const auto& heldObject = Blam::Objects::Get(heldObjectIndex);
			for (auto objectIndex = heldObject->FirstChild; objectIndex != DatumHandle::Null
				&& m_NumSourceMagnets < MAX_SOURCE_MAGNETS;)
			{
				auto object = Blam::Objects::Get(objectIndex);
				if (!object)
					continue;

				AddSourceObject(objectIndex);
				objectIndex = object->NextSibling;
			}
		}

		if (Blam::Time::TicksToSeconds(Blam::Time::GetGameTicks() - m_LastCheck) > 0.3f)
		{
			m_LastCheck = Blam::Time::GetGameTicks();

			auto heldObject = Blam::Objects::Get(heldObjectIndex);

			uint32_t proximityObjects[256];
			auto numProximityObjects = Objects_GetObjectsInCluster(0, 0, &heldObject->ClusterIndex,
				&heldObject->Center, heldObject->Radius, proximityObjects, 256);

			m_NumDestMagnets = 0;
			for (auto i = 0; i < numProximityObjects; i++)
			{
				auto objectIndex = proximityObjects[i];
				if (objectIndex == heldObjectIndex)
					continue;

				AddDestObject(objectIndex);
			}

			PerformPairing();
		}
	}

	void MagnetManager::PerformPairing()
	{
		m_MagnetPairing.IsValid = false;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player)
			return;

		RealMatrix4x3 unitTransform;
		GetObjectTransformationMatrix(player->SlaveUnit, &unitTransform);

		auto shortestDistance = 999.0f;
		auto shortestUnitDistance = 999.0f;
		const auto minDistance = Modules::ModuleForge::Instance().VarMagnetsStrength->ValueFloat;
		// worst case O(n^2)
		for (auto i = 0; i < m_NumSourceMagnets; i++)
		{
			const auto& sourceMagnet = m_SourceMagnets[i];

			auto unitDistance = (unitTransform.Position - sourceMagnet.Position).Length2();

			// try to pick the source closest to the player
			if (m_MagnetPairing.IsValid && unitDistance > shortestUnitDistance)
				continue;

			for (auto j = 0; j < m_NumDestMagnets; j++)
			{
				const auto& destMagnet = m_DestMagnets[j];

				auto d = sourceMagnet.Position - destMagnet.Position;
				auto distance2 = d.Length2();
				
				
				if (distance2 < (minDistance * minDistance) && distance2 < shortestDistance)
				{
					shortestUnitDistance = unitDistance;
					shortestDistance = distance2;
					m_MagnetPairing.IsValid = true;
					m_MagnetPairing.Source = &m_SourceMagnets[i];
					m_MagnetPairing.Dest = &m_DestMagnets[j];
				}
			}
		}
	}

	void MagnetManager::RenderMagnet(const Magnet& magnet)
	{
		using namespace Forge::Geoemetry;

		static const auto sub_A232D0 = (int(*)(int a1))(0xA232D0);
		static const auto sub_A22BA0 = (int(*)())(0xA22BA0);
		static const auto SetShaderConstant = (void(*)(int id, int type, const float* data))(0x00A66410);
		static const auto DrawPrimitive = (int(*)(int primitiveType, int primitiveCount, void *data, int stride))(0x00A28330);

		if (m_MagnetPairing.IsValid && (m_MagnetPairing.Dest == &magnet || m_MagnetPairing.Source == &magnet))
		{
			const float color[] = { 2, 0, 0, 2 };
			SetShaderConstant(20, 1, color);
		}
		else
		{
			const float color[] = { 0, 0, 2, 2 };
			SetShaderConstant(20, 1, color);
		}

		const auto scale = 0.05f;
		RealVector3D forward(1, 0, 0), left(0, 1, 0), up(0, 0, 1);
		float m[] = 				// transpose
		{
			forward.I * scale, left.I * scale,  up.I * scale, magnet.Position.I,
			forward.J * scale, left.J * scale,  up.J * scale, magnet.Position.J,
			forward.K * scale, left.K * scale,  up.K * scale, magnet.Position.K
		};

		SetShaderConstant(24, 3, m);

		auto v14 = sub_A22BA0();
		sub_A232D0(1);
		DrawPrimitive(PT_TRIANGLESTRIP, 2, (void*)BOX_VERTICES, sizeof(VertexPosUV));
		DrawPrimitive(PT_TRIANGLESTRIP, 2, (void*)(BOX_VERTICES + 4), sizeof(VertexPosUV));
		DrawPrimitive(PT_TRIANGLESTRIP, 2, (void*)(BOX_VERTICES + 8), sizeof(VertexPosUV));
		DrawPrimitive(PT_TRIANGLESTRIP, 2, (void*)(BOX_VERTICES + 12), sizeof(VertexPosUV));
		DrawPrimitive(PT_TRIANGLESTRIP, 2, (void*)(BOX_VERTICES + 16), sizeof(VertexPosUV));
		DrawPrimitive(PT_TRIANGLESTRIP, 2, (void*)(BOX_VERTICES + 20), sizeof(VertexPosUV));
		sub_A232D0(v14);
	}

	static uint16_t GetPlasmaShaderTagIndex()
	{
		static bool tagLookup = true;
		static uint16_t tagIndex = 0xFFFF;

		if (tagLookup && (tagIndex == 0xFFFF))
		{
			// lookup the multiplayer_object_plasma tag instance
			tagIndex = Blam::Tags::TagInstance::Find('rmsh', "objects\\multi\\shaders\\multiplayer_object_plasma").Index;

			// tag instance was not found, don't attempt to look it up again
			if (tagIndex == 0xFFFF)
				tagLookup = false;
		}

		return tagIndex;
	}

	void MagnetManager::Render()
	{
		static const auto UseDefaultShader = (bool(*)(int defaultShaderIndex, int a2, int a3, int a4))(0x00A23300);
		static const auto sub_A3CA60 = (void(*)(uint32_t shaderTagIndex, void* shaderDef, int a3, unsigned int a4, int a5, int a6))(0xA3CA60);

		const auto SHADER_TAGINDEX = GetPlasmaShaderTagIndex();
		if (!UseDefaultShader(64, 0x14, 0, 0))
			return;

		auto shaderDef = Blam::Tags::TagInstance(SHADER_TAGINDEX).GetDefinition<void>();
		sub_A3CA60(SHADER_TAGINDEX, shaderDef, 20, 0, Geoemetry::PT_TRIANGLESTRIP, 1);

		for (auto i = 0; i < m_NumDestMagnets; i++)
			RenderMagnet(m_DestMagnets[i]);
		for (auto i = 0; i < m_NumSourceMagnets; i++)
			RenderMagnet(m_SourceMagnets[i]);
	}

	int MagnetManager::GetObjectMarkers(uint32_t tagIndex, std::vector<RealVector3D> &markers)
	{
		using ObjectDefinition = Blam::Tags::Objects::Object;
		auto objeDefinition = Blam::Tags::TagInstance(tagIndex).GetDefinition<ObjectDefinition>();
		if (!objeDefinition)
			return 0;
		auto hlmtDefinition = Blam::Tags::TagInstance(objeDefinition->Model.TagIndex).GetDefinition<uint8_t>();
		if (!hlmtDefinition)
			return 0;
		auto modeDefinition = Blam::Tags::TagInstance(*(uint32_t*)&hlmtDefinition[0xC]).GetDefinition<uint8_t>();
		if (!modeDefinition)
			return 0;

		return m_MarkerStore.GetMarkers(*(uint32_t*)modeDefinition, markers);
	}

	void MagnetManager::AddSourceObject(uint32_t objectIndex)
	{
		const auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return;

		RealMatrix4x3 objectTransform;
		GetObjectTransformationMatrix(objectIndex, &objectTransform);
		const auto objectRotation = RealQuaternion::CreateFromRotationMatrix(objectTransform);

		auto markers = std::vector<RealVector3D>();
		const auto numMarkers = GetObjectMarkers(object->TagIndex, markers);
		for (auto i = 0; i < numMarkers; i++)
		{
			if (m_NumSourceMagnets >= MAX_SOURCE_MAGNETS)
				return;

			m_SourceMagnets[m_NumSourceMagnets].Position =
				RealVector3D::Transform(markers[i], objectRotation) + objectTransform.Position;
            ++m_NumSourceMagnets;
		}
	}

	void MagnetManager::AddDestObject(uint32_t objectIndex)
	{
		const auto& object = Blam::Objects::Get(objectIndex);
		if (!object || object->PlacementIndex == -1)
			return;

		RealMatrix4x3 objectTransform;
		GetObjectTransformationMatrix(objectIndex, &objectTransform);
		const auto objectRotation = RealQuaternion::CreateFromRotationMatrix(objectTransform);

		auto markers = std::vector<RealVector3D>();
		const auto numMarkers = GetObjectMarkers(object->TagIndex, markers);
		for (auto i = 0; i < numMarkers; i++)
		{
			if (m_NumDestMagnets >= MAX_DEST_MAGNETS)
				return;

			m_DestMagnets[m_NumDestMagnets].Position =
				RealVector3D::Transform(markers[i], objectRotation) + objectTransform.Position;
            ++m_NumDestMagnets;
		}
	}
}

namespace
{
	int JsonMarkerStore::GetMarkers(uint32_t name, std::vector<RealVector3D> &markers)
	{
		auto it = m_Markers.find(name);
		if (it == m_Markers.end())
			return 0;

		for (const auto& m : it->second)
			markers.push_back(m);

		return markers.size();
	}

	template <typename T>
	bool ReadJsonArrayIntoVector3(T& value, RealVector3D* outVector)
	{
		if (!outVector)
			return false;

		auto it = value->Begin();
		if (it == value->End())
			return false;
		outVector->I = static_cast<float>(it->GetDouble());
		if (++it == value->End())
			return false;
		outVector->J = static_cast<float>(it->GetDouble());
		if (++it == value->End())
			return false;
		outVector->K = static_cast<float>(it->GetDouble());

		return true;
	}

	void JsonMarkerStore::Load(const std::string& path)
	{
		std::unordered_map<std::string, uint32_t> stringIdLookup;
		Blam::Cache::StringIDCache stringIdCache;
		if (!stringIdCache.Load(ElDorito::Instance().GetMapsFolder() + "string_ids.dat"))
			return;

		for (auto i = 0; i < stringIdCache.Header.StringCount; i++)
		{
			const auto s = stringIdCache.Strings[i];
			if (!s)
				continue;
			stringIdLookup[s] = i;
		}

		std::ifstream is(path);
		if (!is.is_open())
			return;

		std::stringstream ss;
		ss << is.rdbuf();

		const auto source = ss.str();

		rapidjson::Document doc;
		if ((doc.Parse<0>(source.c_str()).HasParseError()))
		{
			Utils::Logger::Instance().Log(Utils::LogTypes::Game, Utils::LogLevel::Error, "failed to parse magnets.json. Error: %s (%u)",
				rapidjson::GetParseError_En(doc.GetParseError()), doc.GetErrorOffset());
			return;
		}

		if (!doc.IsObject())
			return;

		for (auto objectIt = doc.MemberBegin(); objectIt != doc.MemberEnd(); ++objectIt)
		{
			const auto name = objectIt->name.GetString();
			if (!objectIt->value.IsArray())
				continue;

			auto lookupIt = stringIdLookup.find(name);
			if (lookupIt == stringIdLookup.end())
				continue;

			auto modelNameId = lookupIt->second;

			std::vector<RealVector3D> markers;
			for (auto markerIt = objectIt->value.Begin(); markerIt != objectIt->value.End(); ++markerIt)
			{
				RealVector3D vec;
				ReadJsonArrayIntoVector3(markerIt, &vec);
				markers.emplace_back(vec);
			}

			m_Markers[modelNameId] = markers;
		}
	}
}