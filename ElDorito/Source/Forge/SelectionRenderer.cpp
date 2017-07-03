#include "SelectionRenderer.hpp"
#include "../Blam/Math/RealMatrix4x3.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Forge/ForgeUtil.hpp"
#include "../Forge/Selection.hpp"
#include "../Forge/ObjectSet.hpp"
#include "../Patch.hpp"
#include <cstdint>

using namespace Blam::Math;

namespace
{
	void SpecialWeaponHUDHook(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objectsInCluster, int16_t objectcount, BYTE* activeSpecialChudTypes);

	bool s_Enabled = false;
}

namespace Forge
{
	void SelectionRenderer::Initialize()
	{
		Hook(0x62E760, SpecialWeaponHUDHook, HookFlags::IsCall).Apply();
	}

	void SelectionRenderer::Update()
	{

	}

	void SelectionRenderer::SetEnabled(bool enabled)
	{
		s_Enabled = enabled;
	}
}

namespace
{
	void SpecialWeaponHUDHook(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objectsInCluster, int16_t objectcount, BYTE* activeSpecialChudTypes)
	{
		static auto sub_A2CAA0 = (void(__cdecl*)(int a1, uint32_t unitObjectIndex, int a3, uint32_t* objects, int16_t objectCount, BYTE *result))(0xA2CAA0);

		static auto sub_686FD0 = (void(__cdecl*)())(0x686FD0);
		static auto sub_A78230 = (void(__cdecl*)())(0xA78230);
		static auto sub_A53160 = (void(__cdecl*)(int a1, int a2, char a3, char a4))(0xA53160);
		static auto sub_A7A510 = (void(__cdecl*)(int a1))(0xA7A510);
		static auto sub_A242E0 = (unsigned __int8(__cdecl*)(int a1, int a2))(0xA242E0);
		static auto sub_A48E40 = (int(__cdecl*)(int a1, int a2, int a3))(0xA48E40);
		static auto sub_A78F00 = (int(__cdecl*)(int a1, int a2))(0xA78F00);
		static auto sub_A781F0 = (int(*)())(0xA781F0);
		static auto sub_686DE0 = (int(*)())(0x686DE0);

		sub_A2CAA0(a1, unitObjectIndex, a3, objectsInCluster, objectcount, activeSpecialChudTypes);

		if (!s_Enabled)
			return;

		auto mapv = Forge::GetMapVariant();
		if (!mapv)
			return;

		const auto& selection = Forge::Selection::GetSelection();
		if (!selection.Any())
			return;

		for (auto i = 0; i < mapv->UsedPlacementsCount; i++)
		{
			auto& placement = mapv->Placements[i];

			if (selection.Contains(placement.ObjectIndex))
			{
				const int specialHudType = 6;
				activeSpecialChudTypes[specialHudType] = 1;

				sub_686FD0();
				sub_A78230();
				sub_A53160(placement.ObjectIndex, a1, 0, 1);
				sub_A7A510(0);
				sub_A242E0(
					18,
					(unsigned __int8)(signed int)floor((float)((float)(signed int)specialHudType * 255.0) * 0.125));
				sub_A48E40(2, 23, -1);
				sub_A78F00(0, 1);
				sub_A48E40(1, 0, 0);
				sub_A48E40(2, 0, 0);
				sub_A781F0();
				sub_686DE0();
			}
		}
	}
}
