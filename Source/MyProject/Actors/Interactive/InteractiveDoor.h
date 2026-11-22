// B3: интерактивная дверь — поворот створки по E с плавной интерполяцией.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "InteractiveDoor.generated.h"

class UStaticMeshComponent;
class UTimelineComponent;
class UCurveFloat;

/**
 * Дверь: корпус + створка (DoorMesh), крутится вокруг локального Z на OpenAngle.
 * Вызывается по E через IInteractiveActor. Без ассета кривой работает линейно —
 * UTimelineComponent не обязателен для DoD, но оставлен как точка расширения в BP.
 */
UCLASS()
class MYPROJECT_API AInteractiveDoor : public AActor, public IInteractiveActor
{
	GENERATED_BODY()

public:
	AInteractiveDoor();

	/** IInteractiveActor: открыть/закрыть по нажатию E */
	virtual void Interact_Implementation(AMyPlayerCharacter* Caller) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Корпус/рама (неподвижная часть) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USceneComponent* RootScene;

	/** Петля: вокруг этой точки крутится створка */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USceneComponent* PivotComponent;

	/** Сама створка */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* DoorMesh;

	/** Угол открытия, градусы (знак задаёт направление поворота) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door", meta = (ClampMin = "-180.0", ClampMax = "180.0"))
	float OpenAngle = 90.f;

	/** Время поворота, с */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door", meta = (ClampMin = "0.05"))
	float OpenDuration = 1.f;

private:
	bool bIsOpen = false;
	bool bIsMoving = false;
	float MoveAlpha = 0.f;          // 0 — закрыто, 1 — открыто

	void UpdateRotation();
	void StartMove(bool bOpen);
};
