// Компонент экипировки персонажа: выдача, надевание/снятие и стрельба.

#include "CharacterEquipmentComponent.h"

#include "Actors/Equipment/BaseWeapon.h"
#include "Actors/Equipment/EquipableItem.h"
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
	// Обратная совместимость с ДЗ №5: в BP персонажа задан только SideArmClass
	if (ItemClasses.Num() == 0 && SideArmClass)
	{
		ItemClasses.Add(EEquipableItemType::Pistol, SideArmClass);
	}

	if (!CachedCharacter.IsValid() || Items.Num() > 0)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = CachedCharacter.Get();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (const TPair<EEquipableItemType, TSubclassOf<AEquipableItem>>& Pair : ItemClasses)
	{
		if (Pair.Key == EEquipableItemType::None || !Pair.Value)
		{
			continue;
		}

		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(Pair.Value, SpawnParameters);

		if (!Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Equipment] Не удалось создать предмет %s"), *Pair.Value->GetName());
			continue;
		}

		Item->SetActorEnableCollision(false);
		Items.Add(Pair.Key, Item);

		// Пока не в руках — сразу в кобуру/на спину (или скрыто, если сокета нет)
		UnequipItem(Pair.Key);
	}

	if (!bEquipOnStart)
	{
		return;
	}

	// Стартовый слот: пистолет, если он есть, иначе первый по порядку
	const TArray<EEquipableItemType> Slots = GetSlotOrder();

	if (Slots.Num() > 0)
	{
		CurrentSlot = Slots.Contains(EEquipableItemType::Pistol) ? EEquipableItemType::Pistol : Slots[0];
		EquipItem(CurrentSlot);
		bIsEquipped = true;
	}
}

void UCharacterEquipmentComponent::Equip()
{
	if (CurrentSlot == EEquipableItemType::None)
	{
		// Слот ещё не выбран — берём первый доступный
		const TArray<EEquipableItemType> Slots = GetSlotOrder();

		if (Slots.Num() == 0)
		{
			return;
		}

		CurrentSlot = Slots[0];
	}

	EquipItem(CurrentSlot);
	bIsEquipped = true;
}

void UCharacterEquipmentComponent::Unequip()
{
	if (CurrentSlot == EEquipableItemType::None)
	{
		return;
	}

	UnequipItem(CurrentSlot);
	bIsEquipped = false;
}

void UCharacterEquipmentComponent::SwitchToNextWeapon()
{
	const TArray<EEquipableItemType> Slots = GetSlotOrder();

	if (Slots.Num() == 0)
	{
		return;
	}

	// Одно оружие — сохраняем поведение ДЗ №5 (Q надевает/снимает)
	if (Slots.Num() == 1)
	{
		ToggleEquip();
		return;
	}

	const int32 CurrentIndex = Slots.IndexOfByKey(CurrentSlot);
	const EEquipableItemType NextSlot = Slots[(CurrentIndex + 1) % Slots.Num()];

	// Старое оружие уходит в кобуру/на спину, новое берём в руки
	if (bIsEquipped && CurrentSlot != EEquipableItemType::None && CurrentSlot != NextSlot)
	{
		UnequipItem(CurrentSlot);
	}

	CurrentSlot = NextSlot;
	EquipItem(CurrentSlot);
	bIsEquipped = true;
}

void UCharacterEquipmentComponent::EquipItem(EEquipableItemType ItemType)
{
	AEquipableItem* Item = GetItem(ItemType);

	if (!Item || !CachedCharacter.IsValid())
	{
		return;
	}

	if (USkeletalMeshComponent* CharacterMesh = CachedCharacter->GetMesh())
	{
		Item->OnEquipped(CharacterMesh, SocketCharacterWeapon);
	}
}

void UCharacterEquipmentComponent::UnequipItem(EEquipableItemType ItemType)
{
	AEquipableItem* Item = GetItem(ItemType);

	if (!Item || !CachedCharacter.IsValid())
	{
		return;
	}

	USkeletalMeshComponent* CharacterMesh = CachedCharacter->GetMesh();
	const FName HolsterSocket = GetHolsterSocket(ItemType);

	if (CharacterMesh && !HolsterSocket.IsNone() && CharacterMesh->DoesSocketExist(HolsterSocket))
	{
		Item->OnUnequipped(CharacterMesh, HolsterSocket);
		return;
	}

	// Кобуры/спины нет в скелете — прячем, чтобы оружие не висело в руках
	UE_LOG(LogTemp, Warning, TEXT("[Equipment] Нет сокета '%s' для %s — предмет скрыт"),
		*HolsterSocket.ToString(), *Item->GetName());
	Item->SetActorHiddenInGame(true);
}

AEquipableItem* UCharacterEquipmentComponent::GetItem(EEquipableItemType ItemType) const
{
	const TObjectPtr<AEquipableItem>* Found = Items.Find(ItemType);
	return Found ? Found->Get() : nullptr;
}

FName UCharacterEquipmentComponent::GetHolsterSocket(EEquipableItemType ItemType) const
{
	if (const FName* Found = HolsterSockets.Find(ItemType))
	{
		return *Found;
	}

	// Дефолт, если карта не настроена в BP
	switch (ItemType)
	{
	case EEquipableItemType::Pistol:
		return SocketCharacterSideArm;
	case EEquipableItemType::Rifle:
		return SocketCharacterBack;
	default:
		return NAME_None;
	}
}

TArray<EEquipableItemType> UCharacterEquipmentComponent::GetSlotOrder() const
{
	TArray<EEquipableItemType> Slots;
	Items.GetKeys(Slots);
	Slots.Sort([](const EEquipableItemType& Left, const EEquipableItemType& Right)
	{
		return static_cast<uint8>(Left) < static_cast<uint8>(Right);
	});

	return Slots;
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
	if (bIsEquipped)
	{
		if (ABaseWeapon* Weapon = GetCurrentWeapon())
		{
			Weapon->Shot();
		}
	}
}

void UCharacterEquipmentComponent::AltFire()
{
	if (bIsEquipped)
	{
		if (ABaseWeapon* Weapon = GetCurrentWeapon())
		{
			Weapon->AltShot();
		}
	}
}

void UCharacterEquipmentComponent::ToggleShotOrigin()
{
	if (ABaseWeapon* Weapon = GetCurrentWeapon())
	{
		Weapon->ToggleShotOrigin();
	}
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	return (bIsEquipped && GetItem(CurrentSlot)) ? CurrentSlot : EEquipableItemType::None;
}

ABaseWeapon* UCharacterEquipmentComponent::GetCurrentWeapon() const
{
	return Cast<ABaseWeapon>(GetItem(CurrentSlot));
}
