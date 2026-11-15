// B2: зона замедления — вдвое снижает скорость и запрещает бег.

#include "Actors/Zones/SlowdownZone.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "MyPlayerCharacter.h"
#include "MyProject.h"

ASlowdownZone::ASlowdownZone()
{
	ZoneBox->ShapeColor = FColor::Yellow;
}

void ASlowdownZone::ApplyEffects(AMyPlayerCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	// Флаг зоны проверяется в AMyPlayerCharacter::CanSprint — бег запрещён
	Character->SetSprintSuppressed(true);
	Character->StopSprint();
	Character->ApplySpeedMultiplier(SpeedMultiplier);
	SlowedCharacters.AddUnique(Character);
}

void ASlowdownZone::OnPlayerLeft(AMyPlayerCharacter* Character)
{
	if (!Character)
	{
		return;
	}
	// Возврат скорости и права на спринт — при выходе, иначе эффект висит навсегда
	Character->SetSprintSuppressed(false);
	Character->ApplySpeedMultiplier(1.f);
	SlowedCharacters.Remove(Character);
	UE_LOG(LogMyProject, Log, TEXT("[Zone] %s вышел из зоны замедления — скорость восстановлена"), *Character->GetName());
}

void ASlowdownZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Персонажей могло унести из зоны без Exit-события (телепорт, смена уровня)
	for (const TWeakObjectPtr<AMyPlayerCharacter>& WeakCharacter : SlowedCharacters)
	{
		if (AMyPlayerCharacter* Character = WeakCharacter.Get())
		{
			Character->SetSprintSuppressed(false);
			Character->ApplySpeedMultiplier(1.f);
		}
	}
	SlowedCharacters.Reset();

	Super::EndPlay(EndPlayReason);
}
