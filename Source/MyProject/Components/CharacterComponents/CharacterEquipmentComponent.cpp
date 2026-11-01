// Компонент экипировки персонажа: выдача, надевание/снятие и стрельба.

#include "CharacterEquipmentComponent.h"

#include "Actors/Equipment/RangeWeaponItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = Cast<ACharacter>(GetOwner());
	CreateLoadout();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	if (!SideArmClass || !CachedCharacter.IsValid() || SideArmWeapon)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = CachedCharacter.Get();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SideArmWeapon = GetWorld()->SpawnActor<ARangeWeaponItem>(SideArmClass, SpawnParameters);

	if (!SideArmWeapon)
	{
		return;
	}

	SideArmWeapon->SetActorEnableCollision(false);

	if (bEquipOnStart)
	{
		Equip();
	}
	else
	{
		// Оружие есть, но снято: персонаж должен играть обычный idle
		SideArmWeapon->SetActorHiddenInGame(true);
		bIsEquipped = false;
	}
}

void UCharacterEquipmentComponent::Equip()
{
	if (!SideArmWeapon || !CachedCharacter.IsValid())
	{
		return;
	}

	if (USkeletalMeshComponent* CharacterMesh = CachedCharacter->GetMesh())
	{
		SideArmWeapon->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketCharacterWeapon);
	}

	SideArmWeapon->SetActorHiddenInGame(false);
	bIsEquipped = true;
}

void UCharacterEquipmentComponent::Unequip()
{
	if (!SideArmWeapon)
	{
		return;
	}

	SideArmWeapon->SetActorHiddenInGame(true);
	bIsEquipped = false;
}

void UCharacterEquipmentComponent::ToggleEquip()
{
	if (bIsEquipped)
	{
		Unequip();
	}
	else
	{
		Equip();
	}
}

void UCharacterEquipmentComponent::Fire()
{
	if (bIsEquipped && SideArmWeapon)
	{
		SideArmWeapon->Shot();
	}
}

void UCharacterEquipmentComponent::ToggleShotOrigin()
{
	if (SideArmWeapon)
	{
		SideArmWeapon->ToggleShotOrigin();
	}
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	return (bIsEquipped && SideArmWeapon) ? SideArmWeapon->GetItemType() : EEquipableItemType::None;
}
