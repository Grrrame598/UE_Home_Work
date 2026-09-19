// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyPlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

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

public:
	AMyPlayerCharacter();

protected:
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
};
