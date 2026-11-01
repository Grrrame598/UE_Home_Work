// Компонент экипировки персонажа: выдача, надевание/снятие и стрельба.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MyProjectTypes.h"
#include "CharacterEquipmentComponent.generated.h"

class ARangeWeaponItem;
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

	/** Экипировано ли сейчас оружие */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsEquipped() const { return bIsEquipped; }

	/** Выстрел из текущего оружия */
	void Fire();

	/** Переключить тип выстрела (камера / дуло) у текущего оружия */
	void ToggleShotOrigin();

	/** Надеть/снять оружие (биндится на кнопку в персонаже) */
	void ToggleEquip();

	/** Надеть оружие */
	void Equip();

	/** Снять оружие */
	void Unequip();

protected:
	virtual void BeginPlay() override;

	/** Класс оружия в левой/правой руке (в BP персонажа: BP_Pistol) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Loadout")
	TSubclassOf<ARangeWeaponItem> SideArmClass = nullptr;

	/** Надевать оружие сразу при старте (для демонстрации критерия 4 можно выключить) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Loadout")
	bool bEquipOnStart = true;

private:
	/** Создаёт оружие по SideArmClass */
	void CreateLoadout();

	/** Текущее оружие персонажа */
	UPROPERTY()
	TObjectPtr<ARangeWeaponItem> SideArmWeapon = nullptr;

	/** Персонаж-владелец компонента */
	TWeakObjectPtr<ACharacter> CachedCharacter;

	/** Оружие в руках? */
	bool bIsEquipped = false;
};
