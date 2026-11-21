// B1: контроллер игрока — владелец Enhanced Input (шаг 2 ТЗ).

#include "GameModes/MyPlayerController.h"
#include "UI/MyHUD.h"

#include "Characters/MyPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "MyProject.h"

AMyPlayerController::AMyPlayerController()
{
	// Управление берём на себя в BeginPlay/SetupInputComponent, тик не нужен
	bAutoManageActiveCameraTarget = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	EnsureInputDefaults();
	AddMappingContexts();
}

void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MyCharacter = Cast<AMyPlayerCharacter>(InPawn);
	UE_LOG(LogMyProject, Log, TEXT("[Controller] %s: управляет %s"), *GetName(), *GetNameSafe(InPawn));

	// Возможно, Pawn уже владел действиями до Possess — контексты добавляем и здесь
	AddMappingContexts();
}

void AMyPlayerController::OnUnPossess()
{
	MyCharacter = nullptr;
	Super::OnUnPossess();
}

void AMyPlayerController::AddMappingContexts()
{
	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	if (GroundMappingContext)
	{
		Subsystem->AddMappingContext(GroundMappingContext, 0);
	}
	if (SwimMappingContext)
	{
		Subsystem->AddMappingContext(SwimMappingContext, 0);
	}
	if (WeaponMappingContext)
	{
		Subsystem->AddMappingContext(WeaponMappingContext, 0);
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	EnsureInputDefaults();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogMyProject, Error, TEXT("[Controller] %s: нет UEnhancedInputComponent — проверь DefaultInputComponentClass"), *GetName());
		return;
	}

	if (JumpAction)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyPlayerController::OnJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyPlayerController::OnJumpCompleted);
	}
	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerController::Move);
	}
	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayerController::Look);
	}
	if (SwimForwardAction)
	{
		EnhancedInputComponent->BindAction(SwimForwardAction, ETriggerEvent::Triggered, this, &AMyPlayerController::SwimForward);
	}
	if (SwimRightAction)
	{
		EnhancedInputComponent->BindAction(SwimRightAction, ETriggerEvent::Triggered, this, &AMyPlayerController::SwimRight);
	}
	if (SwimUpAction)
	{
		EnhancedInputComponent->BindAction(SwimUpAction, ETriggerEvent::Triggered, this, &AMyPlayerController::SwimUp);
	}
	if (SprintAction)
	{
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyPlayerController::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyPlayerController::OnSprintCompleted);
	}
	if (CrouchAction)
	{
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyPlayerController::OnCrouchToggle);
	}
	if (InteractAction)
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMyPlayerController::OnInteract);
	}
	if (FireAction)
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyPlayerController::OnFire);
	}
	if (AltFireAction)
	{
		EnhancedInputComponent->BindAction(AltFireAction, ETriggerEvent::Started, this, &AMyPlayerController::OnAltFire);
	}
	if (ToggleWeaponAction)
	{
		EnhancedInputComponent->BindAction(ToggleWeaponAction, ETriggerEvent::Started, this, &AMyPlayerController::OnToggleWeapon);
	}
	if (ToggleShotOriginAction)
	{
		EnhancedInputComponent->BindAction(ToggleShotOriginAction, ETriggerEvent::Started, this, &AMyPlayerController::OnToggleShotOrigin);
	}
	if (ReloadAction)
	{
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMyPlayerController::OnReload);
	}
}

void AMyPlayerController::EnsureInputDefaults()
{
	// --- Наземное управление ---
	if (!MoveAction)
	{
		MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
		MoveAction->ValueType = EInputActionValueType::Axis2D;
		MoveAction->bConsumeInput = false;
	}
	if (!LookAction)
	{
		LookAction = NewObject<UInputAction>(this, TEXT("IA_Look"));
		LookAction->ValueType = EInputActionValueType::Axis2D;
		LookAction->bConsumeInput = false;
	}
	if (!JumpAction)
	{
		JumpAction = NewObject<UInputAction>(this, TEXT("IA_Jump"));
		JumpAction->ValueType = EInputActionValueType::Boolean;
		JumpAction->bConsumeInput = false;
	}
	if (!SprintAction)
	{
		SprintAction = NewObject<UInputAction>(this, TEXT("IA_Sprint"));
		SprintAction->ValueType = EInputActionValueType::Boolean;
		SprintAction->bConsumeInput = false;
	}
	if (!CrouchAction)
	{
		CrouchAction = NewObject<UInputAction>(this, TEXT("IA_Crouch"));
		CrouchAction->ValueType = EInputActionValueType::Boolean;
		CrouchAction->bConsumeInput = false;
	}
	if (!InteractAction)
	{
		InteractAction = NewObject<UInputAction>(this, TEXT("IA_Interact"));
		InteractAction->ValueType = EInputActionValueType::Boolean;
		InteractAction->bConsumeInput = false;
	}

	if (!GroundMappingContext)
	{
		GroundMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Ground"));

		// IA_Move (Axis2D): W/S — вперёд/назад (ось Y), A/D — влево/вправо (ось X).
		// Для цифровых клавиш ось задаётся модификаторами: Swizzle переносит X в Y.
		{
			FEnhancedActionKeyMapping& M = GroundMappingContext->MapKey(MoveAction, EKeys::W);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(GroundMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			M.Modifiers.Add(Swizzle);
		}
		{
			FEnhancedActionKeyMapping& M = GroundMappingContext->MapKey(MoveAction, EKeys::S);
			M.Modifiers.Add(NewObject<UInputModifierNegate>(GroundMappingContext));
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(GroundMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			M.Modifiers.Add(Swizzle);
		}
		{
			FEnhancedActionKeyMapping& M = GroundMappingContext->MapKey(MoveAction, EKeys::A);
			M.Modifiers.Add(NewObject<UInputModifierNegate>(GroundMappingContext));
		}
		GroundMappingContext->MapKey(MoveAction, EKeys::D);

		// Стрелки тоже оставляем рабочими
		{
			FEnhancedActionKeyMapping& M = GroundMappingContext->MapKey(MoveAction, EKeys::Up);
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(GroundMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			M.Modifiers.Add(Swizzle);
		}
		{
			FEnhancedActionKeyMapping& M = GroundMappingContext->MapKey(MoveAction, EKeys::Down);
			M.Modifiers.Add(NewObject<UInputModifierNegate>(GroundMappingContext));
			UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(GroundMappingContext);
			Swizzle->Order = EInputAxisSwizzle::YXZ;
			M.Modifiers.Add(Swizzle);
		}
		{
			FEnhancedActionKeyMapping& M = GroundMappingContext->MapKey(MoveAction, EKeys::Left);
			M.Modifiers.Add(NewObject<UInputModifierNegate>(GroundMappingContext));
		}
		GroundMappingContext->MapKey(MoveAction, EKeys::Right);

		// IA_Look (Axis2D): мышь
		GroundMappingContext->MapKey(LookAction, EKeys::Mouse2D);

		// IA_Jump: Space (в воде Space занят погружением — см. SwimMappingContext ниже)
		GroundMappingContext->MapKey(JumpAction, EKeys::SpaceBar);

		// Спринт (hold) и присед (toggle)
		GroundMappingContext->MapKey(SprintAction, EKeys::LeftShift);
		GroundMappingContext->MapKey(CrouchAction, EKeys::LeftControl);

		// Взаимодействие
		GroundMappingContext->MapKey(InteractAction, EKeys::E);
	}

	// --- Плавание ---
	if (!SwimForwardAction)
	{
		SwimForwardAction = NewObject<UInputAction>(this, TEXT("IA_SwimForward"));
		SwimForwardAction->ValueType = EInputActionValueType::Axis1D;
		SwimForwardAction->bConsumeInput = false;
	}
	if (!SwimRightAction)
	{
		SwimRightAction = NewObject<UInputAction>(this, TEXT("IA_SwimRight"));
		SwimRightAction->ValueType = EInputActionValueType::Axis1D;
		SwimRightAction->bConsumeInput = false;
	}
	if (!SwimUpAction)
	{
		SwimUpAction = NewObject<UInputAction>(this, TEXT("IA_SwimUp"));
		SwimUpAction->ValueType = EInputActionValueType::Axis1D;
		SwimUpAction->bConsumeInput = false;
	}

	if (!SwimMappingContext)
	{
		SwimMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Swim"));

		// SwimForward: W (+1), S (-1)
		SwimMappingContext->MapKey(SwimForwardAction, EKeys::W);
		FEnhancedActionKeyMapping& ForwardS = SwimMappingContext->MapKey(SwimForwardAction, EKeys::S);
		ForwardS.Modifiers.Add(NewObject<UInputModifierNegate>(SwimMappingContext));

		// SwimRight: D (+1), A (-1)
		SwimMappingContext->MapKey(SwimRightAction, EKeys::D);
		FEnhancedActionKeyMapping& RightA = SwimMappingContext->MapKey(SwimRightAction, EKeys::A);
		RightA.Modifiers.Add(NewObject<UInputModifierNegate>(SwimMappingContext));

		// SwimUp: Space (+1), LeftControl (-1)
		SwimMappingContext->MapKey(SwimUpAction, EKeys::SpaceBar);
		FEnhancedActionKeyMapping& UpCtrl = SwimMappingContext->MapKey(SwimUpAction, EKeys::LeftControl);
		UpCtrl.Modifiers.Add(NewObject<UInputModifierNegate>(SwimMappingContext));
	}

	// --- Оружие ---
	if (!FireAction)
	{
		FireAction = NewObject<UInputAction>(this, TEXT("IA_Fire"));
		FireAction->ValueType = EInputActionValueType::Boolean;
		FireAction->bConsumeInput = false;
	}
	if (!AltFireAction)
	{
		AltFireAction = NewObject<UInputAction>(this, TEXT("IA_AltFire"));
		AltFireAction->ValueType = EInputActionValueType::Boolean;
		AltFireAction->bConsumeInput = false;
	}
	if (!ToggleWeaponAction)
	{
		ToggleWeaponAction = NewObject<UInputAction>(this, TEXT("IA_ToggleWeapon"));
		ToggleWeaponAction->ValueType = EInputActionValueType::Boolean;
		ToggleWeaponAction->bConsumeInput = false;
	}
	if (!ToggleShotOriginAction)
	{
		ToggleShotOriginAction = NewObject<UInputAction>(this, TEXT("IA_ToggleShotOrigin"));
		ToggleShotOriginAction->ValueType = EInputActionValueType::Boolean;
		ToggleShotOriginAction->bConsumeInput = false;
	}

	if (!ReloadAction)
	{
		ReloadAction = NewObject<UInputAction>(this, TEXT("IA_Reload"));
		ReloadAction->ValueType = EInputActionValueType::Boolean;
		ReloadAction->bConsumeInput = false;
	}

	if (!WeaponMappingContext)
	{
		WeaponMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Weapon"));

		// Выстрел: левая кнопка мыши + правый триггер геймпада
		WeaponMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
		WeaponMappingContext->MapKey(FireAction, EKeys::Gamepad_RightTrigger);

		// Альтернативный выстрел (подствольник): правая кнопка мыши
		WeaponMappingContext->MapKey(AltFireAction, EKeys::RightMouseButton);

		// Надеть/снять оружие: Q
		WeaponMappingContext->MapKey(ToggleWeaponAction, EKeys::Q);

		// Переключить тип выстрела (камера/дуло): R
		WeaponMappingContext->MapKey(ToggleShotOriginAction, EKeys::R);

		// Перезарядка: F (R занят переключением типа выстрела)
		WeaponMappingContext->MapKey(ReloadAction, EKeys::F);
	}
}

void AMyPlayerController::Move(const FInputActionValue& Value)
{
	if (MyCharacter)
	{
		MyCharacter->Move(Value);
	}
}

void AMyPlayerController::Look(const FInputActionValue& Value)
{
	if (MyCharacter)
	{
		MyCharacter->Look(Value);
	}
}

void AMyPlayerController::SwimForward(const FInputActionValue& Value)
{
	if (MyCharacter)
	{
		MyCharacter->SwimForward(Value);
	}
}

void AMyPlayerController::SwimRight(const FInputActionValue& Value)
{
	if (MyCharacter)
	{
		MyCharacter->SwimRight(Value);
	}
}

void AMyPlayerController::SwimUp(const FInputActionValue& Value)
{
	if (MyCharacter)
	{
		MyCharacter->SwimUp(Value);
	}
}

void AMyPlayerController::OnJumpStarted()
{
	if (!MyCharacter || !MyCharacter->CanJumpPublic())
	{
		return;
	}
	MyCharacter->Jump();
}

void AMyPlayerController::OnJumpCompleted()
{
	if (MyCharacter)
	{
		MyCharacter->StopJumping();
	}
}

void AMyPlayerController::OnSprintStarted()
{
	if (MyCharacter)
	{
		MyCharacter->StartSprint();
	}
}

void AMyPlayerController::OnSprintCompleted()
{
	if (MyCharacter)
	{
		MyCharacter->StopSprint();
	}
}

void AMyPlayerController::OnCrouchToggle()
{
	if (MyCharacter)
	{
		MyCharacter->ChangeCrouchState();
	}
}

void AMyPlayerController::OnInteract()
{
	if (MyCharacter)
	{
		MyCharacter->Interact();
	}
}

void AMyPlayerController::OnFire()
{
	if (MyCharacter)
	{
		MyCharacter->Fire();
	}
}

void AMyPlayerController::OnAltFire()
{
	if (MyCharacter)
	{
		MyCharacter->AltFire();
	}
}

void AMyPlayerController::OnToggleWeapon()
{
	if (MyCharacter)
	{
		MyCharacter->ToggleWeapon();

		// A6: HUD должен показать новое оружие и его патроны
		if (AMyHUD* MyHud = Cast<AMyHUD>(GetHUD()))
		{
			MyHud->RefreshWeapon();
		}
	}
}

void AMyPlayerController::OnToggleShotOrigin()
{
	if (MyCharacter)
	{
		MyCharacter->ToggleShotOrigin();
	}
}

void AMyPlayerController::OnReload()
{
	if (MyCharacter)
	{
		MyCharacter->ReloadWeapon();
	}
}
