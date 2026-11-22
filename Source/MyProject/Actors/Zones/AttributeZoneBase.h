// B2: базовый класс зоны-триггера. Наследники задают ApplyEffects.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttributeZoneBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class AMyPlayerCharacter;

/**
 * Зона на UBoxComponent: отслеживает вход/выход персонажа и раз в IntervalSeconds
 * применяет эффект (урон / лечение / замедление) к тем, кто внутри.
 * Наследники переопределяют ApplyEffects и (по желанию) OnPlayerEntered / OnPlayerLeft.
 */
UCLASS(Abstract)
class MYPROJECT_API AAttributeZoneBase : public AActor
{
	GENERATED_BODY()

public:
	AAttributeZoneBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Эффект зоны для одного персонажа (вызывается периодически) */
	virtual void ApplyEffects(AMyPlayerCharacter* Character) {}

	/** Персонаж вошёл в зону */
	/** Персонаж вошёл в зону */
	virtual void OnPlayerEntered(AMyPlayerCharacter* Character) {}

	/** Персонаж покинул зону */
	virtual void OnPlayerLeft(AMyPlayerCharacter* Character) {}

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                        bool bFromSweep, const FHitResult& SweepResult);

	/** Персонаж покинул зону */
	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Коллизия-объём зоны */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
	UBoxComponent* ZoneBox;

	/** Визуальный объём зоны (полупрозрачный куб), виден в рантайме */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
	UStaticMeshComponent* ZoneVisual;

	/** Показывать объём зоны в игре/PIE */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone|Visual")
	bool bVisibleInGame = true;

	/** Цвет объёма зоны (альфа = прозрачность) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone|Visual")
	FLinearColor ZoneColor = FLinearColor(1.f, 0.f, 0.f, 0.25f);

	/** Период применения эффекта, с */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone", meta = (ClampMin = "0.05"))
	float IntervalSeconds = 1.f;

	/** Персонажи внутри зоны */
	UPROPERTY()
	TArray<TWeakObjectPtr<AMyPlayerCharacter>> OverlappingCharacters;

private:
	FTimerHandle EffectTimer;

	void ApplyToOverlapping();
};
