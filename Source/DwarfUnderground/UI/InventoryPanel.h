// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryPanelSlot.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "InventoryPanel.generated.h"

class UInventorySubSystem;
/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	//생성자 역할
	virtual void NativeConstruct() override;

	//더미 데이터 생성(Inventory)
	void CreateDummyInventory();
	
	//Inventory를 참고하여 패널의 내용을 업데이트한다.
	void UpdatePanel(AUnit* TargetUnit);

	UFUNCTION()
	void HandleInventoryUpdated(FName UpdatedItemID, int32 Amount);


private:
	//실제 아이템 정보
	UInventorySubSystem* Inventory;

	//현재 인벤토리 대상 유닛 정보(Can NULL)
	AUnit* Unit;
	
	//각 패널의 슬롯
	TArray<UInventoryPanelSlot*> InventorySlots;

	//패널 슬롯이 추가될 WrapBox
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess="true"))
	UWrapBox* SlotWrapBox;

	//패널 슬롯의 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UInventoryPanelSlot> InventorySlotClass;

	
};