// Анимационный инстанс персонажа: передаёт в AnimBP данные для оружия и Aim Offset.

#include "MyPlayerAnimInstance.h"

#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "MyPlayerCharacter.h"

void UMyPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CachedCharacter = Cast<AMyPlayerCharacter>(TryGetPawnOwner());
}

void UMyPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedCharacter)
	{
		CachedCharacter = Cast<AMyPlayerCharacter>(TryGetPawnOwner());
	}

	if (!CachedCharacter)
	{
		return;
	}

	// Данные для Aim Offset
	AimRotation = CachedCharacter->GetBaseAimRotation();
	AimPitch = FMath::ClampAngle(AimRotation.Pitch, -90.f, 90.f);
	AimYaw = FRotator::NormalizeAxis(AimRotation.Yaw - CachedCharacter->GetActorRotation().Yaw);

	// Поза зависит от того, экипировано ли оружие
	if (const UCharacterEquipmentComponent* EquipmentComponent = CachedCharacter->GetCharacterEquipmentComponent())
	{
		CurrentEquippedItemType = EquipmentComponent->GetCurrentEquippedItemType();
	}

	// Для бленд-спейсов движения: горизонтальная скорость и направление относительно персонажа
	const FVector Velocity = CachedCharacter->GetVelocity();
	Speed = Velocity.Size2D();

	// Стрейф-локомоция: корпус по камере, ноги — по направлению ввода
	bIsStrafing = CachedCharacter->IsStrafing();

	// Аналог UKismetAnimationLibrary::CalculateDirection — чтобы не зависеть от нод шаблона в AnimGraph
	Direction = 0.f;
	if (!Velocity.IsNearlyZero())
	{
		const FVector NormalizedVelocity = Velocity.GetSafeNormal2D();
		const FMatrix ActorRotationMatrix = FRotationMatrix(CachedCharacter->GetActorRotation());
		const FVector ForwardVector = ActorRotationMatrix.GetScaledAxis(EAxis::X);
		const FVector RightVector = ActorRotationMatrix.GetScaledAxis(EAxis::Y);

		const float ForwardDeltaDegrees = FMath::RadiansToDegrees(
			FMath::Acos(FMath::Clamp(FVector::DotProduct(ForwardVector, NormalizedVelocity), -1.f, 1.f)));

		Direction = FVector::DotProduct(RightVector, NormalizedVelocity) < 0.f ? -ForwardDeltaDegrees : ForwardDeltaDegrees;
	}
}
