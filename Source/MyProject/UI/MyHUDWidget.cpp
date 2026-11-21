// HUD-виджет (A6): подписка на делегаты атрибутов и патронов.

#include "UI/MyHUDWidget.h"

#include "MyProject.h"
#include "Actors/Equipment/RangeWeaponItem.h"
#include "Characters/MyPlayerCharacter.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UMyHUDWidget::InitializeHUD(AMyPlayerCharacter* InCharacter)
{
	if (CachedCharacter.Get() == InCharacter)
	{
		RefreshWeapon();
		return;
	}

	// Отписываемся от прежнего персонажа
	if (UCharacterAttributesComponent* OldAttributes =
			CachedCharacter.IsValid() ? CachedCharacter->GetCharacterAttributesComponent() : nullptr)
	{
		OldAttributes->OnAttributesChanged.RemoveDynamic(this, &UMyHUDWidget::HandleAttributesChanged);
	}

	UnbindWeapon();
	CachedCharacter = InCharacter;

	if (!InCharacter)
	{
		UE_LOG(LogMyProject, Warning, TEXT("[HUD] персонаж не передан — виджет пустой"));
		return;
	}

	if (UCharacterAttributesComponent* Attributes = InCharacter->GetCharacterAttributesComponent())
	{
		Attributes->OnAttributesChanged.AddUniqueDynamic(this, &UMyHUDWidget::HandleAttributesChanged);
		OnAttributesUpdated(Attributes->GetHealthPercent(), Attributes->GetStaminaPercent());
	}
	else
	{
		UE_LOG(LogMyProject, Warning, TEXT("[HUD] у %s нет компонента атрибутов"), *InCharacter->GetName());
	}

	RefreshWeapon();
}

void UMyHUDWidget::RefreshWeapon()
{
	UnbindWeapon();

	if (!CachedCharacter.IsValid())
	{
		return;
	}

	UCharacterEquipmentComponent* Equipment = CachedCharacter->GetCharacterEquipmentComponent();

	if (!Equipment)
	{
		return;
	}

	ARangeWeaponItem* Weapon = Cast<ARangeWeaponItem>(Equipment->GetCurrentWeapon());

	if (!Weapon)
	{
		// Персонаж ещё без оружия (спавн лоадаута в BeginPlay) — пробуем позже
		if (!RetryTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(
				RetryTimerHandle, this, &UMyHUDWidget::RetryRefreshWeapon, 0.2f, false);
		}
		return;
	}

	// Подписались на новое оружие — сразу показываем его состояние
	CachedWeapon = Weapon;
	Weapon->OnAmmoChanged.AddUniqueDynamic(this, &UMyHUDWidget::HandleAmmoChanged);

	UE_LOG(LogMyProject, Log, TEXT("[HUD] оружие: %s, патроны %d / запас %d"),
		*Weapon->GetName(), Weapon->GetAmmoInMag(), Weapon->GetReserveAmmo());

	OnWeaponUpdated(
		FText::FromString(Weapon->GetName()),
		Weapon->GetAmmoInMag(),
		Weapon->GetReserveAmmo());
}

void UMyHUDWidget::RetryRefreshWeapon()
{
	RefreshWeapon();
}

void UMyHUDWidget::UnbindWeapon()
{
	if (CachedWeapon.IsValid())
	{
		CachedWeapon->OnAmmoChanged.RemoveDynamic(this, &UMyHUDWidget::HandleAmmoChanged);
		CachedWeapon = nullptr;
	}
}

void UMyHUDWidget::HandleAttributesChanged(float HealthPercent, float StaminaPercent)
{
	OnAttributesUpdated(HealthPercent, StaminaPercent);
}

void UMyHUDWidget::HandleAmmoChanged(int32 AmmoInMag, int32 ReserveAmmo)
{
	const FText WeaponName = CachedWeapon.IsValid()
		? FText::FromString(CachedWeapon->GetName())
		: FText::FromString(TEXT("—"));

	OnWeaponUpdated(WeaponName, AmmoInMag, ReserveAmmo);
}