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

	/** Альтернативный выстрел: правая кнопка мыши (подствольник) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AltFireAction;

	/** Надеть/снять оружие (Q) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleWeaponAction;

	/** Переключение типа выстрела камера/дуло (R) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleShotOriginAction;

	/** Mapping context для оружия (Fire / ToggleWeapon) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WeaponMappingContext;

	/** Спринт (LeftShift, удержание) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Присед (LeftCtrl, переключение) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Скорость спринта, см/с */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 800.f;

	/** Обычная скорость ходьбы, см/с (к ней возвращаемся после спринта) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed = 500.f;

public:
	AMyPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual bool CanJumpInternal_Implementation() const override;

	/**
	 * Лог входа/выхода из воды (шаг 4 ТЗ, критерий «логирование в критических точках»).
	 * Ловит ЛЮБОЙ источник режима плавания — PhysicsVolume с Water Volume, собственный
	 * триггер, Water Plugin: лог не привязан к конкретному актору воды.
	 */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

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

	/** Спринт: ускорение + флаг для AnimBP */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void StartSprint();

	/** Конец спринта: вернуть обычную скорость */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void StopSprint();

	/** Переключить присед (LeftCtrl) */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void ChangeCrouchState();

	/** Можно ли сейчас бежать (не в воде, не в приседе, есть стамина) */
	UFUNCTION(BlueprintPure, Category = "Character | Movement")
	bool CanSprint() const;

	/** Спринт (LeftShift, удержание) */
	void OnSprintStarted();
	void OnSprintCompleted();

	/** Присед: камера через spring arm */
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	/** Бег включён (читает AnimInstance, поэтому public) */
	UFUNCTION(BlueprintPure, Category = "Character | Movement")
	bool IsSprinting() const { return bIsSprinting; }

	/** Returns CameraBoom subobject **/

	/** Правая кнопка мыши — альтернативный выстрел (снаряд из подствольника) */
	void AltFire();

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

	/** Бег включён (для AnimBP и стамины в A3) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	bool bIsSprinting = false;
};
