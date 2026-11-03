// Компонент взрыва: урон по области, VFX и импульс физическим объектам (ДЗ №6).

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"

class UDamageType;
class UParticleSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion);

/**
 * Взрыв.
 *
 * Ставится на снаряд (AExplosiveProjectile) и может ставиться на любой
 * взрывающийся объект (бочка, мина). Сам ничего не решает о моменте взрыва —
 * это делает владелец, вызывая Explode().
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/**
	 * Взрыв в точке компонента.
	 * @param InstigatorController Контроллер того, кто взорвал (нужен, чтобы урон засчитался игроку)
	 */
	UFUNCTION(BlueprintCallable, Category = "Explosion")
	void Explode(AController* InstigatorController);

	/** Событие взрыва — сюда можно повесить звук, decal, спавн осколков в BP */
	UPROPERTY(BlueprintAssignable, Category = "Explosion")
	FOnExplosion OnExplosion;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	float MaxDamage = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	float MinDamage = 30.f;

	/** Насколько быстро урон падает от InnerRadius к OuterRadius (1 = линейно) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	float DamageFalloff = 1.f;

	/** Класс урона (DT_Explosion); пусто — дефолтный DamageType */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	/** Радиус полного урона */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Radius")
	float InnerRadius = 300.f;

	/** Радиус, дальше которого урона нет */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Radius")
	float OuterRadius = 500.f;

	/** Cascade-эффект взрыва (P_Grenade_Explosion_Dark) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|VFX")
	UParticleSystem* ExplosionVFX = nullptr;

	/** Сила радиального импульса — «взрывная волна» для физических объектов */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Impulse")
	float ImpulseStrength = 800.f;

	/** true — импульс сразу меняет скорость (лёгкие объекты разлетаются охотнее) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Impulse")
	bool bImpulseVelChange = true;
};
