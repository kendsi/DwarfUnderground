// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentBase.h"
#include "Armor.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API AArmor : public AEquipmentBase
{
	GENERATED_BODY()
	
public:
	AArmor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stats")
	float ArmorRating;
};
