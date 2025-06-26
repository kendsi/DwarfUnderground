// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "DwarfUnderground/Item/Item.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "InventoryPanelSlot.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UInventoryPanelSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UItem* SlotItem;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UFUNCTION(BlueprintCallable)
	void SetItem(UItem* Item);

	UFUNCTION(BlueprintCallable)
	void SetTargetUnit(AUnit* Unit);

	UFUNCTION(BlueprintCallable)
	void RemoveItem();

	UFUNCTION(BlueprintCallable)
	void HandleSlotClicked();

private:
	FSlateBrush DefaultBrush;

	AUnit* TargetUnit;
};
