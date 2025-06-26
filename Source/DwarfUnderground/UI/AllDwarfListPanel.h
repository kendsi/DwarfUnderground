// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DwarfListPanel.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "DwarfListPanelElement.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"
#include "AllDwarfListPanel.generated.h"

/**
 * 
 */

 //버튼 클릭에 대한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryPanelVisibilityChangedDelegate, ESlateVisibility, Visibility, ADwarfUnit*, TargetDwarf);

UCLASS()
class DWARFUNDERGROUND_API UAllDwarfListPanel : public UDwarfListPanel
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "UIEvent")
	FOnInventoryPanelVisibilityChangedDelegate OnInventoryPanelToggleRequest;

	UPROPERTY(BlueprintReadWrite, Category = "Dwarf")
	int32 DwarfNum;

	bool IsRendered;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void ReRenderPanel();
};
