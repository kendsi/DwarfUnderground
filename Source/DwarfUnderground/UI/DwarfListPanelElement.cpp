// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfListPanelElement.h"

#include "DwarfUnderground/Subsystem/DwarfDataSubsystem.h"
#include "DwarfUnderground/Unit/UnitStateScript.h"

void UDwarfListPanelElement::SetDwarf(ADwarfUnit* TargetDwarf)
{
	if (!TargetDwarf)
	{
		return;
	}
	Dwarf = TargetDwarf;
	DwarfName = FText::FromString(TargetDwarf->DwarfName);

	if (GetGameInstance())
	{
		if (auto* DataManager = GetGameInstance()->GetSubsystem<UDwarfDataSubsystem>())
		{
			FUnitStateScript Script = DataManager->GetDwarfStateScript(TargetDwarf->GetUnitState());
			StatusMessage = FText::FromString(Script.Korean);
		}
	}


	OnSetDwarf(TargetDwarf);
}

void UDwarfListPanelElement::NativeConstruct()
{
	DwarfName = FText::FromString(TEXT("이름 설정되지 않음"));
	StatusMessage = FText::FromString(TEXT("상태메시지 설정되지 않음"));
}

void UDwarfListPanelElement::HandleElementButtonClicked()
{
	if (!Dwarf)
	{
		UE_LOG(LogTemp, Error, TEXT("DwarfManagementPanelElement: 대상 Dwarf가 존재하지 않습니다(NULL)"));
	}
	OnElementClicked.Broadcast(Dwarf);
	UE_LOG(LogTemp, Display, TEXT("Element Button Clicked: %s"), *DwarfName.ToString());
}
