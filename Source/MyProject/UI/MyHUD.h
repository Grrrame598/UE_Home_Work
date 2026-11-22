// Класс HUD: создаёт виджет (WBP_HUD) и держит его привязанным к персонажу.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UMyHUDWidget;

/**
 * HUD игрока. В BP-наследнике задаётся HUDWidgetClass = WBP_HUD,
 * сам виджет создаётся в BeginPlay и подписывается на персонажа.
 * После смены оружия (Q) вызывается RefreshWeapon.
 */
UCLASS()
class MYPROJECT_API AMyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Перечитаться на текущего персонажа (после рестарта PIE / респавна) */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RefreshHUD();

	/** Переподписаться на оружие текущего слота (зовётся после смены оружия) */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RefreshWeapon();

protected:
	/** Класс виджета (WBP_HUD — наследник UMyHUDWidget) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UMyHUDWidget> HUDWidgetClass;

	/** Созданный виджет */
	UPROPERTY()
	UMyHUDWidget* HUDWidget = nullptr;

private:
	/** К какому персонажу уже привязан виджет */
	TWeakObjectPtr<AActor> BoundPawn;
};
