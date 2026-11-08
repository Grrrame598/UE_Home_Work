// Взрывающийся снаряд: ExplosionComponent + взрыв при столкновении (ДЗ №6).

#include "Actors/Projectiles/ExplosiveProjectile.h"

#include "Components/Explosion/ExplosionComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(GetRootComponent());
}

void AExplosiveProjectile::OnProjectileLaunched()
{
	Super::OnProjectileLaunched();

	// Взрыв по столкновению: ProjectileMovement останавливается при BlockingHit
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AExplosiveProjectile::OnProjectileStop);
	}

	// Страховка: если за DetonationTime ничего не задели — подрываемся сами
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DetonationTimer, this, &AExplosiveProjectile::Detonate, DetonationTime, false);
	}
}

void AExplosiveProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	Detonate();
}

void AExplosiveProjectile::Detonate()
{
	if (bDetonated)
	{
		return;
	}
	bDetonated = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DetonationTimer);
	}

	if (ExplosionComponent)
	{
		ExplosionComponent->Explode(GetOwnerController());
	}

	Destroy();
}

AController* AExplosiveProjectile::GetOwnerController() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	return IsValid(OwnerPawn) ? OwnerPawn->GetController() : nullptr;
}
