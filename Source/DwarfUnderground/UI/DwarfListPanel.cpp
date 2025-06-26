// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfListPanel.h"

#include "DwarfUnderground/Unit/DwarfManagerSubsystem.h"

void UDwarfListPanel::UpdateDwarfList(TArray<ADwarfUnit*> DwarfList)
{
	UE_LOG(LogTemp, Display, TEXT("굿"));
	Dwarves = DwarfList;
	RenderDwarfList();
}

void UDwarfListPanel::RenderDwarfList()
{
	ElementContainer->ClearChildren();
	//TODO: Add대신 갱신으로 (Create 비쌈)
	for (auto* Dwarf : Dwarves)
	{
		UDwarfListPanelElement* Element = AddElementOnPanel();		
		Element->SetDwarf(Dwarf);
		RenderDwarfListElement(Dwarf, Element);
	}
}

void UDwarfListPanel::RenderDwarfListElement_Implementation(ADwarfUnit* TargetDwarf, UDwarfListPanelElement* Element)
{
}

void UDwarfListPanel::HandleElementClicked(ADwarfUnit* TargetDwarf)
{
    //TODO: 상위 HUD에 전송 or 로 값 가져오기
    OnElementClicked.Broadcast(TargetDwarf);
    UE_LOG(LogTemp, Display, TEXT("Management Panel: 패널 Element 이벤트 호출됨, 대상: %s"), *TargetDwarf->DwarfName);
}

UDwarfListPanelElement* UDwarfListPanel::AddElementOnPanel()
{
	//자식으로 있는 ScrollBox 찾기

	UDwarfListPanelElement* NewPanelElement = CreateWidget<UDwarfListPanelElement>(GetWorld(), PanelElementClass);
	if (!NewPanelElement)
	{
		UE_LOG(LogTemp, Warning, TEXT("DwarfManagementPanel: HUD_DwarfManagementPanelElement 생성 실패"));
		return nullptr;
	}

	NewPanelElement->OnElementClicked.AddDynamic(this, &UDwarfListPanel::HandleElementClicked);
	ElementContainer->AddChild(NewPanelElement);

	return NewPanelElement;
}
