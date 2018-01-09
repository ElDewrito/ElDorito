#include "Simulation.hpp"
#include "../Blam/Math/RealVector3D.hpp"
#include "../Blam/BlamNetwork.hpp"
#include "../Blam/BlamTypes.hpp"
#include "../Patches/Network.hpp"
#include "../Blam/Tags/Objects/Object.hpp"
#include "../Blam/BlamObjects.hpp"
#include "../Patch.hpp"

namespace
{
	using namespace Blam::Math;

	struct GenericEntitySimulationData;

	bool __cdecl c_simulation_generic_entity__serialize2_hook(char a1, int a6, int a3, DWORD *a4, GenericEntitySimulationData *data, Blam::BitStream *stream, int a7, char a8, char a9);
	bool __cdecl c_simulation_generic_entity__deserialize2_hook(int a1, int a2, GenericEntitySimulationData *data, Blam::BitStream *stream, char a5);
	uint32_t __fastcall c_simulation_generic_entity_definition__spawn_object_hook(int thisptr, void *unused, uint8_t *data, GenericEntitySimulationData *simulationData, int a4, uint8_t *newObject);
	int ScenerySyncHook(uint32_t tagIndex, int a2, char a3);
	void ProjectileAttachmentHook();
	void SendSimulationDamageAftermathEventHook(int a1, int a2, int a3, int playerIndex, size_t size, void *data);
	void ClientRagdollDamageHook(int objectIndex, char *damageData);

	void simulation_control_decode(uint8_t *input, uint8_t *output);
	void simulation_control_encode(uint8_t *input, uint32_t unitObjectIndex, uint8_t *output);

	void OnMapVariantRequestChange(Blam::MapVariant *mapVariant);

	Blam::MapVariant::VariantPlacement s_SyncPlacements[640] = { 0 };
}

namespace Patches::Simulation
{
	void ApplyAll()
	{
		// fix client map variant placement precision
		Hook(0xC928C, c_simulation_generic_entity__serialize2_hook, HookFlags::IsCall).Apply();
		Hook(0xC913A, c_simulation_generic_entity__deserialize2_hook, HookFlags::IsCall).Apply();
		Hook(0xC8E91, c_simulation_generic_entity_definition__spawn_object_hook, HookFlags::IsCall).Apply();

		Patches::Network::OnMapVariantRequestChange(OnMapVariantRequestChange);
		Hook(0x000B2E1B, ScenerySyncHook, HookFlags::IsCall).Apply();

		Hook(0x0077862A, ProjectileAttachmentHook).Apply();

		// sync flashlight
		Hook(0xA7880, simulation_control_decode, HookFlags::IsCall).Apply();
		Hook(0xA7A17, simulation_control_encode, HookFlags::IsCall).Apply();
		Hook(0xA77BF, simulation_control_encode, HookFlags::IsCall).Apply();
		Patch::NopFill(Pointer::Base(0x74C4E7), 6);

		// fixes nade jumping, hammer boosting, etc..
		Hook(0xB22C4, SendSimulationDamageAftermathEventHook, HookFlags::IsCall).Apply();
		// prevent from sending duplicate damage aftermath events
		Patch(0x00754226, 0x90, 12).Apply();

		// restore h3 backflips
		Hook(0x007543DF, ClientRagdollDamageHook, HookFlags::IsCall).Apply();
	}
}

namespace
{
	const auto GetMapVariant = (Blam::MapVariant* (__cdecl*)())(0x00583230);

	struct GenericEntitySimulationData
	{
		RealVector3D Position;
		RealVector3D Forward;
		RealVector3D Up;
		float Scale;
		RealVector3D TransitionalVelocity;
		RealVector3D AngularVelocity;
		float BodyVitality;
		char BodyStunned;
		char Unknown45;
		char Unknown46;
		char Unknown47;
		float ShieldVitality;
		char ShieldStunned;
		char ShieldCharging;
		char ShieldDepleted;
		char Unknown4F;
		int16_t OwnerTeamIndex;
		char Unknown52;
		char Unknown53;
		int Unknown54;
		int Unknown58;
		int Unknown5C;
		char Unknown60;
		char Unknown61;
		char Unknown62;
		char ConstraintState;
		int Unknown64;
		int Unknown68;
		int Unknown6C;
		int Unknown70;
		int InitialAttachment;
		RealVector3D AttachmentPosition;
		RealVector3D AttachmentUp;
		RealVector3D AttachmentForward;
		int16_t MultiplayerPropertiesOffset;
		int16_t VariantPlacementIndex;
	};

	bool __cdecl c_simulation_generic_entity__serialize2_hook(char a1, int a6, int a3, DWORD *a4, GenericEntitySimulationData *data, Blam::BitStream *stream, int a7, char a8, char a9)
	{
		const auto sub_4AEB10 = (bool(*)(char a1, int a6, int a3, void *a4, GenericEntitySimulationData *data, Blam::BitStream *stream, int a7, char a8, char a9))(0x4AEB10);

		auto ret = sub_4AEB10(a1, a6, a3, a4, data, stream, a7, a8, a9);

		if (data->VariantPlacementIndex && data->VariantPlacementIndex != -1)
		{
			auto placement = &s_SyncPlacements[data->VariantPlacementIndex];
			if (placement->PlacementFlags & 2)
			{
				stream->WriteBool(true);
				stream->WriteBlock(0x54 * 8, (uint8_t*)placement);
			}
			else
			{
				stream->WriteBool(false);
			}
		}
		else
		{
			stream->WriteBool(false);
		}

		return ret && stream->Position() <= 8 * stream->Size();
	}

	bool __cdecl c_simulation_generic_entity__deserialize2_hook(int a1, int a2, GenericEntitySimulationData *data, Blam::BitStream *stream, char a5)
	{
		const auto sub_4AE5F0 = (bool(*)(int a1, int a2, GenericEntitySimulationData *data, Blam::BitStream *stream, char a5))(0x4AE5F0);
		auto ret = sub_4AE5F0(a1, a2, data, stream, a5);

		if (ret && stream->ReadBool())
			stream->ReadBlock(0x54 * 8, (uint8_t*)&s_SyncPlacements[data->VariantPlacementIndex]);

		return ret && stream->Position() <= 8 * stream->Size();
	}

	uint32_t __fastcall c_simulation_generic_entity_definition__spawn_object_hook(int thisptr, void *unused,
		uint8_t *data, GenericEntitySimulationData *simulationData, int a4, uint8_t *newObject)
	{
		const auto c_simulation_generic_entity_definition__spawn_object = (uint32_t(__thiscall*)(int thisptr,
			uint8_t *data, GenericEntitySimulationData *simulationData, int a4, uint8_t *newObject))(0x004AD3E0);
		const auto Object_SyncPlacementProperties = (void(__thiscall *)(void *thisptr, Blam::MapVariant::VariantProperties *placementProps,
			int objectIndex_1))(0x00580E80);

		auto placementIndex = *(int16_t*)(uint8_t*)(data + 0x4);
		if (placementIndex != -1)
		{
			auto &placement = s_SyncPlacements[placementIndex];
			if (placement.PlacementFlags & 2)
			{
				*(RealVector3D*)(newObject + 0x1c) = placement.Position;
				*(RealVector3D*)(newObject + 0x28) = placement.RightVector;
				*(RealVector3D*)(newObject + 0x34) = placement.UpVector;
			}
		}

		auto objectIndex = c_simulation_generic_entity_definition__spawn_object(thisptr, data, simulationData, a4, newObject);
		if (placementIndex != -1 && objectIndex != -1)
		{
			auto &placement = s_SyncPlacements[placementIndex];
			if (placement.PlacementFlags & 2)
				Object_SyncPlacementProperties(GetMapVariant(), &placement.Properties, objectIndex);
		}

		return objectIndex;
	}

	void OnMapVariantRequestChange(Blam::MapVariant *mapVariant)
	{
		for (auto i = 0; i < 640; i++)
			s_SyncPlacements[i] = mapVariant->Placements[i];
	}

	int ScenerySyncHook(uint32_t tagIndex, int a2, char a3)
	{
		const auto sub_4AFA90 = (int(__cdecl *)(uint32_t tagIndex, int a2, char a3))(0x4AFA90);

		auto def = Blam::Tags::TagInstance(tagIndex).GetDefinition<Blam::Tags::Objects::Object>();
		if (def)

		switch (Blam::Objects::ObjectType(def->ObjectType))
		{
		case Blam::Objects::eObjectTypeScenery:
			return 16;
		case Blam::Objects::eObjectTypeCrate:
			return (a3 != 0) + 0x10;
		}
	
		return sub_4AFA90(tagIndex, a2, a3);
	}

	bool ShouldAttachProjectile(uint32_t parentObjectIndex, uint32_t projectileObjectIndex)
	{
		const auto object_is_phased = (bool(*)(uint32_t objectIndex))(0x0059A7B0);

		auto parentObject = Blam::Objects::Get(parentObjectIndex);
		if (!parentObject)
			return false;

		return !parentObject->GetMultiplayerProperties() || !object_is_phased(parentObjectIndex);
	}

	__declspec(naked) void ProjectileAttachmentHook()
	{
		__asm
		{
			pushad
			push [ebp+0x8]
			push [ebp+0xC]
			call ShouldAttachProjectile
			add esp, 0x8
			test al, al
			popad
			jnz ATTACH
			mov eax, 0xB786C8
			jmp eax
			ATTACH:
			mov eax, 0xB78630
			jmp eax
		}
	}


	void simulation_control_decode(uint8_t *input, uint8_t *output)
	{
		const auto sub_4712D0 = (void(*)(uint8_t *input, uint8_t *ouput))(0x4712D0);
		sub_4712D0(input, output);

		if (*(input + 0x18) & (1 << 5))
			*(uint32_t*)(output + 0x8) |= 4;
		else
			*(uint32_t*)(output + 0x8) &= ~4;
	}

	void simulation_control_encode(uint8_t *input, uint32_t unitObjectIndex, uint8_t *output)
	{
		const auto sub_4708F0 = (void(*)(uint8_t *data, int unitObjectIndex, uint8_t *a3))(0x4708F0);
		sub_4708F0(input, unitObjectIndex, output);

		if (*(uint32_t*)(input + 0x8) & 4)
			*(output + 0x18) |= (1 << 5);
		else
			*(output + 0x18) &= ~(1 << 5);
	}


	void __cdecl SendSimulationDamageAftermathEventHook(int a1, int a2, int a3, int playerIndex, size_t size, void *data)
	{
		Pointer(data)(0xA).WriteFast<uint8_t>(1); // flag the direction vector to be serialized

		auto simulation_broadcast_event = (void(__cdecl*)(int a1, int a2, int a3, int playerIndex, size_t size, void *data))(0x4E5A30);
		simulation_broadcast_event(a1, a2, a3, playerIndex, size, data);
	}

	void ClientRagdollDamageHook(int objectIndex, char *damageData)
	{
		const auto object_deal_damage = (void(*)(int objectIndex, char *damageData))(0x00B52C50);
		*(int*)(damageData + 0x54) = 1;
		object_deal_damage(objectIndex, damageData);
	}
}
