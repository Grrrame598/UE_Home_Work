// B2: зона урона.

#include "Actors/Zones/DamageZone.h"

#include "Kismet/GameplayStatics.h"
#include "MyPlayerCharacter.h"
#include "Components/BoxComponent.h"

ADamageZone::ADamageZone()
{
	ZoneBox->ShapeColor = FColor::Red;
}

void ADamageZone::ApplyEffects(AMyPlayerCharacter* Character)
{
	if (!Character || DamagePerInterval <= 0.f)
	{
		return;
	}

	// Урон идёт через ApplyDamage -> OnTakeAnyDamage в CharacterAttributesComponent
	UGameplayStatics::ApplyDamage(Character, DamagePerInterval, nullptr, this, nullptr);
}
