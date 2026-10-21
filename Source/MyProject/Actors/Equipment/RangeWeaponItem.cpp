// Огнестрельное оружие (пистолет) — наследник ABaseWeapon.

#include "RangeWeaponItem.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	// Тип предмета: по нему AnimBP персонажа включает позу «с пистолетом»
	ItemType = EEquipableItemType::Pistol;
}

void ARangeWeaponItem::Shot()
{
	// 1. Анимации (оружие + персонаж)
	PlayWeaponFireMontage();

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		if (CharacterFireMontage && CharacterOwner->GetMesh())
		{
			if (UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance())
			{
				CharacterAnimInstance->Montage_Play(CharacterFireMontage);
			}
		}
	}

	// 2. Сам выстрел — базовая реализация из ABaseWeapon (трасса + урон + эффекты)
	Super::Shot();
}

float ARangeWeaponItem::PlayWeaponFireMontage()
{
	if (!WeaponFireMontage)
	{
		return 0.f;
	}

	USkeletalMeshComponent* MeshComponent = GetWeaponMesh();
	if (!MeshComponent)
	{
		return 0.f;
	}

	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		return AnimInstance->Montage_Play(WeaponFireMontage);
	}

	return 0.f;
}
