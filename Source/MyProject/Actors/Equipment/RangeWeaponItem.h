// Огнестрельное оружие (пистолет) — наследник ABaseWeapon.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/BaseWeapon.h"
#include "RangeWeaponItem.generated.h"

class UAnimMontage;
class UWeaponBarellComponent;
class AMyProjectile;

/** Патроны изменились: (в обойме, в запасе). Запас = -1 означает «бесконечный» */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, AmmoInMag, int32, ReserveAmmo);

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

	// --- A5: патроны ---------------------------------------------------------
	// Схема «обойма + запас». Запас пока бесконечный (-1), поэтому снаружи
	// поведение как в референсе; чтобы ввести реальный запас, достаточно
	// выставить MaxReserveAmmo и обнулить bInfiniteReserve в BP.

	/** Патроны изменились — на него подписывается HUD (A6) */
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Ammo")
	FOnAmmoChanged OnAmmoChanged;

	/** Патронов в обойме */
	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int32 GetAmmoInMag() const { return AmmoInMag; }

	/** Патронов в запасе; -1 — бесконечный */
	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int32 GetReserveAmmo() const { return bInfiniteReserve ? -1 : ReserveAmmo; }

	/** Размер обоймы */
	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int32 GetMagSize() const { return MagSize; }

	/** Есть ли чем стрелять */
	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	bool HasAmmo() const { return AmmoInMag > 0; }

	/** Перезарядка: добирает обойму из запаса. Возвращает false, если перезаряжаться нечем */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	bool Reload();

	/** Перезарядка с задержкой ReloadDuration (автоперезарядка, F) */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void StartReload();

	/** Идёт ли перезарядка */
	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	bool IsReloading() const { return bIsReloading; }

protected:
	/** Размер обоймы, патронов */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (ClampMin = "1"))
	int32 MagSize = 12;

	/** Стартовое количество патронов в обойме */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (ClampMin = "0"))
	int32 AmmoInMag = 12;

	/** Запас патронов (не используется, пока bInfiniteReserve = true) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (ClampMin = "0"))
	int32 ReserveAmmo = 0;

	/** Бесконечный запас: обойма всегда добирается до полной */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	bool bInfiniteReserve = true;

	/** Сколько патронов максимум может лежать в запасе (0 — без лимита) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (ClampMin = "0"))
	int32 MaxReserveAmmo = 0;

	/** Автоматически перезаряжаться, когда обойма опустела */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	bool bAutoReload = true;

	/** Длительность перезарядки, с */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (ClampMin = "0.0"))
	float ReloadDuration = 1.5f;

	/** Идёт перезарядка прямо сейчас */
	bool bIsReloading = false;

	/** Таймер перезарядки */
	FTimerHandle ReloadTimerHandle;

	/** Реальная перезарядка по таймеру */
	void FinishReload();

	/** Меняет обойму и рассылает OnAmmoChanged */
	void SetAmmoInMag(int32 NewAmmo);

	/** Меняет запас и рассылает OnAmmoChanged */
	void SetReserveAmmo(int32 NewReserve);

	virtual void BeginPlay() override;

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
