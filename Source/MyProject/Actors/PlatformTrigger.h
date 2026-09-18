#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PlatformTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvocatorActivated);

UCLASS()
class MYPROJECT_API APlatformTrigger : public AActor
{
	GENERATED_BODY()

public:
	APlatformTrigger();

	UFUNCTION(BlueprintCallable)
	void Invoke();

	UPROPERTY(BlueprintAssignable)
	FOnInvocatorActivated OnInvocatorActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerBox;
};
