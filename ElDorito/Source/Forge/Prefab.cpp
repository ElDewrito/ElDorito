#include "Prefab.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../ElDorito.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "ForgeUtil.hpp"
#include "ObjectSet.hpp"
#include <fstream>
#include <chrono>

namespace
{
	using namespace Blam;
	using namespace Blam::Math;

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

namespace Forge
{
	namespace Prefabs
	{
		bool Save(const ObjectSet& objectSet, const std::string& name, const std::string& path)
		{
			std::ofstream fs(path, std::ios::binary);
			if (!fs.is_open())
				return false;

			auto& modulePlayer = Modules::ModulePlayer::Instance();
			auto& playerName = modulePlayer.VarPlayerName->ValueString;

			PrefabHeader header = { 0 };
			header.Magic = 'prfb';
			header.DateCreated = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
			strcpy_s(header.Name, 16, name.c_str());
			strcpy_s(header.Author, 16, playerName.c_str());

			header.ObjectCount = objectSet.Count();

			fs.write((char*)&header, sizeof(header));

			auto mapv = Forge::GetMapVariant();
			const auto& crosshairPoint = Pointer(ElDorito::GetMainTls(0x48))[0](0xE21C).Read<RealVector3D>();

			for (auto i = 0; i < 640; i++)
			{
				auto& placement = mapv->Placements[i];
				if (!(placement.PlacementFlags & 1) || !objectSet.Contains(placement.ObjectIndex))
					continue;

				auto p = placement.Position - crosshairPoint;

				PrefabPlacement prefabPlacement = { 0 };
				prefabPlacement.TagIndex = mapv->Budget[placement.BudgetIndex].TagIndex;
				prefabPlacement.Position = p;
				prefabPlacement.RightVector = placement.RightVector;
				prefabPlacement.UpVector = placement.UpVector;
				prefabPlacement.Properties = placement.Properties;

				fs.write((char*)&prefabPlacement, sizeof(prefabPlacement));

				if (fs.fail())
					return false;
			}

			return true;
		}

		bool Load(ObjectSet& objectSet, const std::string& path)
		{
			std::ifstream fs(path, std::ios::binary);
			if (!fs.is_open())
				return false;

			PrefabHeader header;
			fs.read((char*)&header, sizeof(header));

			auto mapv = Forge::GetMapVariant();
			if (!mapv)
				return false;

			const auto& crosshairPoint = Pointer(ElDorito::GetMainTls(0x48))[0](0xE21C).Read<RealVector3D>();

			auto& objects = Blam::Objects::GetObjects();

			objectSet.Clear();

			for (auto i = 0; i < header.ObjectCount; i++)
			{
				PrefabPlacement prefabPlacement;
				fs.read((char*)&prefabPlacement, sizeof(prefabPlacement));
				if (fs.eof())
					break;

				if (fs.fail())
					return false;

				auto SpawnObject = (uint32_t(__thiscall *)(Blam::MapVariant* thisptr, uint32_t tagIndex, int a3, int16_t placementIndex, const RealVector3D *position,
					const RealVector3D *rightVec, const RealVector3D *upVec, int16_t scnrPlacementBlockIndex, int objectType,
					Blam::MapVariant::VariantProperties* variantProperties, uint16_t placementFlags))(0x00582110);

				auto p = crosshairPoint + prefabPlacement.Position;

				auto newObjectIndex = SpawnObject(mapv, prefabPlacement.TagIndex, 0, -1, &p,
					&prefabPlacement.RightVector, &prefabPlacement.UpVector, -1, -1, &prefabPlacement.Properties, 0);

				if (newObjectIndex == -1)
					return false;

				objectSet.Add(newObjectIndex);
			}

			return true;
		}
	}
}
