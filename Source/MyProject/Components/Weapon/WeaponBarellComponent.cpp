// Компонент ствола: точка выстрела, трасса, урон и эффекты.

#include "WeaponBarellComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Types/MyProjectTypes.h"

UWeaponBarellComponent::UWeaponBarellComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponBarellComponent::Shot(const FVector& ShotStart, const FVector& ShotDirection, AController* InstigatorController)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	const FVector TraceStart = ShotStart;
	const FVector Direction = ShotDirection.GetSafeNormal();
	const FVector TraceFinish = TraceStart + Direction * FiringRange;

	// Не стреляем в самого себя: игнорируем владельца оружия и пешку стрелка
	FCollisionQueryParams QueryParams(FName(TEXT("WeaponShot")), false, GetOwner());
	if (InstigatorController && InstigatorController->GetPawn())
	{
		QueryParams.AddIgnoredActor(InstigatorController->GetPawn());
	}

	FHitResult HitResult;
	const bool bHasHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceFinish, ECC_Bullet, QueryParams);
	const FVector TraceEnd = bHasHit ? HitResult.ImpactPoint : TraceFinish;

	// Дульная вспышка — в точке этого компонента (то есть на дуле оружия)
	if (MuzzleFlashFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MuzzleFlashFX, GetComponentLocation(), GetComponentRotation());
	}

	// Попадание: наносим урон тому, в кого попали
	if (bHasHit && IsValid(HitResult.GetActor()))
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), DamageAmount, InstigatorController, GetOwner(), UDamageType::StaticClass());
	}

	// След пули
	if (TraceFX)
	{
		if (UNiagaraComponent* TraceComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, TraceFX, TraceStart))
		{
			TraceComponent->SetVariableVec3(FXParamTraceEnd, TraceEnd);
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		const FColor DebugColor = bHasHit ? FColor::Green : FColor::Red;
		DrawDebugLine(World, TraceStart, TraceEnd, DebugColor, false, 2.f, 0, 2.f);
		if (bHasHit)
		{
			DrawDebugPoint(World, HitResult.ImpactPoint, 10.f, DebugColor, false, 2.f);
		}
	}
#endif
}
