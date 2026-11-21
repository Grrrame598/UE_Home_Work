// Огнестрельное оружие (пистолет) — наследник ABaseWeapon.

#include "RangeWeaponItem.h"

#include "MyProject.h"
#include "Actors/Projectiles/MyProjectile.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

// --- A5: патроны -----------------------------------------------------------

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();

	// Раскладываем стартовые значения по правилам (обойма не больше магазина)
	AmmoInMag = FMath::Clamp(AmmoInMag, 0, MagSize);

	if (!bInfiniteReserve && MaxReserveAmmo > 0)
	{
		ReserveAmmo = FMath::Clamp(ReserveAmmo, 0, MaxReserveAmmo);
	}

	OnAmmoChanged.Broadcast(GetAmmoInMag(), GetReserveAmmo());
}

void ARangeWeaponItem::SetAmmoInMag(int32 NewAmmo)
{
	AmmoInMag = FMath::Clamp(NewAmmo, 0, MagSize);
	OnAmmoChanged.Broadcast(GetAmmoInMag(), GetReserveAmmo());
}

void ARangeWeaponItem::SetReserveAmmo(int32 NewReserve)
{
	ReserveAmmo = MaxReserveAmmo > 0 ? FMath::Clamp(NewReserve, 0, MaxReserveAmmo) : FMath::Max(NewReserve, 0);
	OnAmmoChanged.Broadcast(GetAmmoInMag(), GetReserveAmmo());
}

void ARangeWeaponItem::StartReload()
{
	// Уже перезаряжаемся или обойма полна — ничего не делаем
	if (bIsReloading || AmmoInMag >= MagSize)
	{
		return;
	}

	// Перезаряжаться нечем (реальный запас кончился)
	if (!bInfiniteReserve && ReserveAmmo <= 0)
	{
		UE_LOG(LogMyProject, Warning, TEXT("[%s] перезарядка: запас пуст"), *GetName());
		return;
	}

	bIsReloading = true;
	UE_LOG(LogMyProject, Log, TEXT("[%s] начало перезарядки (%.2f с)"), *GetName(), ReloadDuration);

	// Мгновенный вариант (ReloadDuration = 0) — без таймера
	if (ReloadDuration <= 0.f)
	{
		FinishReload();
		return;
	}

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle, this, &ARangeWeaponItem::FinishReload, ReloadDuration, false);
}

void ARangeWeaponItem::FinishReload()
{
	bIsReloading = false;
	Reload();
}

bool ARangeWeaponItem::Reload()
{
	if (bIsReloading)
	{
		return false;
	}

	const int32 Missing = MagSize - AmmoInMag;

	if (Missing <= 0)
	{
		return false;
	}

	int32 AmountToLoad = Missing;

	if (!bInfiniteReserve)
	{
		// С реальным запасом берём столько, сколько есть
		AmountToLoad = FMath::Min(Missing, ReserveAmmo);
		SetReserveAmmo(ReserveAmmo - AmountToLoad);
	}

	if (AmountToLoad <= 0)
	{
		UE_LOG(LogMyProject, Warning, TEXT("[%s] перезарядка: запас пуст"), *GetName());
		return false;
	}

	SetAmmoInMag(AmmoInMag + AmountToLoad);
	UE_LOG(LogMyProject, Log, TEXT("[%s] перезарядка: %d / %d (запас %d)"),
		*GetName(), GetAmmoInMag(), MagSize, GetReserveAmmo());

	return true;
}


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
	// 0. Патроны: без них только сухой клик
	if (!HasAmmo())
	{
		// Во время перезарядки не спамим лог на каждый клик
		if (bIsReloading)
		{
			UE_LOG(LogMyProject, Verbose, TEXT("[%s] сухой клик (идёт перезарядка)"), *GetName());
		}
		else
		{
			UE_LOG(LogMyProject, Log, TEXT("[%s] сухой клик: обойма пуста"), *GetName());
		}

		if (bAutoReload && !bIsReloading)
		{
			StartReload();
		}

		return;
	}

	SetAmmoInMag(AmmoInMag - 1);
	UE_LOG(LogMyProject, Log, TEXT("[%s] выстрел: %d / %d"), *GetName(), GetAmmoInMag(), GetMagSize());

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
		UE_LOG(LogMyProject, Warning, TEXT("[%s] AltShot: не задан GrenadeProjectileClass"), *GetName());
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
