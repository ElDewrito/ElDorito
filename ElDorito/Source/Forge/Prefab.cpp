#include "Prefab.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "ForgeUtil.hpp"
#include "ObjectSet.hpp"
#include "Selection.hpp"
#include <fstream>
#include <chrono>

using namespace Blam;
using namespace Blam::Math;

namespace
{
	struct PrefabPlacement
	{
		uint32_t Flags;
		uint32_t TagIndex;
		RealVector3D Position;
		RealVector3D RightVector;
		RealVector3D UpVector;
		Blam::MapVariant::VariantProperties Properties;
	};
	
	bool ReadHeader(std::ifstream &stream, Forge::Prefabs::PrefabHeader &header);
}

namespace Forge::Prefabs
{
	bool Save(const std::string &author, std::string& name, const std::string& path)
	{
		std::ofstream fs(path, std::ios::binary);
		if (!fs.is_open())
			return false;

		auto& objectSet = Forge::Selection::GetSelection();

		PrefabHeader header = { 0 };
		header.Magic = 'prfb';
		header.DateCreated = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
		strncpy_s(header.Name, name.c_str(), sizeof(header.Name));
		strncpy_s(header.Author, author.c_str(), sizeof(header.Author));
		header.ObjectCount = objectSet.Count();

		fs.write((char*)&header, sizeof(header));

		auto mapv = Forge::GetMapVariant();

		auto playerHandle = Blam::Players::GetLocalPlayer(0);
		const auto& crosshairPoint = GetSandboxGlobals().CrosshairPoints[playerHandle.Index];

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

		PrefabHeader header;
		if (!::ReadHeader(fs, header))
		{
			PrintKillFeedText(0, L"Not a valid prefab file", 0);
			return false;
		}

		auto quota = Forge::CalculateObjectQuota();

		auto playerHandle = Blam::Players::GetLocalPlayer(0);
		const auto& crosshairPoint = GetSandboxGlobals().CrosshairPoints[playerHandle.Index];


		auto remainingObjects = quota.TotalObjectsAvailable - quota.TotalObjectsUsed;
		if (int(header.ObjectCount) > remainingObjects)
		{
			wchar_t buff[1024];
			swprintf_s(buff, L"Unable to load prefab: out of objects. (%d more objects needed)",
				(quota.TotalObjectsUsed + header.ObjectCount) - quota.TotalObjectsAvailable);
			PrintKillFeedText(0, buff, 0);
			return false;
		}

		auto& objectSet = Forge::Selection::GetSelection();
		objectSet.Clear();

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

	bool ReadHeader(const std::string &path, PrefabHeader &header)
	{
		std::ifstream fs(path, std::ios::binary);
		return fs.is_open() && ::ReadHeader(fs, header);
	}
}

namespace
{
	bool ReadHeader(std::ifstream &stream, Forge::Prefabs::PrefabHeader &header)
	{
		stream.read((char*)&header, sizeof(header));
		return !stream.fail() && header.Magic == 'prfb';
	}
}
