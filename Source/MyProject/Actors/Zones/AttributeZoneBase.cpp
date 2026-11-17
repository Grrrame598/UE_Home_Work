// B2: базовый класс зоны-триггера.

#include "Actors/Zones/AttributeZoneBase.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/World.h"
#include "MyPlayerCharacter.h"
#include "TimerManager.h"

AAttributeZoneBase::AAttributeZoneBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
	ZoneBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));
	ZoneBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(ZoneBox);

	// Полупрозрачный куб, видимый в игре: базовый куб 100×100×100 см,
	// масштаб подгоняется под Box Extent в конструкторе/OnConstruction.
	ZoneVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneVisual"));
	ZoneVisual->SetupAttachment(ZoneBox);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		ZoneVisual->SetStaticMesh(CubeMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMat(
		TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
	if (TranslucentMat.Succeeded())
	{
		ZoneVisual->SetMaterial(0, TranslucentMat.Object);
	}
	ZoneVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ZoneVisual->SetCastShadow(false);
	FVector Ext = ZoneBox->GetUnscaledBoxExtent();
	ZoneVisual->SetRelativeScale3D(Ext * 2.f / 100.f);
}

void AAttributeZoneBase::BeginPlay()
{
	Super::BeginPlay();

	// Визуал: привязать к текущему Box Extent (может быть изменён в Details) и покрасить
	if (ZoneVisual)
	{
		FVector Ext = ZoneBox->GetUnscaledBoxExtent();
		ZoneVisual->SetRelativeScale3D(Ext * 2.f / 100.f);
		ZoneVisual->SetVisibility(bVisibleInGame);

		if (UMaterialInstanceDynamic* MID = ZoneVisual->CreateDynamicMaterialInstance(0))
		{
			MID->SetVectorParameterValue(TEXT("Color"), ZoneColor);
			MID->SetVectorParameterValue(TEXT("BaseColor"), ZoneColor);
		}
	}

	ZoneBox->OnComponentBeginOverlap.AddDynamic(this, &AAttributeZoneBase::HandleBeginOverlap);
	ZoneBox->OnComponentEndOverlap.AddDynamic(this, &AAttributeZoneBase::HandleEndOverlap);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(EffectTimer, this, &AAttributeZoneBase::ApplyToOverlapping,
		                                  IntervalSeconds, true);
	}
}

void AAttributeZoneBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(EffectTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void AAttributeZoneBase::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                            bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMyPlayerCharacter* Character = Cast<AMyPlayerCharacter>(OtherActor))
	{
		OverlappingCharacters.AddUnique(Character);
		OnPlayerEntered(Character);
	}
}

void AAttributeZoneBase::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMyPlayerCharacter* Character = Cast<AMyPlayerCharacter>(OtherActor))
	{
		OverlappingCharacters.Remove(Character);
		OnPlayerLeft(Character);
	}
}

void AAttributeZoneBase::ApplyToOverlapping()
{
	// Чистим мусор заодно с применением: персонажи могли умереть/удалиться
	for (int32 Index = OverlappingCharacters.Num() - 1; Index >= 0; --Index)
	{
		AMyPlayerCharacter* Character = OverlappingCharacters[Index].Get();
		if (!Character)
		{
			OverlappingCharacters.RemoveAt(Index);
			continue;
		}
		ApplyEffects(Character);
	}
}
