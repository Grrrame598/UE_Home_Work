// Базовый абстрактный класс оружия (ДЗ №5).

#include "BaseWeapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(false);

	// Компонент ствола крепится к мешу без сокета: в BP его просто ставят на дуло.
	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh);
}

void ABaseWeapon::GetShotOrigin(FVector& OutStart, FVector& OutDirection, AController*& OutInstigator) const
{
	OutInstigator = nullptr;

	// Владельцем оружия является персонаж (см. UCharacterEquipmentComponent)
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		OutInstigator = OwnerPawn->GetController();
	}

	// Тип выстрела №1: из камеры игрока
	if (ShotOriginType == EShotOrigin::Camera && IsValid(OutInstigator))
	{
		FVector ViewLocation = FVector::ZeroVector;
		FRotator ViewRotation = FRotator::ZeroRotator;
		OutInstigator->GetPlayerViewPoint(ViewLocation, ViewRotation);

		OutStart = ViewLocation;
		OutDirection = ViewRotation.Vector();
		return;
	}

	// Тип выстрела №2: из дула оружия
	OutStart = WeaponBarell ? WeaponBarell->GetComponentLocation() : GetActorLocation();
	OutDirection = WeaponBarell ? WeaponBarell->GetForwardVector() : GetActorForwardVector();
}

void ABaseWeapon::Shot()
{
	FVector ShotStart = FVector::ZeroVector;
	FVector ShotDirection = GetActorForwardVector();
	AController* InstigatorController = nullptr;

	GetShotOrigin(ShotStart, ShotDirection, InstigatorController);

	if (!WeaponBarell)
	{
		return;
	}

	// Вся логика (трасса, урон, эффекты) живёт в компоненте ствола
	WeaponBarell->Shot(ShotStart, ShotDirection, InstigatorController);
}

void ABaseWeapon::ToggleShotOrigin()
{
	// Рекомендация из задания: два типа выстрела — из камеры игрока и из дула оружия
	ShotOriginType = (ShotOriginType == EShotOrigin::Camera) ? EShotOrigin::Muzzle : EShotOrigin::Camera;

	const FString Message = FString::Printf(TEXT("[%s] Shot origin: %s"), *GetName(),
		ShotOriginType == EShotOrigin::Camera ? TEXT("Camera") : TEXT("Muzzle"));

	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);

	if (GEngine)
	{
		// GetUniqueID() возвращает uint32, а перегрузки принимают int32/uint64 — приводим явно
		GEngine->AddOnScreenDebugMessage(static_cast<int32>(GetUniqueID()), 2.f, FColor::Cyan, Message);
	}
}
