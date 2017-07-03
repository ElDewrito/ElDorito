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

	int s_SnapAngleIndex = 0;

	struct
	{
		uint32_t ObjectIndex;
		uint32_t StartTime;
		float Current;
		RealQuaternion StartRotation;
		RealQuaternion EndRotation;

	} s_RotationSnapState;

	void HandleInput();
}

namespace Forge
{
	void RotationSnap::Update(uint32_t playerIndex, uint32_t heldObjectIndex)
	{
		if (heldObjectIndex == -1)
			return;

		HandleInput();

		static auto& moduleForge = Modules::ModuleForge::Instance();
		const auto snapAngleDegrees = moduleForge.VarRotationSnap->ValueFloat;

		if (snapAngleDegrees < 1)
			return;

		if (heldObjectIndex == -1)
		{
			s_RotationSnapState.StartTime = Blam::Time::GetGameTicks();
			s_RotationSnapState.StartRotation = RealQuaternion();
			s_RotationSnapState.EndRotation = s_RotationSnapState.StartRotation;
			s_RotationSnapState.Current = 1.0f;
			s_RotationSnapState.ObjectIndex = DatumIndex::Null;
		}

		s_RotationSnapState.Current += Blam::Time::GetSecondsPerTick() / DURATION_SECONDS;
		if (s_RotationSnapState.Current > 1) s_RotationSnapState.Current = 1;

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

	void RotationSnap::RotateSnapped(const Blam::Math::RealVector3D& axis)
	{
		static auto& moduleForge = Modules::ModuleForge::Instance();
		const auto snapAngleDegrees = moduleForge.VarRotationSnap->ValueFloat;
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
}

namespace
{
	void SetSnapAngle(float angle)
	{
		static auto& moduleForge = Modules::ModuleForge::Instance();

		if (angle < 0 || angle >= 360)
			angle = 0;

		moduleForge.VarRotationSnap->ValueFloat = angle;

		wchar_t buff[256];
		if (angle > 0)
			swprintf_s(buff, 256, L"Rotation Snap: %.2f\n", angle);
		else
			swprintf_s(buff, 256, L"Rotation Snap: OFF");

		static auto PrintKillFeedText = (void(__cdecl *)(unsigned int hudIndex, wchar_t *text, int a3))(0x00A95920);
		PrintKillFeedText(0, buff, 0);
	}

	void HandleInput()
	{
		auto uiLeftAction = Blam::Input::GetActionState(Blam::Input::eGameActionUiLeft);
		auto uiRightAction = Blam::Input::GetActionState(Blam::Input::eGameActionUiRight);

		int snapAngleCount = sizeof(SNAP_ANGLES) / sizeof(SNAP_ANGLES[0]);

		if (uiLeftAction->Ticks == 1)
		{
			if (--s_SnapAngleIndex < 0)
				s_SnapAngleIndex = snapAngleCount - 1;

			uiLeftAction->Flags |= Blam::Input::eActionStateFlagsHandled;
			SetSnapAngle(SNAP_ANGLES[s_SnapAngleIndex]);
		}
		if (uiRightAction->Ticks == 1)
		{
			if (++s_SnapAngleIndex >= snapAngleCount)
				s_SnapAngleIndex = 0;

			uiRightAction->Flags |= Blam::Input::eActionStateFlagsHandled;
			SetSnapAngle(SNAP_ANGLES[s_SnapAngleIndex]);
		}

		if (uiRightAction->Ticks > 0 && uiRightAction->Msec > 150)
		{
			SetSnapAngle(SNAP_ANGLES[s_SnapAngleIndex = 0]);
		}
	}
}
