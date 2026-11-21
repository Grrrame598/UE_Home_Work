// Атрибуты персонажа (A3): здоровье и стамина.

#include "Components/CharacterComponents/CharacterAttributesComponent.h"

#include "GameFramework/Actor.h"
#include "MyProject.h"
#include "Characters/MyPlayerCharacter.h"

UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Stamina = MaxStamina;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
	}
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsSprinting)
	{
		Stamina = FMath::Max(0.f, Stamina - SprintStaminaConsumptionVelocity * DeltaTime);
		LogTimer += DeltaTime;
		if (LogTimer >= 1.f)
		{
			LogTimer = 0.f;
			UE_LOG(LogMyProject, Log, TEXT("[Attributes] %s стамина %.0f/%.0f"),
			       *GetNameSafe(GetOwner()), Stamina, MaxStamina);
		}
		if (Stamina <= 0.f)
		{
			// Стамина кончилась — спринт выключает владелец
			bIsSprinting = false;
			if (AMyPlayerCharacter* Character = Cast<AMyPlayerCharacter>(GetOwner()))
			{
				Character->StopSprint();
			}
			UE_LOG(LogMyProject, Log, TEXT("[Attributes] %s -> стамина 0"), *GetNameSafe(GetOwner()));
		}
	}
	else
	{
		const float Before = Stamina;
		Stamina = FMath::Min(MaxStamina, Stamina + StaminaRestoreVelocity * DeltaTime);
		if (Before < Stamina)
		{
			LogTimer += DeltaTime;
			if (LogTimer >= 1.f)
			{
				LogTimer = 0.f;
				UE_LOG(LogMyProject, Log, TEXT("[Attributes] %s стамина %.0f/%.0f (восстановление)"),
				       *GetNameSafe(GetOwner()), Stamina, MaxStamina);
			}
		}
	}
}

float UCharacterAttributesComponent::GetHealthPercent() const
{
	return MaxHealth > 0.f ? Health / MaxHealth : 0.f;
}

void UCharacterAttributesComponent::RestoreHealth(float Amount)
{
	if (Amount <= 0.f || Health <= 0.f)
	{
		return;
	}

	const float Before = Health;
	Health = FMath::Min(MaxHealth, Health + Amount);
	if (Health > Before)
	{
		UE_LOG(LogMyProject, Log, TEXT("[Attributes] %s лечение +%.0f, HP %.0f/%.0f"),
		       *GetNameSafe(GetOwner()), Health - Before, Health, MaxHealth);
	}
}


float UCharacterAttributesComponent::GetStaminaPercent() const
{
	return MaxStamina > 0.f ? Stamina / MaxStamina : 0.f;
}

void UCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage,
                                                    const UDamageType* DamageType,
                                                    AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || Health <= 0.f)
	{
		return;
	}

	Health = FMath::Max(0.f, Health - Damage);
	UE_LOG(LogMyProject, Log, TEXT("[Attributes] %s получил %.0f урона, HP %.0f/%.0f"),
	       *GetNameSafe(DamagedActor), Damage, Health, MaxHealth);

	if (Health <= 0.f)
	{
		UE_LOG(LogMyProject, Warning, TEXT("[Attributes] %s погиб"), *GetNameSafe(GetOwner()));
		OnDeath.Broadcast();
	}
}
