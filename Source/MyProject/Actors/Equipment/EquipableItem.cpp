// База любого надеваемого предмета. ДЗ №6.

#include "EquipableItem.h"

#include "Components/SkeletalMeshComponent.h"

AEquipableItem::AEquipableItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEquipableItem::OnEquipped(USkeletalMeshComponent* CharacterMesh, FName SocketName)
{
	if (CharacterMesh)
	{
		AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}

	SetActorHiddenInGame(false);
}

void AEquipableItem::OnUnequipped(USkeletalMeshComponent* CharacterMesh, FName SocketName)
{
	if (CharacterMesh && !SocketName.IsNone())
	{
		AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

		// ВРЕМЕННАЯ ДИАГНОСТИКА (подбор Rot сокета в редакторе без слепых переборов):
		// печатает мировые оси кости, мировую ориентацию сокета и куда смотрит оружие.
		// Удалить после настройки CharacterSideArmSocket / CharacterBackSocket.
		const FName BoneName = CharacterMesh->GetSocketBoneName(SocketName);
		const FTransform SocketWorld = CharacterMesh->GetSocketTransform(SocketName, RTS_World);
		const FQuat BoneRot = CharacterMesh->GetBoneQuaternion(BoneName, EBoneSpaces::WorldSpace);
		const FQuat MeshRot = CharacterMesh->GetComponentQuat();

		UE_LOG(LogTemp, Warning,
			TEXT("[Socket] %s (кость %s) | сокет world rot=%s | кость world rot=%s "
				 "| оси кости X=%s Y=%s Z=%s | оси меша X=%s Y=%s Z=%s | оружие forward=%s"),
			*SocketName.ToString(), *BoneName.ToString(),
			*SocketWorld.Rotator().ToString(), *BoneRot.Rotator().ToString(),
			*BoneRot.GetAxisX().ToString(), *BoneRot.GetAxisY().ToString(), *BoneRot.GetAxisZ().ToString(),
			*MeshRot.GetAxisX().ToString(), *MeshRot.GetAxisY().ToString(), *MeshRot.GetAxisZ().ToString(),
			*GetActorForwardVector().ToString());

		SetActorHiddenInGame(false);
		return;
	}

	// Сокета нет (например, ещё не создан в скелете) — просто скрываем предмет
	SetActorHiddenInGame(true);
}
