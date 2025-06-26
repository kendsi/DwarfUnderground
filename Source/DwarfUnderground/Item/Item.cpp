// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

UItem::UItem()
{
	ID = GetNextItemID();

	ItemName = FText::FromString("Init Item name");
	ItemDescription = FText::FromString("Init Item description");
	ItemIcon = nullptr;
	Quantity = 1;
	bIsStackable = false;
	bIsVisible = true;
}

uint32 UItem::GetID() const
{
	return ID;
}

bool UItem::IsVisible() const
{
	return bIsVisible;
}

uint32 UItem::GetNextItemID()
{
	static uint32 ItemID = 0;
	return ItemID++;
}
