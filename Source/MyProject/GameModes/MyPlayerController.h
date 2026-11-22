// B1: управление персонажем вынесено в контроллер (шаг 2 ТЗ).
// Контроллер владеет Input Action / Mapping Context и биндингами Enhanced Input;
// персонаж остаётся владельцем камеры, поворота и самой логики перемещения.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AMyPlayerCharacter;

UCLASS()
class MYPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	/** Персонаж-владелец (кэш, обновляется в OnPossess/OnUnPossess) */
	UFUNCTION(BlueprintPure, Category = "Controller")
	AMyPlayerCharacter* GetMyCharacter() const { return MyCharacter; }

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void SetupInputComponent() override;

	/** Создаёт Input Action и Mapping Context в C++, если они не назначены в BP */
	void EnsureInputDefaults();

	/** Добавляет все Mapping Context'ы в Enhanced Input Subsystem локального игрока */
	void AddMappingContexts();

	// --- Хендлеры Enhanced Input: только проброс в персонажа ---

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SwimForward(const FInputActionValue& Value);
	void SwimRight(const FInputActionValue& Value);
	void SwimUp(const FInputActionValue& Value);

	void OnJumpStarted();
	void OnJumpCompleted();
	void OnSprintStarted();
	void OnSprintCompleted();
	void OnCrouchToggle();
	void OnInteract();
	void OnFire();
	void OnAltFire();
	void OnToggleWeapon();
	void OnToggleShotOrigin();

	/** A5: перезарядка текущего оружия (F) */
	void OnReload();

	// --- Ассеты ввода ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* GroundMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* SwimMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* WeaponMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwimForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwimRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwimUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AltFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleShotOriginAction;

	/** A5: перезарядка (F) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;

private:
	UPROPERTY(Transient)
	AMyPlayerCharacter* MyCharacter = nullptr;
};
