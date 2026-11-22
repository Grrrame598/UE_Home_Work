// B2: зона замедления — вдвое снижает скорость и запрещает бег.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Zones/AttributeZoneBase.h"
#include "SlowdownZone.generated.h"

UCLASS()
class MYPROJECT_API ASlowdownZone : public AAttributeZoneBase
{
	GENERATED_BODY()

public:
	ASlowdownZone();

protected:
	virtual void ApplyEffects(AMyPlayerCharacter* Character) override;
	virtual void OnPlayerLeft(AMyPlayerCharacter* Character) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Множитель к обычной скорости ходьбы внутри зоны */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone|Slowdown", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float SpeedMultiplier = 0.5f;

private:
	/** Кого мы замедлили — чтобы вернуть скорость при выходе */
	UPROPERTY()
	TArray<TWeakObjectPtr<AMyPlayerCharacter>> SlowedCharacters;
};
