#include "BasePlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"

ABasePlatform::ABasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformRoot"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();

	if (PlatformMesh)
	{
		StartLocation = PlatformMesh->GetRelativeLocation();
	}

	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);

		if (TimelineCurve->FloatCurve.Keys.Num() > 0)
		{
			PlatformTimeline.SetTimelineLength(TimelineCurve->FloatCurve.Keys.Last().Time);
		}
		else
		{
			PlatformTimeline.SetTimelineLength(1.0f);
		}

		PlatformTimeline.SetLooping(PlatformBehavior == EPlatformBehavior::Loop);

		if (PlatformBehavior == EPlatformBehavior::Loop)
		{
			PlatformTimeline.Play();
		}
	}

	if (IsValid(PlatformTrigger))
	{
		PlatformTrigger->OnInvocatorActivated.AddDynamic(this, &ABasePlatform::OnPlatformTriggered);
	}
}

void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::PlatformTimelineUpdate(float Alpha)
{
	if (!PlatformMesh)
	{
		return;
	}

	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}

void ABasePlatform::OnPlatformTriggered()
{
	if (PlatformTimeline.IsPlaying())
	{
		PlatformTimeline.Reverse();
	}
	else
	{
		const float Position = PlatformTimeline.GetPlaybackPosition();
		const float Length = PlatformTimeline.GetTimelineLength();

		if (Position >= Length)
		{
			PlatformTimeline.Reverse();
		}
		else
		{
			PlatformTimeline.Play();
		}
	}
}
