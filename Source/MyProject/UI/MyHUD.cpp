// Класс HUD: создание виджета и привязка к персонажу.

#include "UI/MyHUD.h"

#include "MyProject.h"
#include "Blueprint/UserWidget.h"
#include "Characters/MyPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "UI/MyHUDWidget.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!HUDWidgetClass)
	{
		UE_LOG(LogMyProject, Warning, TEXT("[HUD] не задан HUDWidgetClass (WBP_HUD) у %s"), *GetName());
		return;
	}

	APlayerController* PC = GetOwningPlayerController();

	if (!PC)
	{
		UE_LOG(LogMyProject, Warning, TEXT("[HUD] нет PlayerController"));
		return;
	}

	HUDWidget = CreateWidget<UMyHUDWidget>(PC, HUDWidgetClass);

	if (!HUDWidget)
	{
		UE_LOG(LogMyProject, Warning, TEXT("[HUD] не удалось создать виджет %s"), *HUDWidgetClass->GetName());
		return;
	}

	HUDWidget->AddToViewport();
	RefreshHUD();

	UE_LOG(LogMyProject, Log, TEXT("[HUD] виджет %s создан"), *HUDWidgetClass->GetName());
}

void AMyHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HUDWidget)
	{
		return;
	}

	// Персонаж появляется не в первом кадре (спавн в PIE) — ловим при смене pawn
	APlayerController* PC = GetOwningPlayerController();
	AActor* CurrentPawn = PC ? PC->GetPawn() : nullptr;

	if (CurrentPawn != BoundPawn.Get())
	{
		RefreshHUD();
	}
}

void AMyHUD::RefreshHUD()
{
	if (!HUDWidget)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	AMyPlayerCharacter* Character = PC ? Cast<AMyPlayerCharacter>(PC->GetPawn()) : nullptr;

	BoundPawn = Character;
	HUDWidget->InitializeHUD(Character);
}

void AMyHUD::RefreshWeapon()
{
	if (HUDWidget)
	{
		HUDWidget->RefreshWeapon();
	}
}
