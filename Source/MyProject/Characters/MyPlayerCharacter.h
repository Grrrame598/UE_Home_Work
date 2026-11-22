// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyPlayerCharacter.generated.h"

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

	/** Скорость спринта, см/с */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 800.f;

	/** Обычная скорость ходьбы, см/с (к ней возвращаемся после спринта) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement", meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed = 500.f;

public:
	AMyPlayerCharacter();

	// --- B1: управление живёт в AMyPlayerController, он вызывает эти хендлеры ---

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

	/** Присед: переключение состояния */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void ChangeCrouchState();


	/** Спринт: ускорение + флаг для AnimBP */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void StartSprint();

	/** Конец спринта: вернуть обычную скорость (зовётся и из компонента атрибутов на нуле стамины) */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void StopSprint();

	/** Запрет бега извне (зона замедления, B2) */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void SetSprintSuppressed(bool bSuppressed) { bCanSprintSuppressed = !bSuppressed; }

	/** Множитель скорости ходьбы (1.0 — обычная). Зона замедления ставит 0.5 */
	UFUNCTION(BlueprintCallable, Category = "Character | Movement")
	void ApplySpeedMultiplier(float Multiplier);


protected:
	virtual void BeginPlay() override;

	/** B1: прыжок инициирует контроллер — он проверяет CanJump перед вызовом Jump() */
	virtual bool CanJumpInternal_Implementation() const override;

public:
	/** Публичная обёртка: контроллер не может звать protected CanJumpInternal_Implementation */
	bool CanJumpPublic() const { return CanJumpInternal_Implementation(); }

protected:

	/**
	 * Лог входа/выхода из воды (шаг 4 ТЗ, критерий «логирование в критических точках»).
	 * Ловит ЛЮБОЙ источник режима плавания — PhysicsVolume с Water Volume, собственный
	 * триггер, Water Plugin: лог не привязан к конкретному актору воды.
	 */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	/** Дальность взаимодействия по E, см */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Interaction")
	float InteractTraceDistance = 300.f;

	/** Радиус сферы при E, см — прощает неточное прицеливание */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Interaction", meta = (ClampMin = "1.0"))
	float InteractTraceRadius = 60.f;

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

	/** Взаимодействие (E): трассировка вперёд, вызов Interact() у интерактивного актора */
	UFUNCTION(BlueprintCallable, Category = "Character | Interaction")
	void Interact();

	/** A5: перезарядка текущего оружия (F) */
	UFUNCTION(BlueprintCallable, Category = "Character | Weapon")
	void ReloadWeapon();

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

	/** Returns CharacterAttributesComponent subobject (HP, стамина) **/
	FORCEINLINE class UCharacterAttributesComponent* GetCharacterAttributesComponent() const { return CharacterAttributesComponent; }

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

	/** Компонент атрибутов: HP и стамина (A3) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterAttributesComponent* CharacterAttributesComponent;

protected:
	/** Режим стрейфа включён (персонаж с оружием: корпус по камере, ноги — в сторону движения) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	bool bIsStrafing = false;

	/** Бег включён (для AnimBP и стамины в A3) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	bool bIsSprinting = false;

	/** Бег запрещён снаружи (зона замедления, B2) */
	bool bCanSprintSuppressed = false;
};
