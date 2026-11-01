// Базовый абстрактный класс оружия (ДЗ №5).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/MyProjectTypes.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UWeaponBarellComponent;

/**
 * Базовая сущность любого оружия.
 *
 * Здесь описана базовая функция Shot(): строится трасса, при попадании по цели
 * наносится урон. Наследники (например ARangeWeaponItem) добавляют своё поведение
 * (анимации, боезапас и т.п.) и при необходимости переопределяют точку выстрела.
 */
UCLASS(Abstract)
class MYPROJECT_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	/** Базовый выстрел: трасса + урон + эффекты. Вызывается из персонажа */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Shot();

	/** Тип предмета — по нему AnimBP персонажа выбирает позу (есть оружие / нет) */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	EEquipableItemType GetItemType() const { return ItemType; }

protected:
	/**
	 * Определяет, откуда и куда стреляем.
	 * Базовая реализация умеет два типа выстрела (из камеры игрока и из дула),
	 * тип задаётся свойством ShotOriginType.
	 */
	virtual void GetShotOrigin(FVector& OutStart, FVector& OutDirection, AController*& OutInstigator) const;

	/** Меш оружия */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	USkeletalMeshComponent* WeaponMesh;

	/** Ствол = точка выстрела. Крепится к мешу без сокета, позиция подбирается в BP */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UWeaponBarellComponent* WeaponBarell;

	/** Тип предмета для анимаций персонажа */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEquipableItemType ItemType = EEquipableItemType::None;

	/** Откуда производится выстрел: из камеры игрока или из дула оружия (в игре переключается клавишей R) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shot")
	EShotOrigin ShotOriginType = EShotOrigin::Camera;

public:
	/** Переключает тип выстрела: камера <-> дуло */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ToggleShotOrigin();

	/** Текущий тип выстрела */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	EShotOrigin GetShotOriginType() const { return ShotOriginType; }

public:
	/** Меш оружия (нужен, например, анимационному BP оружия) */
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
};
