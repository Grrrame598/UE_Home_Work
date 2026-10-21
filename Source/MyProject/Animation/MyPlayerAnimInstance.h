// Анимационный инстанс персонажа: передаёт в AnimBP данные для оружия и Aim Offset.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types/MyProjectTypes.h"
#include "MyPlayerAnimInstance.generated.h"

class AMyPlayerCharacter;

/**
 * Базовый класс для AnimBP персонажа (в BP: Class Settings -> Parent Class = MyPlayerAnimInstance).
 *
 * Отдаёт в граф:
 *  - CurrentEquippedItemType — есть оружие или нет (переключение позы);
 *  - AimRotation / AimPitch / AimYaw — входы Aim Offset (целиться верхней частью тела, без ног);
 *  - Speed / Direction — для бленд-спейсов.
 */
UCLASS()
class MYPROJECT_API UMyPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/** Тип экипированного предмета: None — обычный idle, Pistol — поза с оружием */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	EEquipableItemType CurrentEquippedItemType = EEquipableItemType::None;

	/** Полный Aim Rotation персонажа (как в уроке) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	FRotator AimRotation = FRotator::ZeroRotator;

	/** Pitch взгляда, ограничен ±90 — вход Aim Offset по вертикали */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float AimPitch = 0.f;

	/** Разница Yaw взгляда и тела, ±180 — вход Aim Offset по горизонтали */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float AimYaw = 0.f;

	/** Горизонтальная скорость персонажа (для бленд-спейса) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float Speed = 0.f;

	/** Направление движения относительно персонажа, -180..180 (аналог CalculateDirection):
	 *  0 — вперёд, +90 — вправо, ±180 — назад, -90 — влево. Вход Direction бленд-спейса. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float Direction = 0.f;

	/** Персонаж, за которым закреплён этот AnimBP */
	UPROPERTY(Transient)
	TObjectPtr<AMyPlayerCharacter> CachedCharacter = nullptr;

	/** Стрейф-локомоция включена (персонаж с оружием смотрит по камере, ноги идут в сторону).
	 *  Аналог bIsStrafing из референсного проекта: bIsStrafing = !bOrientRotationToMovement. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsStrafing = false;
};
