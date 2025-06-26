// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LogUI.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API ULogUI : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	static void Log(FString Text, float Duration = 1.0f);
	
};
