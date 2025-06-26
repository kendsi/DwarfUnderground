// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentBase.h"
#include "DwarfUnderground/Interface/FightableInterface.h"
#include "Weapon.generated.h"

UCLASS()
class DWARFUNDERGROUND_API AWeapon : public AEquipmentBase, public IFightableInterface
{
	GENERATED_BODY()
	
public:	
	AWeapon();

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stats")
	float AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stats")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stats")
	float AttackCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stats")
	float Accuracy;

	virtual void AttackEnemy(AActor* Enemy) override;
};
