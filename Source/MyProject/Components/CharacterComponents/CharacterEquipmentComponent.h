// Компонент экипировки персонажа: выдача, надевание/снятие и стрельба.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MyProjectTypes.h"
#include "CharacterEquipmentComponent.generated.h"

class AEquipableItem;
class ABaseWeapon;
class ACharacter;

/**
 * Отвечает за «лоадаут» персонажа:
 *  - создаёт оружие из класса SideArmClass и крепит его к сокету CharacterWeaponSocket;
 *  - умеет надеть/снять оружие (по кнопке) — это включает/выключает позу в AnimBP;
 *  - пробрасывает выстрел в текущее оружие.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterEquipmentComponent();

	/** Тип текущего экипированного предмета (None, если оружие снято) */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	EEquipableItemType GetCurrentEquippedItemType() const;

	/** Текущий слот (Pistol / Rifle) */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	EEquipableItemType GetCurrentSlot() const { return CurrentSlot; }

	/** Оружие текущего слота (nullptr, если слот пуст) */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	ABaseWeapon* GetCurrentWeapon() const;

	/** Экипировано ли сейчас оружие */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsEquipped() const { return bIsEquipped; }

	/** Выстрел из текущего оружия */
	void Fire();

	/** Альтернативный выстрел из текущего оружия (подствольник, ПКМ) */
	void AltFire();

	/** Переключить тип выстрела (камера / дуло) у текущего оружия */
	void ToggleShotOrigin();

	/**
	 * Взять следующее оружие из слотов (Q). Если предмет всего один —
	 * работает как надеть/снять из ДЗ №5.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SwitchToNextWeapon();

	/** Надеть/снять оружие (биндится на кнопку в персонаже) */
	void ToggleEquip();

	/** Надеть оружие */
	void Equip();

	/** Снять оружие */
	void Unequip();

protected:
	virtual void BeginPlay() override;

	/** Классы предметов по слотам: Pistol -> BP_Pistol, Rifle -> BP_Rifle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Loadout")
	TMap<EEquipableItemType, TSubclassOf<AEquipableItem>> ItemClasses;

	/** Куда убирается снятое оружие: Pistol -> CharacterSideArmSocket, Rifle -> CharacterBackSocket */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Loadout")
	TMap<EEquipableItemType, FName> HolsterSockets;

	/** Устаревшее поле ДЗ №5: если ItemClasses пуст, используется как слот Pistol */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Loadout")
	TSubclassOf<ABaseWeapon> SideArmClass;

	/** Надевать оружие сразу при старте (для демонстрации критерия 4 можно выключить) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Loadout")
	bool bEquipOnStart = true;

private:
	/** Спавн всех предметов из ItemClasses; снятые сразу крепятся в кобуру/на спину */
	void CreateLoadout();

	/** Крепление предмета в руки */
	void EquipItem(EEquipableItemType ItemType);

	/** Крепление в кобуру/на спину (или скрытие, если сокета нет) */
	void UnequipItem(EEquipableItemType ItemType);

	AEquipableItem* GetItem(EEquipableItemType ItemType) const;

	/** Сокет «не в руках» для слота (из HolsterSockets, иначе дефолт по типу) */
	FName GetHolsterSocket(EEquipableItemType ItemType) const;

	/** Слоты в стабильном порядке (по значению enum) */
	TArray<EEquipableItemType> GetSlotOrder() const;

	/** Предметы по слотам */
	UPROPERTY()
	TMap<EEquipableItemType, TObjectPtr<AEquipableItem>> Items;

	/** Текущий слот */
	EEquipableItemType CurrentSlot = EEquipableItemType::None;

	/** Персонаж-владелец компонента */
	TWeakObjectPtr<ACharacter> CachedCharacter;

	/** Оружие в руках? */
	bool bIsEquipped = false;
};
