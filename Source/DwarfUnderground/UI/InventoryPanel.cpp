// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryPanel.h"

#include "Components/WrapBoxSlot.h"
#include "DwarfUnderground/Item/InventorySubSystem.h"


void UInventoryPanel::NativeConstruct()
{
	Super::NativeConstruct();

	//Dummy Inventory 생성
	// TODO: Inventory 위치 변경
	CreateDummyInventory();

	//패널 리렌더(업데이트)
	UpdatePanel(nullptr);
}

void UInventoryPanel::CreateDummyInventory()
{
	Inventory = NewObject<UInventorySubSystem>(this, UInventorySubSystem::StaticClass(),TEXT("PanelInventory"));
	Inventory->OnInventoryUpdated.AddDynamic(this, &UInventoryPanel::HandleInventoryUpdated);
}

void UInventoryPanel::UpdatePanel(AUnit* TargetUnit)
{
	Unit = TargetUnit;
	UE_LOG(LogTemp, Display, TEXT("InventoryPanel: 인벤토리 패널 업데이트됨"));
	if (TargetUnit)
		UE_LOG(LogTemp, Display, TEXT("InventoryPanel: TargetUnit: %s"), *TargetUnit->GetName());

	//패널 슬롯을 WrapBox에 추가
	if (!SlotWrapBox)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryPanel: Cannot find WrapBox"));
		return;
	}

	if (!InventorySlotClass)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryPanel: Cannot find InventoryPanelSlotClass"));
		return;
	}

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryPanel: Inventory is nullptr"));
		return;
	}

	//TODO: Slot 위젯 자체를 삭제-재생성하는 대신 아이템 정보만 초기화 후 재주입
	SlotWrapBox->ClearChildren();

	TArray<UInventoryPanelSlot*> Slots;
	auto Items = Inventory->GetVisibleItems();
	//인벤토리 슬롯 재렌더
	for (auto ItemID : Items)
	{
		//인벤토리 슬롯 초기화
		if (UInventoryPanelSlot* NewSlot = CreateWidget<UInventoryPanelSlot>(this, InventorySlotClass))
		{
			if (UWrapBoxSlot* WrapBoxSlot = SlotWrapBox->AddChildToWrapBox(NewSlot))
			{
				//WrapBoxSlot 초기 설정
				// - 추후 Blueprint에서 수정 가능하게 변경하면 좋을듯
				WrapBoxSlot->SetFillEmptySpace(false);
				WrapBoxSlot->SetFillSpanWhenLessThan(0.0f);
				WrapBoxSlot->SetHorizontalAlignment(HAlign_Left);
				WrapBoxSlot->SetVerticalAlignment(VAlign_Top);

				//추후 초기화를 위해 슬롯 추가
				Slots.Add(NewSlot);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("InventoryPanel: Cannot create WrapBox"));
			}

			//슬롯에 현재 타겟 유닛 등록
			NewSlot->SetTargetUnit(TargetUnit);

			//아이템 추가 가능 시, 아이템 추가
			//TODO: ItemID로 변경
			NewSlot->SetItem(nullptr);
		}
	}
	for (int32 i = Items.Num(); i < Inventory->MaxSlots; i++)
	{		//인벤토리 슬롯 초기화
		if (UInventoryPanelSlot* NewSlot = CreateWidget<UInventoryPanelSlot>(this, InventorySlotClass))
		{
			if (UWrapBoxSlot* WrapBoxSlot = SlotWrapBox->AddChildToWrapBox(NewSlot))
			{
				//WrapBoxSlot 초기 설정
				// - 추후 Blueprint에서 수정 가능하게 변경하면 좋을듯
				WrapBoxSlot->SetFillEmptySpace(false);
				WrapBoxSlot->SetFillSpanWhenLessThan(0.0f);
				WrapBoxSlot->SetHorizontalAlignment(HAlign_Left);
				WrapBoxSlot->SetVerticalAlignment(VAlign_Top);

				//추후 초기화를 위해 슬롯 추가
				Slots.Add(NewSlot);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("InventoryPanel: Cannot create WrapBox"));
			}

			//슬롯에 현재 타겟 유닛 등록
			NewSlot->SetTargetUnit(TargetUnit);
		}
	}
}

void UInventoryPanel::HandleInventoryUpdated(FName UpdatedItemID, int32 Amount)
{
	UE_LOG(LogTemp, Display, TEXT("IP good"));
	UpdatePanel(Unit);
}
