// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ForgePanel.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UForgePanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWeaponList();
};
