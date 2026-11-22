// B3: взрывающаяся бочка — детонирует от полученного урона.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class UExplosionComponent;

/**
 * Бочка: статичный меш + готовый UExplosionComponent.
 * Любой урон (TakeDamage) → Explode(): радиальный урон 50, VFX, импульс.
 * Повторный урон после взрыва игнорируется — бочка уже разлетелась.
 */
UCLASS()
class MYPROJECT_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	AExplosiveBarrel();

	/** Урон по бочке: первый же взрывает её */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	/** Корпус бочки */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Barrel")
	UStaticMeshComponent* BarrelMesh;

	/** Взрыв: урон по области, VFX, импульс */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Barrel")
	UExplosionComponent* ExplosionComponent;

	/** true — бочка уже взорвалась (защита от повторной детонации) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Barrel")
	bool bExploded = false;
};
