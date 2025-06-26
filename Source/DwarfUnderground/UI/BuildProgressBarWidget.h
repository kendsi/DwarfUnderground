// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BuildProgressBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UBuildProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UProgressBar* ProgressBar;
    UTextBlock* TextBlock;

public:
    void NativeConstruct() override;

    UFUNCTION()
    void BeginProgress();
    UFUNCTION()
    void UpdateProgress(float Percent);
    UFUNCTION()
    void EndProgress();	
};
