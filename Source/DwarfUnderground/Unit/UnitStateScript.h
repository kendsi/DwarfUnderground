// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitState.h"
#include "UnitStateScript.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct  DWARFUNDERGROUND_API FUnitStateScript : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Korean;
};
