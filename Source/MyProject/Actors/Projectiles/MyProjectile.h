// Базовый снаряд: сфера + ProjectileMovement (ДЗ №6).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

/**
 * Физический снаряд, летящий по траектории.
 * Сам по себе ничего не взрывает — это делает наследник (AExplosiveProjectile).
 */
UCLASS()
class MYPROJECT_API AMyProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMyProjectile();

	/** Запуск в направлении Direction с начальной скоростью ProjectileMovement */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void LaunchProjectile(const FVector& Direction);

protected:
	/** Коллизия снаряда (корень актора) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	/** Баллистика: скорость, гравитация, остановка при попадании */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	/** Хук для наследников: вызывается сразу после запуска */
	virtual void OnProjectileLaunched();
};
