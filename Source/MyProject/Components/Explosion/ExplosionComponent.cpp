// Компонент взрыва: урон по области, VFX и импульс физическим объектам (ДЗ №6).

#include "Components/Explosion/ExplosionComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

void UExplosionComponent::Explode(AController* InstigatorController)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector ExplosionLocation = GetComponentLocation();

	// Урон по области: от MaxDamage в центре до MinDamage на OuterRadius.
	// Снаряд и его владелец-персонаж не должны ловить свой же урон.
	TArray<AActor*> IgnoredActors;
	if (AActor* OwnerActor = GetOwner())
	{
		IgnoredActors.Add(OwnerActor);
		if (AActor* OwnerInstigator = OwnerActor->GetOwner())
		{
			IgnoredActors.Add(OwnerInstigator);
		}
	}

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		World,
		MaxDamage,
		MinDamage,
		ExplosionLocation,
		InnerRadius,
		OuterRadius,
		DamageFalloff,
		DamageTypeClass,
		IgnoredActors,
		GetOwner(),
		InstigatorController,
		ECC_Visibility);

	// «Взрывная волна»: толкаем физические объекты в радиусе
	const FCollisionShape ImpulseShape = FCollisionShape::MakeSphere(OuterRadius);
	const FCollisionQueryParams ImpulseParams(SCENE_QUERY_STAT(ExplosionImpulse), false, GetOwner());

	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByChannel(
		Overlaps, ExplosionLocation, FQuat::Identity, ECC_WorldDynamic, ImpulseShape, ImpulseParams);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		UPrimitiveComponent* OverlappedComponent = Overlap.GetComponent();
		if (OverlappedComponent && OverlappedComponent->IsSimulatingPhysics())
		{
			OverlappedComponent->AddRadialImpulse(
				ExplosionLocation, OuterRadius, ImpulseStrength, ERadialImpulseFalloff::RIF_Linear, bImpulseVelChange);
		}
	}

	if (IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, ExplosionVFX, ExplosionLocation);
	}

	if (OnExplosion.IsBound())
	{
		OnExplosion.Broadcast();
	}
}
