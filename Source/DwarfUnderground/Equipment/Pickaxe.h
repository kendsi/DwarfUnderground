// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeWeapon.h"
#include "DwarfUnderground/Interface/MinableInterface.h"
#include "DwarfUnderground/Interface/FightableInterface.h"
#include "Pickaxe.generated.h"

class AResourceBase;

UCLASS()
class DWARFUNDERGROUND_API APickaxe : public AMeleeWeapon, public IMinableInterface
{
	GENERATED_BODY()
	
public:	
	APickaxe();

public:	
	virtual int32 MineResource(AResourceBase* Resource, int32 Amount) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* WorkMontage;
};
