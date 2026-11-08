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
		SetActorHiddenInGame(false);
		return;
	}

	// Сокета нет (например, ещё не создан в скелете) — просто скрываем предмет
	SetActorHiddenInGame(true);
}
