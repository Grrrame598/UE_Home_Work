// Простая мишень для демонстрации урона от выстрела.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetDummy.generated.h"

class UStaticMeshComponent;

/**
 * Мишень: принимает урон через стандартный AActor::TakeDamage
 * (его вызывает UWeaponBarellComponent::Shot через ApplyDamage)
 * и показывает остаток здоровья на экране.
 */
UCLASS(Blueprintable)
class MYPROJECT_API ATargetDummy : public AActor
{
	GENERATED_BODY()

public:
	ATargetDummy();

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Меш мишени (куб по умолчанию, можно заменить в BP) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	UStaticMeshComponent* Mesh;

	/** Максимальное здоровье */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.f;

	/** Вывести текущее здоровье в лог и на экран */
	void ShowHealth() const;

private:
	/** Текущее здоровье */
	float CurrentHealth = 100.f;
};
