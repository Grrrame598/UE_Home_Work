// Общие типы и константы для системы оружия (ДЗ №5 «Пистолет»).

#pragma once

#include "CoreMinimal.h"
#include "MyProjectTypes.generated.h"

/**
 * Канал трассировки для пуль.
 * Создаётся в Project Settings -> Collision -> Trace Channels: "Bullet".
 * Project Settings пишет его в DefaultEngine.ini как ECC_GameTraceChannel1.
 */
#define ECC_Bullet ECC_GameTraceChannel1

/** Сокет на скелете персонажа, к которому крепится оружие (правая кисть) */
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");

/** Имя параметра Niagara, куда передаём конец трассы пули */
const FName FXParamTraceEnd = FName("TraceEnd");

/** Тип экипируемого предмета. По нему AnimBP выбирает позу персонажа */
UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None,
	Pistol
};

/** Откуда производится выстрел (рекомендация из задания: два типа выстрелов) */
UENUM(BlueprintType)
enum class EShotOrigin : uint8
{
	/** Из камеры игрока — стреляем туда, куда смотрит прицел */
	Camera,
	/** Из дула оружия — стреляем по направлению ствола */
	Muzzle
};
