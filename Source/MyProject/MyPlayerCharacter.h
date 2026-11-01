// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyPlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCharacterEquipmentComponent;

/**
 * Собственный персонаж для задания «Продвинутое перемещение» (плавание).
 * Каркас: ACharacter + SpringArm + Camera + ориентирование по движению.
 */
UCLASS()
class MYPROJECT_API AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Mapping context наземного управления (Move/Look/Jump). Создаётся в C++, если не назначен в BP */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* GroundMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Swim forward Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwimForwardAction;

	/** Swim right Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwimRightAction;

	/** Swim up/down Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwimUpAction;

	/** Mapping context для осей плавания */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SwimMappingContext;

	/** Fire Input Action: левая кнопка мыши / правый триггер геймпада */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	/** Надеть/снять оружие (Q) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleWeaponAction;

	/** Переключение типа выстрела камера/дуло (R) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleShotOriginAction;

	/** Mapping context для оружия (Fire / ToggleWeapon) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WeaponMappingContext;

public:
	AMyPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual bool CanJumpInternal_Implementation() const override;

	/** Обычное перемещение по земле */
	void Move(const FInputActionValue& Value);

	/** Поворот камеры */
	void Look(const FInputActionValue& Value);

	/** Плавание: вперед/назад с учетом Pitch+Yaw камеры */
	void SwimForward(const FInputActionValue& Value);

	/** Плавание: влево/вправо с учетом Yaw камеры */
	void SwimRight(const FInputActionValue& Value);

	/** Плавание: вверх/вниз по мировому Up */
	void SwimUp(const FInputActionValue& Value);

	/** Создаёт IA_Fire / IA_ToggleWeapon и WeaponMappingContext, если не назначены в BP */
	void EnsureWeaponInputDefaults();

	/** Нажатие кнопки выстрела — пробрасывается в текущее оружие */
	void Fire();

	/** Надеть/снять оружие */
	void ToggleWeapon();

	/** Переключить тип выстрела (камера / дуло) */
	void ToggleShotOrigin();

	/**
	 * Создает дефолтные InputAction и MappingContext для наземного управления и плавания,
	 * если они не назначены в Blueprint. Нужно, чтобы механика работала без редакторских ассетов.
	 */
	void EnsureInputDefaults();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns CharacterEquipmentComponent subobject **/
	FORCEINLINE class UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const { return CharacterEquipmentComponent; }

	/**
	 * Включает/выключает режим стрейфа (как в референсном проекте: bIsStrafing = !bOrientRotationToMovement).
	 * В стрейфе корпус всегда смотрит по камере (bUseControllerRotationYaw = true), а WASD уводит
	 * перемещение в ноги — благодаря этому Direction в бленд-спейсе становится ненулевым
	 * и играются боковые/задние анимации вместо простого доворота персонажа.
	 */
	void SetStrafeMode(bool bEnabled);

	/** Активен ли режим стрейфа (стрейф-локомоция) */
	UFUNCTION(BlueprintPure, Category = "Character | Movement")
	bool IsStrafing() const { return bIsStrafing; }

	/** Компонент экипировки: выдает оружие и переключает его наличие */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterEquipmentComponent* CharacterEquipmentComponent;

protected:
	/** Режим стрейфа включён (персонаж с оружием: корпус по камере, ноги — в сторону движения) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	bool bIsStrafing = false;
};
