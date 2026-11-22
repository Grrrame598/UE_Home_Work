// B2: зона лечения.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Zones/AttributeZoneBase.h"
#include "HealthRestoreZone.generated.h"

UCLASS()
class MYPROJECT_API AHealthRestoreZone : public AAttributeZoneBase
{
	GENERATED_BODY()

public:
	AHealthRestoreZone();

protected:
	virtual void ApplyEffects(AMyPlayerCharacter* Character) override;

	/** Сколько HP восстанавливается за один тик эффекта */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone|Heal")
	float HealPerInterval = 10.f;
};
