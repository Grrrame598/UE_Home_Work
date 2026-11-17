// B3: интерфейс интерактивных акторов (двери, рычаги и т.п.) — вызывается по клавише E.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveActor.generated.h"

class AMyPlayerCharacter;

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractiveActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Взаимодействие с актором. Реализуется в C++ (override Interact_Implementation)
 * или в Blueprint. Персонаж трассирует вперёд и вызывает Interact() у первого
 * актора, реализующего этот интерфейс.
 */
class MYPROJECT_API IInteractiveActor
{
	GENERATED_BODY()

public:
	/** Вызывается при нажатии E, когда персонаж смотрит на актор */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AMyPlayerCharacter* Caller);
};
