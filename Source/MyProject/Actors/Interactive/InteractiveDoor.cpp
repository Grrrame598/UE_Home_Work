// B3: интерактивная дверь.

#include "Actors/Interactive/InteractiveDoor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Characters/MyPlayerCharacter.h"
#include "MyProject.h"
#include "UObject/ConstructorHelpers.h"

AInteractiveDoor::AInteractiveDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;   // тик нужен только на время анимации

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	// Петля: крутим этот компонент. Положение петли задаётся его Relative Location в BP.
	PivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComponent"));
	PivotComponent->SetupAttachment(RootScene);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(PivotComponent);
	// Меш по умолчанию — куб StarterContent; створку и pivot задают в BP.
	// LoadObject, а не FObjectFinder: отсутствие ассета не должно ронять CDO.
	DoorMesh->SetStaticMesh(LoadObject<UStaticMesh>(
		nullptr, TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube")));
	DoorMesh->SetRelativeScale3D(FVector(0.1f, 1.f, 2.f));
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void AInteractiveDoor::BeginPlay()
{
	Super::BeginPlay();

	if (!DoorMesh->GetStaticMesh())
	{
		UE_LOG(LogMyProject, Error, TEXT("[Door] %s: не выставлен меш створки"), *GetName());
	}
	UpdateRotation();
}

void AInteractiveDoor::Interact_Implementation(AMyPlayerCharacter* Caller)
{
	if (bIsMoving)
	{
		return;      // не дёргаем створку посреди анимации
	}
	StartMove(!bIsOpen);
}

void AInteractiveDoor::StartMove(bool bOpen)
{
	bIsOpen = bOpen;
	bIsMoving = true;
	SetActorTickEnabled(true);

	UE_LOG(LogMyProject, Log, TEXT("[Door] %s -> %s"), *GetName(), bOpen ? TEXT("Opening") : TEXT("Closing"));
}

void AInteractiveDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float Direction = bIsOpen ? 1.f : -1.f;
	MoveAlpha = FMath::Clamp(MoveAlpha + Direction * DeltaSeconds / FMath::Max(OpenDuration, 0.05f), 0.f, 1.f);

	UpdateRotation();

	// Дошли до целевого угла — гасим тик, ждём следующего E
	if ((bIsOpen && MoveAlpha >= 1.f) || (!bIsOpen && MoveAlpha <= 0.f))
	{
		bIsMoving = false;
		SetActorTickEnabled(false);
		UE_LOG(LogMyProject, Log, TEXT("[Door] %s: %s"), *GetName(), bIsOpen ? TEXT("открыта") : TEXT("закрыта"));
	}
}

void AInteractiveDoor::UpdateRotation()
{
	// Плавность на концах хода: ease-in-out. Крутим ПЕТЛЮ, а не меш.
	const float Smoothed = FMath::InterpEaseInOut(0.f, 1.f, MoveAlpha, 2.f);
	PivotComponent->SetRelativeRotation(FRotator(0.f, Smoothed * OpenAngle, 0.f));
}
