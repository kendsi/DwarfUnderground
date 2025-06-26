// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "SquadCreatePanel.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API USquadCreatePanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Squad")
	ADwarfUndergroundPlayerController* PlayerController;
	
	UPROPERTY(BlueprintReadWrite, Category = "SquadCreation")
	TArray<ADwarfUnit*> TemporarySquad;

public:
	USquadCreatePanel(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	bool CreateSquad();
	
	UFUNCTION(BlueprintCallable)
	void UpdateSquad(USquadComponent* Squad);
};
