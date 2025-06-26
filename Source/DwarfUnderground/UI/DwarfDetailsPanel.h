// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryPanel.h"
#include "Blueprint/UserWidget.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "Components/TextBlock.h"
#include "DwarfDetailsPanel.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateDetailsPanelDelegate, ADwarfUnit*, Target);

UCLASS()
class DWARFUNDERGROUND_API UDwarfDetailsPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//현재 패널이 가리키는 드워프
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dwarf")
	ADwarfUnit* TargetDwarf;

	//패널이 업데이트되는 순간에 대한 Delegate
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnUpdateDetailsPanelDelegate OnUpdateDetailsPanel;

public:
	//패널을 열 때 사용합니다. 누구의 Detail을 보여줄 것인지 정합니다.
	UFUNCTION(BlueprintCallable)
	void OpenPanel(ADwarfUnit* Target);

	//패널을 닫을 때 사용합니다.
	UFUNCTION(BlueprintCallable)
	void ClosePanel();

	//패널을 업데이트합니다.
	// - 패널 내부의 값들은 Binding을 통해 바꿈.
	UFUNCTION(BlueprintCallable)
	void UpdatePanel(ADwarfUnit* Target);

private:
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* DwarfNameTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess="true"))
	// UTextBlock* HPTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* OffenseTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* DefenseTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* AccuracyTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* DodgeTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* MiningTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* BuildingTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* FarmingTextBlock;
	//
	// UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	// UTextBlock* RecruitingTextBlock;

	//UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//UInventoryPanel* InventoryPanel;
};
