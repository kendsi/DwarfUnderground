// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "RangedWeapon.generated.h"

UCLASS()
class DWARFUNDERGROUND_API ARangedWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	ARangedWeapon();

public:
	virtual void AttackEnemy(AActor* Enemy) override;
};
