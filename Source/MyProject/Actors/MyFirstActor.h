// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Structures/MyActorData.h"
#include "MyFirstActor.generated.h"

UCLASS()
class MYPROJECT_API AMyFirstActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyFirstActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Struct with 3 variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Actor Data")
	FMyActorData ActorData;

	// Function that prints value to screen based on enum type
	UFUNCTION(BlueprintCallable, Category = "My Actor Functions")
	void PrintValue(EOutputType OutputType);
};
