// Базовый снаряд: сфера + ProjectileMovement (ДЗ №6).

#include "Actors/Projectiles/MyProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMyProjectile::AMyProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(CollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->MaxSpeed = 4000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	// Без отскоков: при попадании снаряд останавливается (по этому событию взрывается наследник)
	ProjectileMovementComponent->bShouldBounce = false;
}

void AMyProjectile::LaunchProjectile(const FVector& Direction)
{
	if (!ProjectileMovementComponent)
	{
		return;
	}

	ProjectileMovementComponent->Velocity = Direction.GetSafeNormal() * ProjectileMovementComponent->InitialSpeed;

	// Не задевать стрелка при старте
	if (CollisionComponent && IsValid(GetOwner()))
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	}

	OnProjectileLaunched();
}

void AMyProjectile::OnProjectileLaunched()
{
}
