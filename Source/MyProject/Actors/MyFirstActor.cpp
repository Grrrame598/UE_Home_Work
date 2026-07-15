// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFirstActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyFirstActor::AMyFirstActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyFirstActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyFirstActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Print value to screen based on enum type
void AMyFirstActor::PrintValue(EOutputType OutputType)
{
	FString ValueStr = FString::Printf(TEXT("Int: %d | Float: %.2f | String: %s"),
		ActorData.IntValue,
		ActorData.FloatValue,
		*ActorData.StringValue);

	FColor MessageColor;
	switch (OutputType)
	{
	case EOutputType::Info:
		MessageColor = FColor::Cyan;
		break;
	case EOutputType::Warning:
		MessageColor = FColor::Yellow;
		break;
	case EOutputType::Error:
		MessageColor = FColor::Red;
		break;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, MessageColor, ValueStr);
	}
}

