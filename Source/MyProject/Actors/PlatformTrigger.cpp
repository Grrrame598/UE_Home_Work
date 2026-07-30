#include "PlatformTrigger.h"
#include "Components/BoxComponent.h"

APlatformTrigger::APlatformTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
}

void APlatformTrigger::Invoke()
{
	OnInvocatorActivated.Broadcast();
}
