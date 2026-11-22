// HUD-виджет (A6): C++-часть. Визуал — в BP-наследнике (WBP_HUD).

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyHUDWidget.generated.h"

class AMyPlayerCharacter;
class UCharacterAttributesComponent;
class ARangeWeaponItem;

/**
 * Обёртка HUD по схеме референса:
 *  - C++ подписывается на делегаты (атрибуты персонажа, патроны оружия);
 *  - сам виджет (бары, тексты) оформляется в BP-наследнике и получает данные
 *    через BlueprintImplementableEvent-хендлеры ниже.
 */
UCLASS(Abstract)
class MYPROJECT_API UMyHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Подписаться на текущего персонажа игрока (зовёт AMyHUD) */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void InitializeHUD(AMyPlayerCharacter* InCharacter);

	/** Переподписаться на оружие текущего слота (после Q) */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RefreshWeapon();

protected:
	// --- События для BP-визуала ---

	/** HP и стамина в процентах (0..1) */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnAttributesUpdated(float HealthPercent, float StaminaPercent);

	/** Оружие/патроны: имя слота, в обойме, в запасе (-1 — бесконечно) */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnWeaponUpdated(const FText& WeaponName, int32 AmmoInMag, int32 ReserveAmmo);

	UFUNCTION()
	void HandleAttributesChanged(float HealthPercent, float StaminaPercent);

	UFUNCTION()
	void HandleAmmoChanged(int32 AmmoInMag, int32 ReserveAmmo);

private:
	/** Отписаться от старого оружия */
	void UnbindWeapon();

	/** Текущий персонаж (weak: HUD переживает рестарт PIE) */
	TWeakObjectPtr<AMyPlayerCharacter> CachedCharacter;

	/** Оружие, к которому подписан OnAmmoChanged */
	TWeakObjectPtr<ARangeWeaponItem> CachedWeapon;

	/** Повторная попытка привязки, пока оружие не создано */
	FTimerHandle RetryTimerHandle;

	/** Одна повторная попытка RefreshWeapon (по таймеру) */
	void RetryRefreshWeapon();
};
