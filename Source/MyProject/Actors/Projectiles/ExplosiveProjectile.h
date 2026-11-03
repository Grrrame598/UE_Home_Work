// Взрывающийся снаряд: ExplosionComponent + взрыв при столкновении (ДЗ №6).

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/MyProjectile.h"
#include "ExplosiveProjectile.generated.h"

class UExplosionComponent;

/**
 * Снаряд, который взрывается при столкновении с поверхностью.
 * Таймер DetonationTime — страховка, если снаряд ни во что не попал.
 */
UCLASS()
class MYPROJECT_API AExplosiveProjectile : public AMyProjectile
{
	GENERATED_BODY()

public:
	AExplosiveProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	/** Страховочный таймер самоподрыва, сек */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
	float DetonationTime = 3.f;

	virtual void OnProjectileLaunched() override;

	/** Снаряд попал в поверхность и остановился — взрываемся */
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

private:
	/** Взрыв + удаление снаряда (защита от двойного срабатывания) */
	void Detonate();

	/** Контроллер стрелка: через Owner-Pawn, иначе nullptr */
	AController* GetOwnerController() const;

	FTimerHandle DetonationTimer;

	/** Взрыв уже произошёл */
	bool bDetonated = false;
};
