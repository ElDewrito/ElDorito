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

	uint32_t GetCurrentObjectIndex(uint32_t playerIndex)
	{
		uint32_t heldObjectIndex, crosshairObjectIndex;
		if (!Forge::GetEditorModeState(playerIndex, &heldObjectIndex, &crosshairObjectIndex))
			return -1;

		return heldObjectIndex != -1 ? heldObjectIndex : crosshairObjectIndex;
	}

	void Selection::SelectAll()
	{
		auto playerIndex = Blam::Players::GetLocalPlayer(0);

		auto currentObject = Blam::Objects::Get(GetCurrentObjectIndex(playerIndex));
		if (!currentObject)
			return;

		const auto mapv = GetMapVariant();

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (placement.ObjectIndex == -1 || placement.BudgetIndex == -1)
				continue;

			if (mapv->Budget[placement.BudgetIndex].TagIndex == currentObject->TagIndex)
				s_SelectedObjects.Add(placement.ObjectIndex);
		}
	}

	void Selection::SelectEverything()
	{
		const auto mapv = GetMapVariant();
		s_SelectedObjects.Clear();
		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (placement.PlacementFlags & 1 && placement.ObjectIndex != -1)
				s_SelectedObjects.Add(placement.ObjectIndex);
		}
	}

	void Selection::SelectAllMaterial()
	{
		auto playerIndex = Blam::Players::GetLocalPlayer(0);

		auto currentObject = Blam::Objects::Get(GetCurrentObjectIndex(playerIndex));
		if (!currentObject || currentObject->PlacementIndex == -1)
			return;

		const auto mapv = GetMapVariant();

		auto currentObjectPlacement = mapv->Placements[currentObject->PlacementIndex];

		for (auto i = 0; i < 640; i++)
		{
			auto &placement = mapv->Placements[i];
			if (!(placement.PlacementFlags & 1) || placement.ObjectIndex == -1)
				continue;

			if (IsReforgeObject(placement.ObjectIndex)
				&& placement.Properties.SharedStorage == currentObjectPlacement.Properties.SharedStorage)
			{
				s_SelectedObjects.Add(placement.ObjectIndex);
			}
		}
	}

	void Selection::DeselectAllOf()
	{
		auto playerIndex = Blam::Players::GetLocalPlayer(0);

		auto currentObject = Blam::Objects::Get(GetCurrentObjectIndex(playerIndex));
		if (!currentObject)
			return;

		const auto mapv = GetMapVariant();

		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (placement.ObjectIndex == -1 || placement.BudgetIndex == -1)
				continue;

			if (mapv->Budget[placement.BudgetIndex].TagIndex == currentObject->TagIndex)
				s_SelectedObjects.Remove(placement.ObjectIndex);
		}
	}

	void Selection::Invert()
	{
		ObjectSet newSelection;
		const auto mapv = GetMapVariant();
		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (placement.ObjectIndex != -1 && !s_SelectedObjects.Contains(placement.ObjectIndex))
			{
				newSelection.Add(placement.ObjectIndex);
			}
		}
		s_SelectedObjects = newSelection;
	}

	void Selection::Expand(float amount)
	{
		const auto mapv = GetMapVariant();

		RealVector3D sum { 0,0,0 };
		RealVector3D min {mapv->WorldBoundsXMax, mapv->WorldBoundsYMax, mapv->WorldBoundsZMax};
		RealVector3D max {mapv->WorldBoundsXMin, mapv->WorldBoundsYMin, mapv->WorldBoundsZMin};

		auto selectedObjectCount = 0;
		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (placement.ObjectIndex != -1 && s_SelectedObjects.Contains(placement.ObjectIndex))
			{
				sum.I += placement.Position.I;
				sum.J += placement.Position.J;
				sum.K += placement.Position.K;

				if (placement.Position.I < min.I) min.I = placement.Position.I;
				if (placement.Position.J < min.J) min.J = placement.Position.J;
				if (placement.Position.K < min.K) min.K = placement.Position.K;
				if (placement.Position.I > max.I) max.I = placement.Position.I;
				if (placement.Position.J > max.J) max.J = placement.Position.J;
				if (placement.Position.K > max.K) max.K = placement.Position.K;

				selectedObjectCount++;
			}
		}

		if (!selectedObjectCount)
			return;

		auto centroid = sum / float(selectedObjectCount);
		float radius = (max - min).Length2() * 0.5f + amount;
		auto radius2 = radius * radius;
	
		for (auto i = 0; i < 640; i++)
		{
			auto& placement = mapv->Placements[i];
			if (placement.ObjectIndex == -1 || s_SelectedObjects.Contains(placement.ObjectIndex))
				continue;

			if ((placement.Position - centroid).Length2() < radius2)
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

		auto playerHandle = Blam::Players::GetLocalPlayer(0);
		auto objectIndex = GetSandboxGlobals().CrosshairObjects[playerHandle.Index];

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

			Forge::DeleteObject(playerHandle.Index, i);
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

		auto playerHandle = Blam::Players::GetLocalPlayer(0);

		const auto& crosshairPoint = GetSandboxGlobals().CrosshairPoints[playerHandle.Index];
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

		uint32_t heldObjectIndex, crosshairObjectIndex;
		if (!Forge::GetEditorModeState(playerIndex, &heldObjectIndex, &crosshairObjectIndex))
			return;

		auto fireRight = GetActionState(eGameActionFireRight);

		if (heldObjectIndex != -1)
			return;

		if (!(fireRight->Flags & eActionStateFlagsHandled) && fireRight->Ticks > 0)
		{
			if (fireRight->Ticks == 1 && crosshairObjectIndex == -1)
				s_IsPaintSelecting = true;

			if (crosshairObjectIndex != -1 && (fireRight->Ticks == 1 || s_IsPaintSelecting))
			{
				auto& selection = Forge::Selection::GetSelection();
				if (selection.Contains(crosshairObjectIndex) && !s_IsPaintSelecting)
					selection.Remove(crosshairObjectIndex);
				else
					selection.Add(crosshairObjectIndex);
			}
		}
		else
		{
			s_IsPaintSelecting = false;
		}
	}
}
