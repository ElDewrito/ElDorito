#include "PlayerScale.hpp"
#include "../Modules/ModulePlayer.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Blam/BlamPlayers.hpp"
#include "../Blam/Tags/TagBlock.hpp"
#include "../Blam/Tags/TagInstance.hpp"
#include "../Blam/BlamTime.hpp"
#include "../Blam/Math/RealMatrix4x3.hpp"
#include "../Blam/Tags/Items/DefinitionWeapon.hpp"
#include "../Blam/Tags/Objects/Biped.hpp"
#include "../Patch.hpp"

namespace
{
	using Blam::Math::RealVector3D;

	struct s_biped_physics;
	struct s_biped_physics_data1;
	struct ZoneShape;

	const auto kTransitionRate = 5.0f;
	const auto kTransitionMaxDelta = 4.5f;
	const auto kPhysicsModelUpdateRate = 0.5f;
	const float kPlayerScales[] =
	{
		0.1f,
		0.15f,
		0.25f,
		0.35f,
		0.5f,
		0.75f,
		1.0f,
		1.25f,
		1.5f,
		2.0f,
		3.0f,
		5.0,
		7.5,
		10.0,
		15.0,
		20.0,
		30.0
	};

	const auto kPlayerScaleCount = sizeof(kPlayerScales) / sizeof(kPlayerScales[0]);
	const float kMaxPlayerScale = kPlayerScales[kPlayerScaleCount - 1];

	auto kSpeedExp = 0.75f;
	auto kMeleeRangeExp = 0.8f;
	auto kMassExp = 1.5f;
	auto kGravityExp = 0.375f;
	auto kJumpVelocityExp = 0.56f;
	auto kAirborneAccelerationScale = 0.4f;
	auto kGroundAccelerationScale = 0.85f;

	void PlayerAuraHook();
	void RagdollHook();
	void *BipedPhysicsHook_4B10E0(s_biped_physics *bipedPhysics, int shapeIndex);
	void *BipedPhysicsHook_5A3467(s_biped_physics *bipedPhysics, int shapeIndex);
	void *BipedPhysicsHook_5E23BC(s_biped_physics *bipedPhysics, int shapeIndex);
	void *BipedPhysicsHook_B7156F(s_biped_physics *bipedPhysics, int shapeIndex);
	void BipedPhysicsHook_BAF4CF();
	void BipedCanStandUnobstructed_Teleporter_Hook();
	void PlayerSpawnLocationHook(uint32_t playerIndex, Blam::Math::RealVector3D *position, Blam::Math::RealVector3D *orientation, int *pStartingLocationIndex);
	bool PlayerZoneTestHook(uint32_t playerIndex, ZoneShape *zone);
	void BipedShapeHook(uint32_t objectIndex, Blam::Math::RealVector3D *outPosition, float *outHeight, float *outRadius);
	void MeleeRangeHook(uint32_t unitObjectIndex, uint32_t name, int a3, void *damageData);
	bool SprintCameraShakeHook(uint32_t playerIndex, float *outValue);
	void CollisionDamageHook();
	bool PlayerCollisionDamageHook(uint32_t bipedObjectIndex, uint32_t otherBipedObjectIndex);
	bool __fastcall WalkAnimationHook(void *thisptr, void *unused, uint32_t unitObjectIndex, Blam::Math::RealVector3D *a3, bool a4);
	void JumpVelocityScaleHook(uint32_t bipedObjectIndex);
	void BipedMovementPhysics(s_biped_physics_data1 *data, int a2, char a3, char a4, char a5, char a6, int a7, float sneak);
	void *__fastcall BipedCreateRigidBodyHook(void *thisptr, void *unused, s_biped_physics *bipedPhysics, int a4, int a5,
		uint32_t collisionFilter_, int shapeIndex, bool hasFriction, char a9);
	void PlayerPersonalGravityHook();
	void LandingDamageResponseHook(uint32_t unitObjectIndex, uint32_t a2, uint32_t damageEffectTagIndex, RealVector3D *a4, RealVector3D *a5, float a6, float responseScale, int a8);
	void DamageResponsePlayerEffectsHook(uint32_t uintObjectIndex, int localPlayerIndex, uint32_t damageResponseTagIndex, int a4, RealVector3D *a5, float a6, float scale, bool a8);
	bool ModelTargetTestHook(int a1, uint32_t objectIndex, int a3, char a4, char a5, char a6, int a7, uint8_t *target,
		int a9, int a10, Blam::Math::RealVector3D *a11, int a12, void *data);
	void ShieldDownParticleEffectHook(uint32_t effectDatumIndex, uint16_t eventIndex);
	void BipedPhysicsHook2(uint32_t objectIndex, int a2, void *a3);
	void BipedPhysicsHook1(int objectIndex);

	struct hkVector4 { float x, y, z, w; };
	struct hkCapsuleShape {
		void **vftable;
		uint16_t m_memSizeAndFlags;
		uint16_t m_referenceCount;
		uint32_t m_userData;
		uint32_t m_type;
		float m_radius;
		int field_14;
		int field_18;
		int field_1C;
		hkVector4 m_bottom;
		hkVector4 m_top;
	};

	struct PlayerState {
		bool IsValid;
		bool IsTransitioning;
		float DesiredScale;
		uint32_t UnitObjectIndex;
		uint32_t PhysicsUpdateDeltaTime;
		hkCapsuleShape PhysicsCapsules[2];
	};

	struct {
		bool IsValid;
		bool IsLandingDamage;
		PlayerState Players[16];
	} state;
}

namespace Patches::PlayerScale
{
	void ApplyAll()
	{
		Hook(0x14F3C0, PlayerAuraHook).Apply();
		Hook(0x2842B0, SprintCameraShakeHook, HookFlags::IsCall).Apply();
		Hook(0x790774, WalkAnimationHook, HookFlags::IsCall).Apply();
		// disable shield down particle effects when tiny
		Hook(0x1BA3D2, ShieldDownParticleEffectHook, HookFlags::IsCall).Apply();

		// physics/collision
		Hook(0x7D80D0, RagdollHook).Apply();
		Hook(0x0B10E0, BipedPhysicsHook_4B10E0, HookFlags::IsCall).Apply();
		Hook(0x1E23BC, BipedPhysicsHook_5E23BC, HookFlags::IsCall).Apply();
		Hook(0x77156F, BipedPhysicsHook_B7156F, HookFlags::IsCall).Apply();
		Hook(0x7AF4CF, BipedPhysicsHook_BAF4CF).Apply();
		Hook(0x1C967F, BipedCanStandUnobstructed_Teleporter_Hook).Apply();
		Hook(0x13CF37, PlayerSpawnLocationHook, HookFlags::IsCall).Apply();
		Hook(0x76E850, BipedShapeHook).Apply();
		Hook(0x7A10B0, PlayerZoneTestHook).Apply();
		Hook(0x7A5053, MeleeRangeHook, HookFlags::IsCall).Apply();
		Hook(0x78175B, MeleeRangeHook, HookFlags::IsCall).Apply();
		Hook(0x742774, MeleeRangeHook, HookFlags::IsCall).Apply();
		Hook(0x3636ED, CollisionDamageHook).Apply();
		Hook(0x1E227C, BipedCreateRigidBodyHook, HookFlags::IsCall).Apply();
		Hook(0x7AF707, BipedMovementPhysics, HookFlags::IsCall).Apply();
		Hook(0x7E276C, JumpVelocityScaleHook, HookFlags::IsCall).Apply();
		Hook(0x3629CC, PlayerCollisionDamageHook, HookFlags::IsCall).Apply();
		Hook(0x7AF500, PlayerPersonalGravityHook, HookFlags::IsCall).Apply();
		Hook(0x7E2335, LandingDamageResponseHook, HookFlags::IsCall).Apply();
		Hook(0x284EDF, DamageResponsePlayerEffectsHook, HookFlags::IsCall).Apply();
		Hook(0x284F21, DamageResponsePlayerEffectsHook, HookFlags::IsCall).Apply();
		// fix jump crouch/lunge issues
		Hook(0x72FF33, BipedPhysicsHook1, HookFlags::IsCall).Apply();
		Hook(0x7B18ED, BipedPhysicsHook2, HookFlags::IsCall).Apply();

		Hook(0x18BAAE, ModelTargetTestHook, HookFlags::IsCall).Apply();
		Hook(0x18C184, ModelTargetTestHook, HookFlags::IsCall).Apply();
	}

	void Tick()
	{
		const auto game_options_is_valid = (bool(*)())(0x00532650);
		if (!game_options_is_valid())
		{
			state.IsValid = false;
		}
		else
		{
			if (!state.IsValid)
			{
				memset(&state, 0, sizeof(state));
				for (auto i = 0; i < 16; i++)
				{
					auto &playerState = state.Players[i];
					playerState.UnitObjectIndex = -1;
					playerState.IsValid = false;
				}
				state.IsValid = true;
			}
		}
	}
}

namespace
{
	const auto objects_scripting_set_scale = (void(*)(uint32_t objectIndex, float scale, float a3))(0x00B965C0);
	const auto object_set_havok_flags = (int(*)(uint32_t objectIndex, uint32_t havokFlags))(0x005C7380);
	const auto object_get_origin = (void(*)(uint32_t objectIndex, Blam::Math::RealVector3D *position))(0x00B2E5A0);

	template <typename t_shape>
	struct s_havok_shape_data
	{
		uint32_t Name;
		uint8_t MaterialIndex;
		uint8_t field_0C;
		uint16_t GlobalMaterialIndex;
		float RelativeMassScale;
		float Friction;
		float Restituion;
		float Volume;
		float Mass;
		uint16_t OverallShapeIndex;
		uint8_t PhantomIndex;
		uint8_t InteractionUnknown;
		t_shape Shape;
	};
	using s_havok_capsule_shape = s_havok_shape_data<hkCapsuleShape>;

	struct s_biped_physics
	{
		int Flags;
		float HeightStanding;
		float HeightCrouching;
		float Radius;
		float Mass;
		uint32_t LivingMaterialName;
		uint32_t DeadMaterialName;
		uint16_t LivingMaterialGlobalIndex;
		uint16_t DeadMaterialIndex;
		Blam::Tags::TagBlock<uint8_t> DeadSphereShapes;
		Blam::Tags::TagBlock<s_havok_capsule_shape> PillShapes;
		Blam::Tags::TagBlock<uint8_t> SphereShapes;
		//....
	};

	struct s_biped_physics_data1
	{
		uint8_t field_0[0x18];
		uint32_t HavokComponentIndex;
		uint8_t field_1C[0x134];
		float Boost;
		Blam::Math::RealVector3D Velocity;
		float GroundAcceleration;
		float AirborneAcceleration;
		void *Shape;
		// .. 
	};

	struct ZoneShape {
		int Shape;
		int ShapeShader;
		float RadiusWidth;
		float Length;
		float Top;
		float Bottom;
		Blam::Math::RealMatrix4x3 Transform;
		float BoundingRadius;
	};


	float SmoothStep(float from, float to, float t, float max)
	{
		if (t > 1) t = 1;
		if (t < 0) t = 0;
		t = -2.0f * t * t * t + 3.0f * t * t;
		if (t > max) t = max;
		return (to * t + from * (1.0f - t));
	}

	void ObjectDeleteAttachedEffects(uint32_t objectIndex)
	{
		struct s_object_attachment {
			uint8_t type;
			uint8_t field_01;
			uint8_t field_02;
			uint8_t field_03;
			uint32_t handle;
		};

		const auto effect_delete = (void(*)(uint32_t effectDatumIndex))(0x005B6000);

		auto unitObject = Blam::Objects::Get(objectIndex);
		if (!unitObject)
			return;

		auto attachmentBlock = *(uint32_t*)((uint8_t*)unitObject + 0x15A);
		auto attachmentsCount = (attachmentBlock & 0xffff) >> 3;
		auto attachmentsOffset = (attachmentBlock >> 16) & 0xffff;
		if (attachmentsOffset == 0xff || attachmentsCount == 0)
			return;

		auto attachments = (s_object_attachment*)((uint8_t*)unitObject + attachmentsOffset);

		for (uint8_t i = 0; i < attachmentsCount; i++)
		{
			auto &attachment = attachments[i];
			if (attachment.type == 2 && attachment.handle != -1)
			{
				effect_delete(attachment.handle);
				attachment.type = 0xff;
				attachment.handle = -1;
			}
		}
	}

	void ObjectDeleteEffectAttachmentsRecursive(uint32_t unitObjectIndex)
	{
		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return;

		auto objectIndex = unitObject->FirstChild;
		do
		{
			ObjectDeleteEffectAttachmentsRecursive(objectIndex);
			auto childObject = Blam::Objects::Get(objectIndex);
			if (!childObject)
				continue;

			ObjectDeleteAttachedEffects(objectIndex);

			objectIndex = childObject->NextSibling;
		} while (objectIndex != Blam::DatumHandle::Null);
	}

	void UpdatePlayerSize(uint32_t playerIndex)
	{
		auto &playerState = state.Players[playerIndex & 0xf];
		if (!playerState.IsValid)
		{
			playerState.IsValid = true;
			playerState.IsTransitioning = false;
			playerState.UnitObjectIndex = -1;
			playerState.PhysicsUpdateDeltaTime = 0;
		}

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player)
			return;


		int playerScaleIndex = *((uint8_t*)player + 0x2DC2) - 2;

		auto desiredScale = 1.0f;
		if (playerScaleIndex >= 0 && playerScaleIndex < kPlayerScaleCount)
			desiredScale = kPlayerScales[playerScaleIndex];

		playerState.DesiredScale = desiredScale;

		auto unitObject = Blam::Objects::Get(player->SlaveUnit);
		if (!unitObject)
			return;

		auto currentScale = unitObject->Scale;

		if (playerState.UnitObjectIndex != player->SlaveUnit.Handle)
		{
			playerState.UnitObjectIndex = player->SlaveUnit;
			objects_scripting_set_scale(player->SlaveUnit, desiredScale, 0.0f);
			object_set_havok_flags(player->SlaveUnit, 0);
			playerState.IsTransitioning = true;
			return;
		}

		if (std::abs(desiredScale - currentScale) < 0.001f)
		{
			if (playerState.IsTransitioning)
			{
				playerState.IsTransitioning = false;
				objects_scripting_set_scale(player->SlaveUnit, desiredScale, 0.0f);
				object_set_havok_flags(player->SlaveUnit, 0);

				if (std::abs(1.0f - desiredScale) > 0.001f)
					ObjectDeleteEffectAttachmentsRecursive(player->SlaveUnit);
			}
			else
			{
				// for weapons
				objects_scripting_set_scale(player->SlaveUnit, desiredScale, 0.0f);
			}
		}
		else
		{
			if (!playerState.IsTransitioning)
			{
				playerState.IsTransitioning = true;
				ObjectDeleteEffectAttachmentsRecursive(player->SlaveUnit);
			}

			currentScale = SmoothStep(currentScale, desiredScale, Blam::Time::GetSecondsPerTick() * kTransitionRate, kTransitionMaxDelta);
			objects_scripting_set_scale(player->SlaveUnit, currentScale, 0.0f);

			if (Blam::Time::TicksToSeconds(playerState.PhysicsUpdateDeltaTime++) >= kPhysicsModelUpdateRate)
			{
				playerState.PhysicsUpdateDeltaTime = 0;
				object_set_havok_flags(player->SlaveUnit, 0);
			}
		}
	}

	bool UnitCanRagdoll(uint32_t unitObjectIndex)
	{
		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return false;

		// disable ragdolling when scale is outside a reasonable threshold to avoid ragdoll issues
		return unitObject->Scale >= 1.0f && unitObject->Scale < 3.0f;
	}

	__declspec(naked) void RagdollHook()
	{
		__asm
		{
			push ebp
			mov ebp, esp
			push[ebp + 0x8]
			call UnitCanRagdoll
			add esp, 4
			test al, al
			jz disabled
			sub esp, 0x34
			push 0x00BD80D6
			retn
			disabled:
			xor al, al
			mov esp, ebp
			pop ebp
			retn
		}
	}

	_declspec(naked) void PlayerAuraHook()
	{
		__asm
		{
			pushad
			push[ebp + 8]
			call UpdatePlayerSize
			add esp, 4
			popad
			mov eax, [ebp + 0x18]
			pop edi
			pop esi
			mov[eax], cl
			pop ebx
			mov esp, ebp
			pop ebp
			retn
		}
	}

	bool SprintCameraShakeHook(uint32_t playerIndex, float *outValue)
	{
		const auto player_get_sprinting = (bool(*)(uint32_t playerIndex, float *outSpeed))(0x0053B590);

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player || player->SlaveUnit == Blam::DatumHandle::Null)
			return false;
		auto unitObject = Blam::Objects::Get(player->SlaveUnit);
		if (!unitObject)
			return false;

		return player_get_sprinting(playerIndex, outValue) && unitObject->Scale >= 0.9f;
	}

	void *GetScaledShape(uint32_t bipedObjectIndex, s_biped_physics *bipedPhysicsData, int shapeIndex)
	{
		auto bipedObject = Blam::Objects::Get(bipedObjectIndex);
		if (!bipedObject)
			return nullptr;

		auto playerIndex = *(uint32_t*)((uint8_t*)bipedObject + 0x198);
		if (playerIndex == -1)
			return nullptr;

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player)
			return nullptr;

		if (shapeIndex < 0 || shapeIndex >= bipedPhysicsData->PillShapes.Count)
			return nullptr;

		auto &pillShape = bipedPhysicsData->PillShapes[shapeIndex];

		auto s = bipedObject->Scale;

		auto &playerState = state.Players[playerIndex & 0xF];
		if (!playerState.IsValid)
			return nullptr;

		auto &stateCapsule = playerState.PhysicsCapsules[shapeIndex];
		stateCapsule = pillShape.Shape;
		playerState.UnitObjectIndex = bipedObjectIndex;
		stateCapsule = pillShape.Shape;
		stateCapsule.m_radius = pillShape.Shape.m_radius*s;
		stateCapsule.m_top.z = (pillShape.Shape.m_top.z*s);
		stateCapsule.m_top.w = (pillShape.Shape.m_top.w*s);
		stateCapsule.m_bottom.z = (pillShape.Shape.m_bottom.z*s);
		stateCapsule.m_bottom.w = (pillShape.Shape.m_bottom.w*s);

		return &stateCapsule;
	}

	void *GetBipedPhysicsShape(uint32_t bipedObjectIndex, s_biped_physics *bipedPhysics, int shapeIndex)
	{
		if (bipedPhysics->PillShapes.Count)
		{
			auto bipedObject = Blam::Objects::Get(bipedObjectIndex);
			if (bipedObject)
			{
				auto playerIndex = *(uint32_t*)((uint8_t*)bipedObject + 0x198);
				if (playerIndex != -1)
				{
					auto &playerState = state.Players[playerIndex & 0xf];
					auto shape = GetScaledShape(bipedObjectIndex, bipedPhysics, shapeIndex);
					if (shape)
						return shape;
				}
			}

			return &bipedPhysics->PillShapes[shapeIndex].Shape;
		}
		else
		{
			auto data = (uint8_t*)bipedPhysics->SphereShapes.Elements + 0x70 * shapeIndex;
			if (bipedPhysics->Flags & 1) // centered at origin
				return  (data + 0x20);
			return (data + 0x40);
		}
	}

	void *BipedPhysicsHook_4B10E0(s_biped_physics *bipedPhysics, int shapeIndex)
	{
		uint32_t objectIndex;
		__asm
		{
			mov eax, [ebx + 4]
			mov objectIndex, eax
		};
		return GetBipedPhysicsShape(objectIndex, bipedPhysics, shapeIndex);
	}

	void *BipedPhysicsHook_5A3467(s_biped_physics *bipedPhysics, int shapeIndex)
	{
		uint32_t objectIndex;
		__asm
		{
			mov eax, [edx + 8]
			mov objectIndex, eax
		};
		return GetBipedPhysicsShape(objectIndex, bipedPhysics, shapeIndex);
	}

	void *BipedPhysicsHook_5E23BC(s_biped_physics *bipedPhysics, int shapeIndex)
	{
		uint32_t objectIndex;
		__asm
		{
			mov eax, [edx + 8]
			mov objectIndex, eax
		};
		return GetBipedPhysicsShape(objectIndex, bipedPhysics, shapeIndex);
	}

	void *BipedPhysicsHook_B7156F(s_biped_physics *bipedPhysics, int shapeIndex)
	{
		uint32_t objectIndex;
		__asm
		{
			mov eax, [esi + 8]
			mov objectIndex, eax
		};
		return GetBipedPhysicsShape(objectIndex, bipedPhysics, shapeIndex);
	}

	__declspec(naked) void BipedPhysicsHook_BAF4CF()
	{
		__asm
		{
			push[ebp + 0x8]
			call GetBipedPhysicsShape
			add esp, 4
			push 0x00BAF4D4
			retn
		}
	}

	bool BipedCanStandUnobstructed(uint32_t bipedObjectIndex, Blam::Math::RealVector3D *point, bool a3)
	{
		const auto game_is_campaign = (bool(*)())(0x00531A60);
		const auto game_is_survival = (bool(*)())(0x00531E20);
		const auto scenario_location_from_point = (void(*)(uint16_t *pClusterIndex, Blam::Math::RealVector3D *point))(0x004EA940);
		const auto sub_5C8010 = (bool(*)(void *shape, uint32_t havokComponentIndex, Blam::Math::RealMatrix4x3 *transform, uint32_t collisionFilterInfo))(0x5C8010);
		const auto sub_6D6AC0 = (bool(*)(uint32_t a1, uint32_t a2, Blam::Math::RealVector3D *a3, int a4, int a5, signed int *a6))(0x6D6AC0);

		if (!game_is_campaign() || game_is_survival())
		{
			auto bipedObject = Blam::Objects::Get(bipedObjectIndex);
			if (!bipedObject)
				return false;

			uint16_t scenarioLocation;
			scenario_location_from_point(&scenarioLocation, point);
			if (!a3 || (scenarioLocation & 0xff) != 0xff)
			{
				auto bipedDefinition = Blam::Tags::TagInstance(bipedObject->TagIndex).GetDefinition<uint8_t>();

				auto bipedPhysics = (s_biped_physics*)(bipedDefinition + 0x4D0);
				auto heightStanding = bipedPhysics->HeightStanding * bipedObject->Scale;

				auto standingShape = GetBipedPhysicsShape(bipedObjectIndex, bipedPhysics, 0);

				Blam::Math::RealMatrix4x3 matrix(1.0f, { 1.0f, 0, 0 }, { 0, 1.0f, 0 }, { 0, 0, 1.0f }, *point);
				matrix.Position.K += 0.2f; // slack;

				if (sub_5C8010(standingShape, -1, &matrix, 12))
					return false;

				auto headPosition = *point;
				headPosition.K = heightStanding * 0.5f + point->K;
				return !sub_6D6AC0(*(uint32_t*)0x471A858, *(uint32_t*)0x471A85C, &headPosition, -1, -1, 0);
			}
		}

		return false;
	}

	__declspec(naked) void BipedCanStandUnobstructed_Teleporter_Hook()
	{
		__asm
		{
			mov[esp], edi // don't need the tag index
			call BipedCanStandUnobstructed
			push 0x5C96A7
			retn
		}
	}

	void PlayerSpawnLocationHook(uint32_t playerIndex, Blam::Math::RealVector3D *position, Blam::Math::RealVector3D *orientation, int *pStartingLocationIndex)
	{
		using BipedDefinition = Blam::Tags::Objects::Biped;

		const auto player_get_spawn_location = (void(*)(uint32_t playerIndex, Blam::Math::RealVector3D *position, Blam::Math::RealVector3D *orientation, int *pStartingLocationIndex))(0x00539E30);
		const auto player_get_third_person_representation = (uint32_t(*)(uint32_t playerIndex, uint32_t *unitTagIndex, uint32_t *variant))(0x00539F70);
		
		uint32_t thirdPersonUnitTagIndex;
		player_get_third_person_representation(playerIndex, &thirdPersonUnitTagIndex, nullptr);
		if (thirdPersonUnitTagIndex != -1)
		{
			auto bipedDefinition = Blam::Tags::TagInstance(thirdPersonUnitTagIndex).GetDefinition<Blam::Tags::Objects::Biped>();
			if (bipedDefinition->PillShapes.Count == 2)
			{
				auto &playerState = state.Players[playerIndex & 0xf];
				if (playerState.IsValid)
				{
					auto s = playerState.DesiredScale;
					static BipedDefinition::PillShape shapes[2];
					for (auto i = 0; i < 2; i++)
					{
						shapes[i] = bipedDefinition->PillShapes[i];
						shapes[i].Radius *= s;
						shapes[i].Top.K *= s;
						shapes[i].TopRadius *= s;
						shapes[i].Bottom.K *= s;
						shapes[i].BottomRadius *= s;
					}

					auto oldPointer = bipedDefinition->PillShapes.Elements;
					bipedDefinition->PillShapes.Elements = shapes;
					player_get_spawn_location(playerIndex, position, orientation, pStartingLocationIndex);
					bipedDefinition->PillShapes.Elements = oldPointer;
					return;
				}
			}
		}

		player_get_spawn_location(playerIndex, position, orientation, pStartingLocationIndex);
	}

	bool PlayerZoneTestHook(uint32_t playerIndex, ZoneShape *zone)
	{
		const auto object_get_center_of_masss = (void(*)(unsigned __int32 objectIndex, Blam::Math::RealVector3D *center))(0x00B2DD90);
		const auto zone_test_point = (bool(*)(Blam::Math::RealVector3D *point, ZoneShape *zone))(0x00BA11F0);

		auto player = Blam::Players::GetPlayers().Get(playerIndex);
		if (!player || player->SlaveUnit == Blam::DatumHandle::Null)
			return false;
		auto bipedObjectHeader = Blam::Objects::GetObjects().Get(player->SlaveUnit);
		if (!bipedObjectHeader)
			return false;

		if (bipedObjectHeader->Type == Blam::Objects::eObjectTypeBiped)
		{
			auto bipedObject = bipedObjectHeader->Data;

			Blam::Math::RealVector3D position;
			float height, radius;
			BipedShapeHook(player->SlaveUnit, &position, &height, &radius);

			auto cameraHeight = *(float*)((uint8_t*)bipedObject + 0x5E8) * bipedObject->Scale;

			Blam::Math::RealVector3D points[2];
			points[0].I = position.I;
			points[0].J = position.J;
			points[0].K = (-radius) + position.K;
			auto headOffset = cameraHeight - radius;
			points[1].I = position.I;
			points[1].J = position.J;
			points[1].K = headOffset + position.K;

			return zone_test_point(&points[0], zone)
				|| zone_test_point(&points[1], zone);
		}
		else
		{
			Blam::Math::RealVector3D centerOfMass;
			object_get_center_of_masss(player->SlaveUnit, &centerOfMass);
			return zone_test_point(&centerOfMass, zone);
		}
	}

	void BipedShapeHook(uint32_t objectIndex, Blam::Math::RealVector3D *outPosition, float *outHeight, float *outRadius)
	{
		auto bipedObject = Blam::Objects::Get(objectIndex);
		if (!bipedObject)
			return;

		auto bipedDefinition = Blam::Tags::TagInstance(bipedObject->TagIndex).GetDefinition<uint8_t>();
		auto bipedPhysics = (s_biped_physics*)(bipedDefinition + 0x4D0);

		auto radius = bipedPhysics->Radius * bipedObject->Scale;
		auto heightStanding = bipedPhysics->HeightStanding * bipedObject->Scale;
		auto heightCrouching = bipedPhysics->HeightCrouching * bipedObject->Scale;

		object_get_origin(objectIndex, outPosition);

		if (!(bipedPhysics->Flags & 1))   // not centered at origin
		{
			outPosition->K = radius + outPosition->K;
		}

		if ((bipedPhysics->Flags >> 1) & 1)
		{
			*outHeight = 0;
			*outRadius = radius;
		}
		else
		{
			auto unitFlags = *(uint32_t*)((uint8_t*)bipedObject + 0x190);

			auto v6 = ((unitFlags >> 21) & 1) ? 1.0f : 0.0f;
			auto v7 = (((heightCrouching - heightStanding) * v6) + heightStanding) - (radius * 2.0f);

			if (v7 <= 0.0f)
				v7 = 0.0f;

			*outHeight = v7;
			*outRadius = radius;
		}
	}

	void MeleeRangeHook(uint32_t unitObjectIndex, uint32_t name, int a3, void *damageData)
	{
		using WeaponDefinition = Blam::Tags::Items::Weapon;

		const auto sub_B3C360 = (void(*)(int unitObjectIndex, uint32_t name, int a3, void *damageData))(0xB3C360);

		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (!unitObject)
			return;

		auto equippedWeapons = ((uint8_t*)unitObject + 0x2CA);
		if (equippedWeapons[0] != 0xff)
		{
			auto weapons = (uint32_t*)((uint8_t*)unitObject + 0x2D0);
			auto weaponObjectIndex = weapons[equippedWeapons[0]];
			auto weaponObject = Blam::Objects::Get(weaponObjectIndex);
			if (weaponObject)
			{
				auto weaponDefinition = Blam::Tags::TagInstance(weaponObject->TagIndex).GetDefinition<WeaponDefinition>();

				auto scale = std::pow(unitObject->Scale, kMeleeRangeExp);

				auto oldDepth = weaponDefinition->DamagePyramidDepth;
				weaponDefinition->DamagePyramidDepth *= scale;
				sub_B3C360(unitObjectIndex, name, a3, damageData);
				weaponDefinition->DamagePyramidDepth = oldDepth;
				return;
			}
		}

		sub_B3C360(unitObjectIndex, name, a3, damageData);
	}

	void ScaleCollisionDamage(uint32_t objectIndex, uint8_t *damageData)
	{
		auto bipedObject = Blam::Objects::Get(objectIndex);
		if (bipedObject)
		{
			*(float*)damageData *= bipedObject->Scale;
			*(float*)(damageData+4) *= bipedObject->Scale;
		}	
	}

	__declspec(naked) void CollisionDamageHook()
	{
		__asm
		{
			pushad
			push[ebp + 0xC]
			push[ebp + 0x8]
			call ScaleCollisionDamage
			add esp, 8
			popad
			pop edi
			pop esi
			mov al, cl
			pop ebx
			mov esp, ebp
			pop ebp
			ret
		}
	}

	void BipedMovementPhysics(s_biped_physics_data1 *data, int a2, char a3, char a4, char a5, char a6, int a7, float sneak)
	{
		const auto sub_7209E0 = (void(*)(void *data, int a2, char a3, char a4, char a5, char a6, int a7, float sneak))(0x7209E0);

		auto scale = 1.0f;

		auto havokComponents = *(Blam::DataArray<Blam::DatumBase>**)0x02446080;
		uint8_t *havokComponent;
		if (data->HavokComponentIndex != -1 && (havokComponent = (uint8_t*)havokComponents->Get(data->HavokComponentIndex)))
		{
			auto objectIndex = *(uint32_t*)((uint8_t*)havokComponent + 0x8);
			if (objectIndex != -1)
			{
				auto object = Blam::Objects::Get(objectIndex);
				if (object)
					scale = object->Scale;
			}
		}

		sub_7209E0(data, a2, a3, a4, a5, a6, a7, sneak);

		data->GroundAcceleration *= std::pow(scale, kSpeedExp * kGroundAccelerationScale);
		data->AirborneAcceleration *= std::pow(scale, kSpeedExp * kAirborneAccelerationScale);

		auto s = std::pow(scale, kSpeedExp);
		data->Velocity.I *= s;
		data->Velocity.J *= s;
		data->Velocity.K *= s;
	}

	bool __fastcall WalkAnimationHook(void *thisptr, void *unused, uint32_t unitObjectIndex, Blam::Math::RealVector3D *a3, bool a4)
	{
		const auto sub_6DEFF0 = (bool(__thiscall*)(void *thisptr, uint32_t unitObjectIndex, Blam::Math::RealVector3D *a3, bool a4))(0x6DEFF0);

		auto object = Blam::Objects::Get(unitObjectIndex);
		if (object)
		{
			auto scale = object->Scale;
			a3->I /= scale;
			a3->J /= scale;
			a3->K /= scale;
		}

		return sub_6DEFF0(thisptr, unitObjectIndex, a3, a4);
	}

	void JumpVelocityScaleHook(uint32_t bipedObjectIndex)
	{
		using BipedDefinition = Blam::Tags::Objects::Biped;
		const auto sub_BE1B30 = (char(*)(uint32_t bipedObjectIndex))(0x00BE1B30);

		auto bipedObject = Blam::Objects::Get(bipedObjectIndex);
		if (!bipedObject)
			return;

		auto bipedDefinition = Blam::Tags::TagInstance(bipedObject->TagIndex).GetDefinition<BipedDefinition>();
		auto oldJumpVelocity = bipedDefinition->JumpVelocity;
		bipedDefinition->JumpVelocity *= std::pow(bipedObject->Scale, kJumpVelocityExp);
		sub_BE1B30(bipedObjectIndex);
		bipedDefinition->JumpVelocity = oldJumpVelocity;
	}

	float __fastcall PlayerPersonalGravity(void *traits, uint32_t unitObjectIndex)
	{
		const auto c_player_movement_traits__get_personal_gravity = (float(__thiscall*)(void *traits))(0x005CC200);

		auto s = 1.0f;
		auto unitObject = Blam::Objects::Get(unitObjectIndex);
		if (unitObject)
			s = std::pow(unitObject->Scale, kGravityExp);
			
		return c_player_movement_traits__get_personal_gravity(traits) * s;
	}

	__declspec(naked) void PlayerPersonalGravityHook()
	{
		__asm
		{
			mov edx, ebx
			call PlayerPersonalGravity
			retn
		}
	}

	void *__fastcall BipedCreateRigidBodyHook(void *thisptr, void *unused, s_biped_physics *bipedPhysics, int a4, int a5,
		uint32_t collisionFilter_, int shapeIndex, bool hasFriction, char a9)
	{
		const auto BipedCreateRigidBody = (void *(__thiscall *)(void *thisptr, s_biped_physics *bipedPhysics, int a4, int a5,
			uint32_t collisionFilter_, int shapeIndex, bool hasFriction, char a9))(0x005E22C0);

		auto object = Blam::Objects::Get(*(uint32_t*)((uint8_t*)thisptr + 0x8));
		if (object)
		{
			auto oldMass = bipedPhysics->Mass;
			bipedPhysics->Mass *= std::pow(object->Scale, kMassExp);
			auto ret = BipedCreateRigidBody(thisptr, bipedPhysics, a4, a5, collisionFilter_, shapeIndex, hasFriction, a9);
			bipedPhysics->Mass = oldMass;
			return ret;
		}

		return BipedCreateRigidBody(thisptr, bipedPhysics, a4, a5, collisionFilter_, shapeIndex, hasFriction, a9);
	}

	bool PlayerCollisionDamageHook(uint32_t bipedObjectIndex, uint32_t otherBipedObjectIndex)
	{
		const auto game_is_multiplayer = (bool(*))(0x00531AF0);
		const auto multiplayer_object_should_cause_collision_damage = (bool(*)(uint32_t objectIndex, uint32_t otherObjectIndex))(0x00763710);

		if (game_is_multiplayer)
		{
			auto objectHeaderA = Blam::Objects::GetObjects().Get(bipedObjectIndex);
			auto objectHeaderB = Blam::Objects::GetObjects().Get(otherBipedObjectIndex);
			if (objectHeaderA && objectHeaderB
				&& objectHeaderA->Type == Blam::Objects::eObjectTypeBiped
				&& objectHeaderB->Type == Blam::Objects::eObjectTypeBiped)
			{
				auto minScale = objectHeaderA->Data->Scale < objectHeaderB->Data->Scale ? objectHeaderA->Data->Scale : objectHeaderB->Data->Scale;
				auto maxScale = objectHeaderA->Data->Scale > objectHeaderB->Data->Scale ? objectHeaderA->Data->Scale : objectHeaderB->Data->Scale;
				if (maxScale < 0.001f)
					maxScale = 0.001f;

				auto d = minScale / maxScale;

				// allow larger players to damage smaller players by standing on them
				if (d < 0.5)
					return true;
			}
		}
		return multiplayer_object_should_cause_collision_damage(bipedObjectIndex, otherBipedObjectIndex);
	}

	void LandingDamageResponseHook(uint32_t unitObjectIndex, uint32_t a2, uint32_t damageEffectTagIndex, RealVector3D *a4, RealVector3D *a5, float a6, float responseScale, int a8)
	{
		const auto LandingDamageResponse = (void(*)(uint32_t bipedObjectIndex, uint32_t a2, 
			uint32_t damageEffectTagIndex, RealVector3D *a4, RealVector3D *a5, float a6, float responseScale, int a8))(0x684DF0);

		state.IsLandingDamage = true;
		LandingDamageResponse(unitObjectIndex, a2, damageEffectTagIndex, a4, a5, a6, responseScale, a8);
		state.IsLandingDamage = false;
	}

	void DamageResponsePlayerEffectsHook(uint32_t uintObjectIndex, int localPlayerIndex, uint32_t damageResponseTagIndex, int a4, RealVector3D *a5, float a6, float scale, bool a8)
	{
		const auto sub_684F30 = (void(*)(uint32_t uintObjectIndex, int localPlayerIndex, uint32_t damageResponseTagIndex, int a4, RealVector3D *a5, float a6, float scale, bool a8))(0x684F30);

		auto unitObject = Blam::Objects::Get(uintObjectIndex);
		if (unitObject)
		{
			if(state.IsLandingDamage)
			{
				scale *= unitObject->Scale;
			}
			else if (unitObject->Scale < 1.0f)
			{
				scale *= std::pow(unitObject->Scale, 0.4f);
			}
		}

		return sub_684F30(uintObjectIndex, localPlayerIndex, damageResponseTagIndex, a4, a5, a6, scale, a8);
	}

	bool JumpCrouchFix1Hook(uint32_t bipedObjectIndex, float *deltaK)
	{
		const auto sub_B746B0 = (bool(*)(uint32_t bipedObjectIndex, float *deltaK))(0xBAE130);

		auto unitObject = Blam::Objects::Get(bipedObjectIndex);
		if (unitObject)
		{
			if (std::abs(1.0f - unitObject->Scale) > 0.25f)
				return false;
		}

		return sub_B746B0(bipedObjectIndex, deltaK);
	}

	bool JumpCrouchFix2Hook(uint32_t bipedObjectIndex)
	{
		const auto sub_B746B0 = (bool(*)(uint32_t bipedObjectIndex))(0xB746B0);

		auto unitObject = Blam::Objects::Get(bipedObjectIndex);
		if (unitObject)
		{
			if (std::abs(1.0f - unitObject->Scale) > 0.25f)
				return false;
		}

		return sub_B746B0(bipedObjectIndex);
	}

	bool ModelTargetTestHook(int a1, uint32_t objectIndex, int a3, char a4, char a5, char a6, int a7, uint8_t *target,
		int a9, int a10, Blam::Math::RealVector3D *a11, int a12, void *data)
	{
		const auto model_target_test = (bool(*)(int a1, uint32_t objectIndex, int a3, char a4, char a5, char a6, int a7, uint8_t *target,
			int a9, int a10, Blam::Math::RealVector3D *a11, int a12, void *data))(0x0058AF40);

		auto object = Blam::Objects::Get(objectIndex);
		if (!object)
			return false;

		auto &targetRadius = *(float*)(target + 0x8);
		auto oldRadius = targetRadius;
		targetRadius *= object->Scale;
		auto result = model_target_test(a1, objectIndex, a3, a4, a5, a6, a7, target, a9, a10, a11, a12, data);
		targetRadius = oldRadius;

		return result;
	}

	void ShieldDownParticleEffectHook(uint32_t effectDatumIndex, uint16_t eventIndex)
	{
		const auto ShieldDownParticleEffect = (void(*)(uint32_t effectDatumIndex, uint16_t eventIndex))(0x005BDF40);

		struct s_effect_datum : Blam::DatumBase
		{
			uint16_t field_2;
			uint32_t field_4;
			uint32_t tag_index;
			uint32_t field_C;
			uint32_t node;
			uint32_t field_14;
			uint32_t primary_scale;
			uint32_t field_1C;
			uint32_t field_20;
			Blam::Math::RealVector3D field_24;
			uint32_t ObjectIndex;
			//...
		};

		auto effects = *(Blam::DataArray<s_effect_datum>**)ElDorito::GetMainTls(0xA0);
		auto effect = effects->Get(effectDatumIndex);
		if (effect && effect->primary_scale == 0x1CDC) // shield_down
		{
			auto object = Blam::Objects::Get(effect->ObjectIndex);
			if (object->Scale < 0.75f)
				return;
		}

		ShieldDownParticleEffect(effectDatumIndex, eventIndex);
	}

	template <typename TCallback>
	void ScaledBipedPhysicsForCall(uint32_t tagIndex, float scale, TCallback callback)
	{
		auto bipedDefinition = Blam::Tags::TagInstance(tagIndex).GetDefinition<Blam::Tags::Objects::Biped>();
		if (!bipedDefinition)
			return;

		auto oldHeightStanding = bipedDefinition->HeightStanding;
		auto oldHeightCrouching = bipedDefinition->HeightCrouching;

		bipedDefinition->HeightStanding *= scale;
		bipedDefinition->HeightCrouching *= scale;

		callback();

		bipedDefinition->HeightStanding = oldHeightStanding;
		bipedDefinition->HeightCrouching = oldHeightCrouching;
	}

	void BipedPhysicsHook2(uint32_t objectIndex, int a2, void *a3)
	{
		static const auto sub_BB01B0 = (void(*)(uint32_t objectIndex, int a2, void *a3))(0xBB01B0);

		auto object = Blam::Objects::Get(objectIndex);
		if (object)
		{
			return ScaledBipedPhysicsForCall(object->TagIndex, object->Scale, [=]() {
				sub_BB01B0(objectIndex, a2, a3);
			});
		}

		sub_BB01B0(objectIndex, a2, a3);
	}

	void BipedPhysicsHook1(int objectIndex)
	{
		const auto sub_B73C00 = (void(*)(uint32_t bipedObjectIndex))(0xB73C00);

		auto objectHeader = Blam::Objects::GetObjects().Get(objectIndex);
		if (objectHeader && objectHeader->Type == 0)
		{
			return ScaledBipedPhysicsForCall(objectHeader->Data->TagIndex, objectHeader->Data->Scale, [=]() {
				sub_B73C00(objectIndex);
			});
		}

		sub_B73C00(objectIndex);
	}
}
