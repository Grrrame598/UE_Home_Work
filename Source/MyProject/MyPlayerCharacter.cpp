// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"


AMyPlayerCharacter::AMyPlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Параметры плавания
	GetCharacterMovement()->MaxSwimSpeed = 300.f;
	GetCharacterMovement()->Buoyancy = 1.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Существующие меш и AnimBP проекта (Manny)
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SKManny(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	if (SKManny.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SKManny.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> ABPManny(TEXT("/Game/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny_C"));
	if (ABPManny.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ABPManny.Class);
	}

	// Стандартные Input Actions шаблона (Move/Look/Jump).
	// Mapping Context для них создаём сами в EnsureInputDefaults(), чтобы не зависеть от настроек IMC_Default.
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveIA(TEXT("/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move"));
	if (MoveIA.Succeeded())
	{
		MoveAction = MoveIA.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookIA(TEXT("/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look"));
	if (LookIA.Succeeded())
	{
		LookAction = LookIA.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpIA(TEXT("/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpIA.Succeeded())
	{
		JumpAction = JumpIA.Object;
	}

	// Ground- и Swim-контексты используют одни и те же клавиши (W/A/S/D/Space).
	// По умолчанию у InputAction включён bConsumeInput, из-за чего один контекст
	// "забирает" клавиши у другого. Отключаем потребление ввода для всех действий.
	if (MoveAction)
	{
		MoveAction->bConsumeInput = false;
	}
	if (LookAction)
	{
		LookAction->bConsumeInput = false;
	}
	if (JumpAction)
	{
		JumpAction->bConsumeInput = false;
	}
}

void AMyPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	EnsureInputDefaults();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (GroundMappingContext)
			{
				Subsystem->AddMappingContext(GroundMappingContext, 0);
			}

			if (SwimMappingContext)
			{
				Subsystem->AddMappingContext(SwimMappingContext, 0);
			}
		}
	}
}

void AMyPlayerCharacter::EnsureInputDefaults()
{
	// --- Наземное управление: создаём собственный MappingContext ---
	if (!GroundMappingContext)
	{
		GroundMappingContext = NewObject<UInputMappingContext>(this, TEXT("GroundMappingContext"));

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

		// IA_Jump: Space
		GroundMappingContext->MapKey(JumpAction, EKeys::SpaceBar);
	}

	// --- Плавание ---
	if (!SwimForwardAction)
	{
		SwimForwardAction = NewObject<UInputAction>(this, TEXT("SwimForwardAction"));
		SwimForwardAction->ValueType = EInputActionValueType::Axis1D;
		SwimForwardAction->bConsumeInput = false;
	}

	if (!SwimRightAction)
	{
		SwimRightAction = NewObject<UInputAction>(this, TEXT("SwimRightAction"));
		SwimRightAction->ValueType = EInputActionValueType::Axis1D;
		SwimRightAction->bConsumeInput = false;
	}

	if (!SwimUpAction)
	{
		SwimUpAction = NewObject<UInputAction>(this, TEXT("SwimUpAction"));
		SwimUpAction->ValueType = EInputActionValueType::Axis1D;
		SwimUpAction->bConsumeInput = false;
	}

	if (!SwimMappingContext)
	{
		SwimMappingContext = NewObject<UInputMappingContext>(this, TEXT("SwimMappingContext"));

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
}

void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyPlayerCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyPlayerCharacter::StopJumping);
		}

		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerCharacter::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayerCharacter::Look);
		}

		if (SwimForwardAction)
		{
			EnhancedInputComponent->BindAction(SwimForwardAction, ETriggerEvent::Triggered, this, &AMyPlayerCharacter::SwimForward);
		}

		if (SwimRightAction)
		{
			EnhancedInputComponent->BindAction(SwimRightAction, ETriggerEvent::Triggered, this, &AMyPlayerCharacter::SwimRight);
		}

		if (SwimUpAction)
		{
			EnhancedInputComponent->BindAction(SwimUpAction, ETriggerEvent::Triggered, this, &AMyPlayerCharacter::SwimUp);
		}
	}
}

void AMyPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Обычное перемещение только на земле/в падении; в воде управляют Swim-функции
	if (Controller != nullptr && (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()))
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMyPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyPlayerCharacter::SwimForward(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();

	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(AxisValue, 1e-6f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator PitchYawRotation(ControlRotation.Pitch, ControlRotation.Yaw, 0.0f);
		const FVector ForwardDirection = PitchYawRotation.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardDirection, AxisValue);
	}
}

void AMyPlayerCharacter::SwimRight(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();

	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(AxisValue, 1e-6f))
	{
		const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		const FVector RightDirection = YawRotation.RotateVector(FVector::RightVector);

		AddMovementInput(RightDirection, AxisValue);
	}
}

void AMyPlayerCharacter::SwimUp(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();

	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(AxisValue, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, AxisValue);
	}
}

bool AMyPlayerCharacter::CanJumpInternal_Implementation() const
{
	// В воде прыжок как на суше запрещаем
	if (GetCharacterMovement()->IsSwimming())
	{
		return false;
	}

	return Super::CanJumpInternal_Implementation();
}
