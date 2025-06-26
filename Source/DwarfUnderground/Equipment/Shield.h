// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Shield.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API AShield : public AWeapon
{
	GENERATED_BODY()

public:
	AShield();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* BlockMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stats")
	float ArmorRating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stats")
	float Evasion;
};
