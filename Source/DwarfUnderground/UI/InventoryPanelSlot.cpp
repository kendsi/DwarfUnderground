// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryPanelSlot.h"

#include "DwarfUnderground/Item/EquipmentItem.h"

void UInventoryPanelSlot::SetItem(UItem* Item)
{
	if (Item==nullptr) return;
	SlotItem = Item;
	DefaultBrush = ItemIcon->GetBrush();
	ItemIcon->SetBrushFromTexture(Item->ItemIcon);
	UE_LOG(LogTemp, Display, TEXT("아이템 추가됨"));
	ItemIcon->SetBrushTintColor(FSlateColor(FColor(255, 0, 0, 255)));
}

void UInventoryPanelSlot::SetTargetUnit(AUnit* Unit)
{
	TargetUnit = Unit;
}

void UInventoryPanelSlot::RemoveItem()
{
	SlotItem = nullptr;
	ItemIcon->SetBrush(DefaultBrush);
}

void UInventoryPanelSlot::HandleSlotClicked()
{
	UE_LOG(LogTemp, Display, TEXT("아이템 클릭됨: %s"), *SlotItem->ItemName.ToString());

	//장비일 경우 장착 시도
	if (UEquipmentItem* ItemToEquip = Cast<UEquipmentItem>(SlotItem))
	{
		if (TargetUnit)
		{
			UE_LOG(LogTemp, Display, TEXT("InventoryPanelSlot: 아이템 장착을 시도합니다."));
			ItemToEquip->Equip(TargetUnit);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryPanelSlot: 장착 대상 유닛을 찾을 수 없습니다."));
		}
	}
}
