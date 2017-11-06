#include "Prefab.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../ElDorito.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "ForgeUtil.hpp"
#include "ObjectSet.hpp"
#include "Selection.hpp"
#include <fstream>
#include <chrono>

using namespace Blam;
using namespace Blam::Math;

namespace
{
	struct PrefabHeader
	{
		uint32_t Magic;
		uint16_t Flags;
		uint16_t Version;
		uint64_t DateCreated;
		char Name[16];
		char Author[16];
		uint16_t ObjectCount;
		uint16_t _Padding;
	};

	struct PrefabPlacement
	{
		uint32_t Flags;
		uint32_t TagIndex;
		RealVector3D Position;
		RealVector3D RightVector;
		RealVector3D UpVector;
		Blam::MapVariant::VariantProperties Properties;
	};
}

namespace Forge::Prefabs
{
	bool Save(const std::string& name, const std::string& path)
	{
		std::ofstream fs(path, std::ios::binary);
		if (!fs.is_open())
			return false;

		auto& objectSet = Forge::Selection::GetSelection();

		auto& modulePlayer = Modules::ModulePlayer::Instance();
		auto& playerName = modulePlayer.VarPlayerName->ValueString;

		PrefabHeader header = { 0 };
		header.Magic = 'prfb';
		header.DateCreated = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
		strncpy(header.Name, name.c_str(), sizeof(header.Name));
		strncpy(header.Author, playerName.c_str(), sizeof(header.Author));
		header.ObjectCount = objectSet.Count();

		fs.write((char*)&header, sizeof(header));

		auto mapv = Forge::GetMapVariant();

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		const auto& crosshairPoint = GetSandboxGlobals().CrosshairPoints[playerIndex.Index()];

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (!placement.InUse() || !objectSet.Contains(placement.ObjectIndex))
				continue;

			PrefabPlacement prefabPlacement = { 0 };
			prefabPlacement.TagIndex = mapv->Budget[placement.BudgetIndex].TagIndex;
			prefabPlacement.Position = placement.Position - GetSandboxGlobals().CrosshairPoints[0];
			prefabPlacement.RightVector = placement.RightVector;
			prefabPlacement.UpVector = placement.UpVector;
			prefabPlacement.Properties = placement.Properties;

			fs.write((char*)&prefabPlacement, sizeof(prefabPlacement));
			if (fs.fail())
				return false;
		}

		return true;
	}

	bool Load(const std::string& path)
	{
		std::ifstream fs(path, std::ios::binary);
		if (!fs.is_open())
			return false;

		auto mapv = Forge::GetMapVariant();
		if (!mapv)
			return false;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		const auto& crosshairPoint = GetSandboxGlobals().CrosshairPoints[playerIndex.Index()];

		auto& objectSet = Forge::Selection::GetSelection();
		objectSet.Clear();

		PrefabHeader header;
		fs.read((char*)&header, sizeof(header));

		for (auto i = 0; i < header.ObjectCount; i++)
		{
			PrefabPlacement prefabPlacement;
			fs.read(reinterpret_cast<char*>(&prefabPlacement), sizeof(prefabPlacement));
			if (fs.eof())
				break;

			if (fs.fail())
				return false;

			auto position = crosshairPoint + prefabPlacement.Position;

			auto newObjectIndex = SpawnObject(mapv, prefabPlacement.TagIndex, 0, -1, &position,
				&prefabPlacement.RightVector, &prefabPlacement.UpVector, -1, -1, &prefabPlacement.Properties, 0);

			if (newObjectIndex == -1)
				return false;

			objectSet.Add(newObjectIndex);
		}

		return true;
	}
}
