#include "RotationSnap.hpp"
#include "../Blam/BlamData.hpp"
#include "../Blam/Math/RealQuaternion.hpp"
#include "../Blam/Math/RealMatrix4x3.hpp"
#include "../Blam/Math/MathUtil.hpp"
#include "../Blam/BlamInput.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Modules/ModuleForge.hpp"
#include "ForgeUtil.hpp"

using namespace Blam;
using namespace Blam::Math;

namespace
{
	const auto DURATION_SECONDS = 0.3f;
	const float SNAP_ANGLES[] = { 0, 3, 5, 15, 30, 45, 90 };

	struct
	{
		uint32_t ObjectIndex;
		uint32_t StartTime;
		float Current;
		RealQuaternion StartRotation;
		RealQuaternion EndRotation;
		bool IsScripted;

	} s_RotationSnapState;

	void HandleInput();
}

namespace Forge::RotationSnap
{
	void Update(uint32_t playerIndex, uint32_t heldObjectIndex)
	{
		if (heldObjectIndex == -1)
			return;

		HandleInput();

		static auto& moduleForge = Modules::ModuleForge::Instance();
		auto currentSnapIndex = moduleForge.VarRotationSnap->ValueInt;
		if (currentSnapIndex < 0 || currentSnapIndex > 6)
			currentSnapIndex = 0;

		const auto snapAngleDegrees = SNAP_ANGLES[currentSnapIndex];

		if (snapAngleDegrees < 1 && !s_RotationSnapState.IsScripted)
			return;

		if (heldObjectIndex == -1)
		{
			s_RotationSnapState.StartTime = Blam::Time::GetGameTicks();
			s_RotationSnapState.StartRotation = RealQuaternion();
			s_RotationSnapState.EndRotation = s_RotationSnapState.StartRotation;
			s_RotationSnapState.Current = 1.0f;
			s_RotationSnapState.ObjectIndex = DatumHandle::Null;
		}

		s_RotationSnapState.Current += Blam::Time::GetSecondsPerTick() / DURATION_SECONDS;
		if (s_RotationSnapState.Current >= 1)
		{
			s_RotationSnapState.IsScripted = false;
			s_RotationSnapState.Current = 1;
		}

		if (s_RotationSnapState.ObjectIndex != heldObjectIndex)
		{
			RealMatrix4x3 objectTransform;

			GetObjectTransformationMatrix(heldObjectIndex, &objectTransform);

			s_RotationSnapState.StartRotation = RealQuaternion::CreateFromRotationMatrix(objectTransform);
			s_RotationSnapState.EndRotation = s_RotationSnapState.StartRotation;
			s_RotationSnapState.Current = 1.0f;
		}

		RealVector3D rightVec(1, 0, 0), upVec(0, 0, 1);

		auto interpolatedRotation = RealQuaternion::Slerp(s_RotationSnapState.StartRotation, s_RotationSnapState.EndRotation, s_RotationSnapState.Current);

		rightVec = RealVector3D::Normalize(RealVector3D::Transform(rightVec, interpolatedRotation));
		upVec = RealVector3D::Normalize(RealVector3D::Transform(upVec, interpolatedRotation));

		static auto Object_Transform = (void(__cdecl*)(bool a1, uint32_t objectIndex, RealVector3D *position, RealVector3D *right, RealVector3D *up))(0x0059E340);
		Object_Transform(0, heldObjectIndex, nullptr, &rightVec, &upVec);

		s_RotationSnapState.ObjectIndex = heldObjectIndex;
	}

	void RotateSnapped(const Blam::Math::RealVector3D& axis)
	{
		static auto& moduleForge = Modules::ModuleForge::Instance();
		auto currentSnap = moduleForge.VarRotationSnap->ValueInt;
		const auto snapAngleDegrees = SNAP_ANGLES[currentSnap];
		const auto snapAngleRadians = snapAngleDegrees / 180.0f * Blam::Math::PI;

		if (s_RotationSnapState.Current < 1)
			return;

		RealMatrix4x3 objectTransform;
		GetObjectTransformationMatrix(s_RotationSnapState.ObjectIndex, &objectTransform);

		auto rotation = RealQuaternion::CreateFromAxisAngle(axis, snapAngleRadians);
		s_RotationSnapState.StartTime = Blam::Time::GetGameTicks();
		s_RotationSnapState.StartRotation = RealQuaternion::Normalize(RealQuaternion::CreateFromRotationMatrix(objectTransform));
		s_RotationSnapState.EndRotation = RealQuaternion::Normalize(rotation * s_RotationSnapState.StartRotation);
		s_RotationSnapState.Current = 0;
	}

	void RotateToScripted(uint32_t objectIndex, const Blam::Math::RealQuaternion& rotation)
	{
		RealMatrix4x3 objectTransform;
		GetObjectTransformationMatrix(s_RotationSnapState.ObjectIndex, &objectTransform);

		s_RotationSnapState.ObjectIndex = objectIndex;
		s_RotationSnapState.IsScripted = true;
		s_RotationSnapState.StartTime = Blam::Time::GetGameTicks();
		s_RotationSnapState.StartRotation = RealQuaternion::Normalize(RealQuaternion::CreateFromRotationMatrix(objectTransform));
		s_RotationSnapState.EndRotation = rotation;
		s_RotationSnapState.Current = 0;
	}
}

namespace
{
	void SetSnap(int value)
	{
		static auto& moduleForge = Modules::ModuleForge::Instance();

		if (value < 0) value = 0;
		if (value > 6) value = 6;

		std::string prev;
		Modules::CommandMap::Instance().SetVariable(moduleForge.VarRotationSnap, std::to_string(value), prev);

		wchar_t buff[256];
		if (value > 0)
			swprintf_s(buff, 256, L"Rotation Snap: %.2f\n", SNAP_ANGLES[value]);
		else
			swprintf_s(buff, 256, L"Rotation Snap: OFF");

		static auto PrintKillFeedText = (void(__cdecl *)(unsigned int hudIndex, wchar_t *text, int a3))(0x00A95920);
		PrintKillFeedText(0, buff, 0);
	}

	void HandleInput()
	{
		using namespace Blam::Input;

		static auto& moduleForge = Modules::ModuleForge::Instance();

		auto uiLeftAction = GetActionState(eGameActionUiLeft);
		auto uiRightAction = GetActionState(eGameActionUiRight);

		int snapAngleCount = sizeof(SNAP_ANGLES) / sizeof(SNAP_ANGLES[0]);
		auto currentSnap = static_cast<int>(moduleForge.VarRotationSnap->ValueInt);

		if (!(uiLeftAction->Flags & eActionStateFlagsHandled) && uiLeftAction->Ticks == 1)
		{
			if (--currentSnap < 0)
				currentSnap = snapAngleCount - 1;

			uiLeftAction->Flags |= eActionStateFlagsHandled;
			SetSnap(currentSnap);
		}
		if (!(uiRightAction->Flags & eActionStateFlagsHandled) &&  uiRightAction->Ticks == 1)
		{
			if (++currentSnap >= snapAngleCount)
				currentSnap = 0;

			uiRightAction->Flags |= eActionStateFlagsHandled;
			SetSnap(currentSnap);
		}
	}
}
