// Атрибуты персонажа (A3): здоровье и стамина.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

/**
 * Здоровье и стамина персонажа.
 *  - HP приходит уроном через OnTakeAnyDamage (ApplyDamage из WeaponBarellComponent);
 *  - стамина тратится в спринте и восстанавливается в покое;
 *  - на нуле стамины персонаж перестаёт бежать (StopSprint).
 * Проценты отдаются в HUD (A6) через GetHealthPercent / GetStaminaPercent.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterAttributesComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/** Событие смерти (персонаж исчерпал HP) */
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnDeath OnDeath;

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetMaxStamina() const { return MaxStamina; }

	/** Лечение (зона восстановления, B2) */
	void RestoreHealth(float Amount);

	/** HP в процентах (0..1) — для прогресс-бара HUD */
	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetHealthPercent() const;

	/** Стамина в процентах (0..1) — для прогресс-бара HUD */
	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetStaminaPercent() const;

	/** Есть ли чем бежать (стамина больше нуля) */
	UFUNCTION(BlueprintPure, Category = "Attributes")
	bool HasStamina() const { return Stamina > 0.f; }

	/** Флаг «персонаж в спринте» — ставит владелец (AMyPlayerCharacter) */
	void SetSprinting(bool bInSprinting) { bIsSprinting = bInSprinting; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxStamina = 100.f;

	/** Расход стамины в спринте, единиц в секунду */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float SprintStaminaConsumptionVelocity = 20.f;

	/** Восстановление стамины в покое, единиц в секунду */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float StaminaRestoreVelocity = 25.f;

	/** Обработчик OnTakeAnyDamage: вычитает HP, на нуле — OnDeath */
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                     AController* InstigatedBy, AActor* DamageCauser);

private:
	float Health = 0.f;
	float Stamina = 0.f;

	/** Спринт сейчас активен (ставит персонаж) */
	bool bIsSprinting = false;

	/** Аккумулятор для лога стамины раз в секунду */
	float LogTimer = 0.f;
};
