// Ввод и выстрел персонажа (вынесено отдельным файлом, чтобы не мешать
// существующей логике перемещения/плавания из ДЗ №4).

#include "MyPlayerCharacter.h"

#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"

void AMyPlayerCharacter::EnsureWeaponInputDefaults()
{
	if (!FireAction)
	{
		FireAction = NewObject<UInputAction>(this, TEXT("FireAction"));
		FireAction->ValueType = EInputActionValueType::Boolean;
		FireAction->bConsumeInput = false;
	}

	if (!ToggleWeaponAction)
	{
		ToggleWeaponAction = NewObject<UInputAction>(this, TEXT("ToggleWeaponAction"));
		ToggleWeaponAction->ValueType = EInputActionValueType::Boolean;
		ToggleWeaponAction->bConsumeInput = false;
	}

	if (!ToggleShotOriginAction)
	{
		ToggleShotOriginAction = NewObject<UInputAction>(this, TEXT("ToggleShotOriginAction"));
		ToggleShotOriginAction->ValueType = EInputActionValueType::Boolean;
		ToggleShotOriginAction->bConsumeInput = false;
	}

	if (!WeaponMappingContext)
	{
		WeaponMappingContext = NewObject<UInputMappingContext>(this, TEXT("WeaponMappingContext"));

		// Выстрел: левая кнопка мыши + правый триггер геймпада
		WeaponMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
		WeaponMappingContext->MapKey(FireAction, EKeys::Gamepad_RightTrigger);

		// Надеть/снять оружие: Q
		WeaponMappingContext->MapKey(ToggleWeaponAction, EKeys::Q);

		// Переключить тип выстрела (камера/дуло): R
		WeaponMappingContext->MapKey(ToggleShotOriginAction, EKeys::R);
	}
}

void AMyPlayerCharacter::Fire()
{
	if (CharacterEquipmentComponent)
	{
		CharacterEquipmentComponent->Fire();
	}
}

void AMyPlayerCharacter::ToggleWeapon()
{
	if (CharacterEquipmentComponent)
	{
		CharacterEquipmentComponent->ToggleEquip();

		// С оружием — стрейф-локомоция (корпус по камере), без оружия — обычный бег
		// с доворотом по направлению движения (как в референсном проекте).
		SetStrafeMode(CharacterEquipmentComponent->IsEquipped());
	}
}

void AMyPlayerCharacter::ToggleShotOrigin()
{
	if (CharacterEquipmentComponent)
	{
		CharacterEquipmentComponent->ToggleShotOrigin();
	}
}
