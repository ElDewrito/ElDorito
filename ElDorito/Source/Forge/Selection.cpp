#include "Selection.hpp"
#include "ObjectSet.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Forge/ForgeUtil.hpp"
#include <stack>

using namespace Blam;
using namespace Blam::Math;

namespace
{
	Forge::ObjectSet s_SelectedObjects;
}

namespace Forge
{
	ObjectSet& Selection::GetSelection()
	{
		return s_SelectedObjects;
	}

	bool IsEmpty()
	{
		return !s_SelectedObjects.Any();
	}

	void Selection::SelectAll()
	{
		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		auto objectUnderCrosshair = Blam::Objects::Get(GetSandboxGlobals().CrosshairObjects[playerIndex.Index()]);
		if (!objectUnderCrosshair)
			return;

		const auto mapv = GetMapVariant();

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (placement.ObjectIndex == -1 || placement.BudgetIndex == -1)
				continue;

			if (mapv->Budget[placement.BudgetIndex].TagIndex == objectUnderCrosshair->TagIndex)
				s_SelectedObjects.Add(placement.ObjectIndex);
		}
	}

	void Selection::Clear()
	{
		s_SelectedObjects.Clear();
	}

	void Selection::Delete()
	{
		auto mapv = GetMapVariant();
		if (!mapv)
			return;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		auto objectIndex = GetSandboxGlobals().CrosshairObjects[playerIndex.Index()];

		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return;

		for (auto i = 0; i < 640; i++)
		{
			const auto& placement = mapv->Placements[i];
			if (!placement.InUse())
				continue;

			const auto& budget = mapv->Budget[placement.BudgetIndex];
			if (budget.TagIndex != object->TagIndex)
				continue;

			Forge::DeleteObject(playerIndex.Index(), i);
		}
	}

	bool Selection::Clone()
	{
		std::stack<uint32_t> cloneStack;

		auto mapv = GetMapVariant();

		for (auto i = 0; i < 640; i++)
		{
			const auto& placement = mapv->Placements[i];
			if (!placement.InUse() || placement.ObjectIndex == -1)
				continue;

			const auto& selection = Forge::Selection::GetSelection();
			if (selection.Contains(placement.ObjectIndex))
				cloneStack.push(placement.ObjectIndex);
		}

		if (cloneStack.empty())
			return false;

		auto referenceObject = Blam::Objects::Get(cloneStack.top());
		if (!referenceObject)
			return false;

		auto& selection = Forge::Selection::GetSelection();
		selection.Clear();

		auto playerIndex = Blam::Players::GetLocalPlayer(0);

		const auto& crosshairPoint = GetSandboxGlobals().CrosshairPoints[playerIndex.Index()];
		const auto& referencePoint = referenceObject->Position;

		while (!cloneStack.empty())
		{
			auto objectIndex = cloneStack.top();
			cloneStack.pop();

			auto object = Blam::Objects::Get(objectIndex);
			if (!object)
				continue;

			RealMatrix4x3 objectTransform;
			GetObjectTransformationMatrix(objectIndex, &objectTransform);

			MapVariant::VariantProperties* variantProperties = nullptr;
			if (object->PlacementIndex != -1)
				variantProperties = &mapv->Placements[object->PlacementIndex].Properties;

			auto displacement = crosshairPoint - objectTransform.Position;
			auto pos = objectTransform.Position + displacement - (referencePoint - objectTransform.Position);

			auto newObjectIndex = SpawnObject(mapv, object->TagIndex, 0, -1, &pos, &objectTransform.Forward,
				&objectTransform.Up, -1, -1, variantProperties, 0);

			if (newObjectIndex != -1)
				selection.Add(newObjectIndex);
		}

		return true;
	}

	void Selection::Update()
	{
		using namespace Blam::Input;

		static bool s_IsPaintSelecting = false;

		auto playerIndex = Blam::Players::GetLocalPlayer(0);
		auto objectIndexUnderCrosshair = Forge::GetSandboxGlobals().CrosshairObjects[playerIndex.Index()];

		if (GetActionState(eGameActionFireRight)->Ticks > 0)
		{
			if (GetActionState(eGameActionFireRight)->Ticks == 1 && objectIndexUnderCrosshair == -1)
				s_IsPaintSelecting = true;

			if (objectIndexUnderCrosshair != -1 &&
				(GetActionState(eGameActionFireRight)->Ticks == 1 || s_IsPaintSelecting))
			{
				auto& selection = Forge::Selection::GetSelection();
				if (selection.Contains(objectIndexUnderCrosshair) && !s_IsPaintSelecting)
					selection.Remove(objectIndexUnderCrosshair);
				else
					selection.Add(objectIndexUnderCrosshair);
			}
		}
		else
		{
			s_IsPaintSelecting = false;
		}
	}
}
