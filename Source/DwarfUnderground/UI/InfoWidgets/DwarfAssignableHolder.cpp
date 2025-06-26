// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfAssignableHolder.h"


void UDwarfAssignableHolder::SetDwarf(ADwarfUnit* Dwarf)
{
	TargetDwarf = Dwarf;
	UE_LOG(LogTemp, Display, TEXT("ListElement: Dwarf Set"));
	OnSetDwarf(Dwarf);
}

void UDwarfAssignableHolder::RemoveDwarf()
{
	TargetDwarf = nullptr;
	OnRemoveDwarf();
}

ADwarfUnit* UDwarfAssignableHolder::GetDwarf()
{
	return TargetDwarf;
}

void UDwarfAssignableHolder::NativeConstruct()
{
	Super::NativeConstruct();

	if (!TargetDwarf)
	{
		OnRemoveDwarf();
	}
	else
	{
		OnSetDwarf(TargetDwarf);
	}
}

void UDwarfAssignableHolder::RequestAssignDwarf()
{
	OnDwarfAddRequested.Broadcast();
}
