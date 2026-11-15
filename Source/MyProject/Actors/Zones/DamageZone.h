// B2: зона урона — наносит DamagePerSecond тем, кто внутри.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Zones/AttributeZoneBase.h"
#include "DamageZone.generated.h"

UCLASS()
class MYPROJECT_API ADamageZone : public AAttributeZoneBase
{
	GENERATED_BODY()

public:
	ADamageZone();

protected:
	virtual void ApplyEffects(AMyPlayerCharacter* Character) override;

	/** Урон за один тик эффекта (при IntervalSeconds = 1 — это урон в секунду) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone|Damage")
	float DamagePerInterval = 10.f;
};
