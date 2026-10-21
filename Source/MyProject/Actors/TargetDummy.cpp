// Простая мишень для демонстрации урона от выстрела.

#include "TargetDummy.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ATargetDummy::ATargetDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	// Кубик 1м, чтобы мишень была заметной на карте
	Mesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	ShowHealth();
}

float ATargetDummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth = FMath::Max(0.f, CurrentHealth - ActualDamage);
	ShowHealth();

	if (CurrentHealth <= 0.f)
	{
		// Мишень «убита»: убираем с карты, но не удаляем актор,
		// чтобы это было видно в PIE и в логе
		UE_LOG(LogTemp, Warning, TEXT("[TargetDummy] %s destroyed"), *GetName());
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}

	return ActualDamage;
}

void ATargetDummy::ShowHealth() const
{
	const FString Message = FString::Printf(TEXT("%s HP: %.0f / %.0f"), *GetName(), CurrentHealth, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("[TargetDummy] %s"), *Message);

	if (GEngine)
	{
		// GetUniqueID() возвращает uint32, а перегрузки принимают int32/uint64 — приводим явно
		GEngine->AddOnScreenDebugMessage(static_cast<int32>(GetUniqueID()), 3.f, FColor::Yellow, Message);
	}
}
