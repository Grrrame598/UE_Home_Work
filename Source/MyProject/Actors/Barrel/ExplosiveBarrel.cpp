// B3: взрывающаяся бочка.

#include "Actors/Barrel/ExplosiveBarrel.h"

#include "Components/StaticMeshComponent.h"
#include "Components/Explosion/ExplosionComponent.h"
#include "Engine/StaticMesh.h"
#include "Particles/ParticleSystem.h"
#include "MyProject.h"
#include "UObject/ConstructorHelpers.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	PrimaryActorTick.bCanEverTick = false;

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
	SetRootComponent(BarrelMesh);
	// Меш по умолчанию из StarterContent; в BP можно заменить на любую бочку.
	// LoadObject, а не FObjectFinder: отсутствие ассета не должно ронять CDO.
	BarrelMesh->SetStaticMesh(LoadObject<UStaticMesh>(
		nullptr, TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder")));
	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BarrelMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	BarrelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(BarrelMesh);

	// VFX по умолчанию: партикл из GameCode, иначе StarterContent. Без него взрыв «немой».
	ExplosionComponent->SetExplosionVFX(LoadObject<UParticleSystem>(
		nullptr, TEXT("/Game/GameCode/Core/VFX/Explosives/FX/Particles/P_Grenade_Explosion_Dark.P_Grenade_Explosion_Dark")));
}

void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (!BarrelMesh->GetStaticMesh())
	{
		UE_LOG(LogMyProject, Error, TEXT("[Barrel] %s: не выставлен меш бочки"), *GetName());
	}
}

float AExplosiveBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                   AController* EventInstigator, AActor* DamageCauser)
{
	// Не вызываем Super::TakeDamage: он бы применил урон к самому актору, а нам нужен только факт попадания
	if (bExploded || DamageAmount <= 0.f)
	{
		return 0.f;
	}

	bExploded = true;
	UE_LOG(LogMyProject, Warning, TEXT("[Barrel] %s взорвана (урон %.0f от %s)"),
	       *GetName(), DamageAmount, *GetNameSafe(EventInstigator));

	ExplosionComponent->Explode(EventInstigator);

	// После взрыва бочка исчезает: визуал скрыт, коллизия выключена, актор умирает
	BarrelMesh->SetVisibility(false);
	BarrelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(0.2f);      // короткая пауза, чтобы VFX успел стартовать
	return DamageAmount;
}
