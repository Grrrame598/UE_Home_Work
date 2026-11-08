// Огнестрельное оружие (пистолет) — наследник ABaseWeapon.

#include "RangeWeaponItem.h"

#include "Actors/Projectiles/MyProjectile.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	// Тип предмета: по нему AnimBP персонажа включает позу «с пистолетом»
	ItemType = EEquipableItemType::Pistol;

	// Подствольник: точка под стволом, откуда вылетает снаряд (положение задаётся в BP)
	UnderbarrelBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("UnderbarrelBarell"));
	UnderbarrelBarell->SetupAttachment(WeaponMesh);
}

void ARangeWeaponItem::Shot()
{
	// 1. Анимации (оружие + персонаж)
	PlayWeaponFireMontage();

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		if (CharacterFireMontage && CharacterOwner->GetMesh())
		{
			if (UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance())
			{
				CharacterAnimInstance->Montage_Play(CharacterFireMontage);
			}
		}
	}

	// 2. Сам выстрел — базовая реализация из ABaseWeapon (трасса + урон + эффекты)
	Super::Shot();
}

void ARangeWeaponItem::AltShot()
{
	UWorld* World = GetWorld();
	if (!World || !GrenadeProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] AltShot: не задан GrenadeProjectileClass"), *GetName());
		return;
	}

	// Направление берём тем же способом, что и для обычного выстрела:
	// из камеры игрока либо по направлению ствола (переключается клавишей R).
	FVector ShotStart = FVector::ZeroVector;
	FVector ShotDirection = GetActorForwardVector();
	AController* InstigatorController = nullptr;
	GetShotOrigin(ShotStart, ShotDirection, InstigatorController);

	// А спавним снаряд из подствольника, а не из камеры
	const FVector SpawnLocation = UnderbarrelBarell ? UnderbarrelBarell->GetComponentLocation() : GetActorLocation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = Cast<APawn>(GetOwner());
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMyProjectile* Projectile = World->SpawnActor<AMyProjectile>(
		GrenadeProjectileClass, SpawnLocation, ShotDirection.Rotation(), SpawnParameters);

	if (!Projectile)
	{
		return;
	}

	Projectile->LaunchProjectile(ShotDirection);

	// Анимация отдачи персонажа — тот же монтаж, что у обычного выстрела
	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		if (CharacterFireMontage && CharacterOwner->GetMesh())
		{
			if (UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance())
			{
				CharacterAnimInstance->Montage_Play(CharacterFireMontage);
			}
		}
	}
}

float ARangeWeaponItem::PlayWeaponFireMontage()
{
	if (!WeaponFireMontage)
	{
		return 0.f;
	}

	USkeletalMeshComponent* MeshComponent = GetWeaponMesh();
	if (!MeshComponent)
	{
		return 0.f;
	}

	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		return AnimInstance->Montage_Play(WeaponFireMontage);
	}

	return 0.f;
}
