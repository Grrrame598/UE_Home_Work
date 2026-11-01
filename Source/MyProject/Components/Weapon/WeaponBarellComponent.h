// Компонент ствола: точка выстрела, трасса, урон и эффекты.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"

class UNiagaraSystem;

/**
 * Ствол оружия.
 *
 * Это обычный SceneComponent, который крепится к мешу оружия БЕЗ сокета,
 * а его положение на дуле подбирается в Blueprint вручную
 * (это ровно та рекомендация, что дана в задании ДЗ №5).
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponBarellComponent();

	/**
	 * Один выстрел.
	 * @param ShotStart          Начало трассы (камера игрока или дуло)
	 * @param ShotDirection      Направление выстрела
	 * @param InstigatorController Контроллер стрелка (нужен для ApplyDamage — кто нанёс урон)
	 */
	void Shot(const FVector& ShotStart, const FVector& ShotDirection, AController* InstigatorController);

protected:
	/** Дальность стрельбы, см */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell|Damage")
	float FiringRange = 5000.f;

	/** Урон за попадание */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell|Damage")
	float DamageAmount = 20.f;

	/** Niagara-эффект дульной вспышки (спавнится в точке компонента) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell|VFX")
	UNiagaraSystem* MuzzleFlashFX = nullptr;

	/** Niagara-эффект следа пули (параметр TraceEnd — конец трассы) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell|VFX")
	UNiagaraSystem* TraceFX = nullptr;

	/** Рисовать отладочную линию/точку попадания */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell|Debug")
	bool bDrawDebug = false;
};
