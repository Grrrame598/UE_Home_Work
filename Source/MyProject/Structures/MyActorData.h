#pragma once

#include "CoreMinimal.h"
#include "MyActorData.generated.h"

/**
 * UENUM for output message type
 */
UENUM(BlueprintType)
enum class EOutputType : uint8
{
	Info    UMETA(DisplayName = "Info"),
	Warning UMETA(DisplayName = "Warning"),
	Error   UMETA(DisplayName = "Error")
};

/**
 * USTRUCT that holds 3 variables from the homework
 */
USTRUCT(BlueprintType, Atomic)
struct FMyActorData
{
	GENERATED_BODY()

	/** Integer value - visible everywhere */
	UPROPERTY(EditAnywhere, Category = "My Actor Data")
	int32 IntValue = 0;

	/** Float value - visible only in Blueprint defaults */
	UPROPERTY(EditDefaultsOnly, Category = "My Actor Data")
	float FloatValue = 0.0f;

	/** String value - visible only on placed instances */
	UPROPERTY(EditInstanceOnly, Category = "My Actor Data")
	FString StringValue = TEXT("Default");
};
