// Огнестрельное оружие (пистолет) — наследник ABaseWeapon.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/BaseWeapon.h"
#include "RangeWeaponItem.generated.h"

class UAnimMontage;

/**
 * Оружие с монтажами выстрела.
 * Наследник базового класса: добавляет анимации оружия и персонажа,
 * а сам выстрел (трасса + урон) выполняется в ABaseWeapon::Shot().
 */
UCLASS(Blueprintable)
class MYPROJECT_API ARangeWeaponItem : public ABaseWeapon
{
	GENERATED_BODY()

public:
	ARangeWeaponItem();

	virtual void Shot() override;

protected:
	/** Монтаж выстрела на самом оружии (отдача/затвор) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animations")
	UAnimMontage* WeaponFireMontage = nullptr;

	/** Монтаж выстрела на персонаже (руки/корпус) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animations")
	UAnimMontage* CharacterFireMontage = nullptr;

private:
	/** Проигрывает монтаж на меше оружия, возвращает длительность */
	float PlayWeaponFireMontage();
};
