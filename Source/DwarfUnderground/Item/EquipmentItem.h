// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "DwarfUnderground/Equipment/Weapon.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "EquipmentItem.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UEquipmentItem : public UItem
{
	GENERATED_BODY()

public:
	UEquipmentItem();
	
	UPROPERTY(BlueprintReadWrite)
	AUnit* EquippedUnit;

	UFUNCTION(BlueprintCallable)
	bool Equip(AUnit* TargetUnit);

	UFUNCTION(BlueprintCallable)
	void Unequip();

	UFUNCTION(BlueprintCallable)
	bool IsEquippable();

private:
	TSubclassOf<AWeapon> DummyEquipmentActor;
};
