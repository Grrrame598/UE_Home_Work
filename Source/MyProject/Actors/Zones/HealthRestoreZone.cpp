// B2: зона лечения.

#include "Actors/Zones/HealthRestoreZone.h"

#include "Components/BoxComponent.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "MyPlayerCharacter.h"

AHealthRestoreZone::AHealthRestoreZone()
{
	ZoneBox->ShapeColor = FColor::Green;
}

void AHealthRestoreZone::ApplyEffects(AMyPlayerCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	if (UCharacterAttributesComponent* Attributes = Character->GetCharacterAttributesComponent())
	{
		Attributes->RestoreHealth(HealPerInterval);
	}
}
