// Огнестрельное оружие (пистолет) — наследник ABaseWeapon.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/BaseWeapon.h"
#include "RangeWeaponItem.generated.h"

class UAnimMontage;
class UWeaponBarellComponent;
class AMyProjectile;

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

	/** Альтернативный выстрел: снаряд из подствольника, летит по баллистике */
	virtual void AltShot() override;

protected:
	/** Монтаж выстрела на самом оружии (отдача/затвор) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animations")
	UAnimMontage* WeaponFireMontage = nullptr;

	/** Монтаж выстрела на персонаже (руки/корпус) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animations")
	UAnimMontage* CharacterFireMontage = nullptr;

	/** Подствольник: точка спавна снаряда (позиция подбирается в BP) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UWeaponBarellComponent* UnderbarrelBarell;

	/** Класс снаряда альтернативного выстрела (BP_ExplosiveProjectile) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|AltShot")
	TSubclassOf<AMyProjectile> GrenadeProjectileClass;

private:
	/** Проигрывает монтаж на меше оружия, возвращает длительность */
	float PlayWeaponFireMontage();
};
