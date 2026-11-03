// База любого надеваемого предмета (оружие, граната и т.п.). ДЗ №6.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/MyProjectTypes.h"
#include "EquipableItem.generated.h"

class USkeletalMeshComponent;

/**
 * Предмет, который персонаж может взять в руки и убрать в сокет (кобура/спина).
 *
 * Сам по себе ничего не умеет: только хранит свой тип (по нему AnimBP выбирает позу)
 * и умеет пристёгиваться к сокету. Логика стрельбы — в наследниках (ABaseWeapon и далее).
 */
UCLASS(Abstract)
class MYPROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	AEquipableItem();

	/** Тип предмета — по нему AnimBP персонажа выбирает позу */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	EEquipableItemType GetItemType() const { return ItemType; }

	/**
	 * Предмет берут в руки: крепим к сокету рук и показываем.
	 * Вызывается из UCharacterEquipmentComponent.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual void OnEquipped(USkeletalMeshComponent* CharacterMesh, FName SocketName);

	/**
	 * Предмет убирают: если сокет задан (кобура, спина) — крепим туда и оставляем видимым,
	 * иначе просто прячем.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	virtual void OnUnequipped(USkeletalMeshComponent* CharacterMesh, FName SocketName);

protected:
	/** Тип предмета для анимаций персонажа */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	EEquipableItemType ItemType = EEquipableItemType::None;
};
