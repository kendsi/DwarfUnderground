// Fill out your copyright notice in the Description page of Project Settings.


#include "ManagedDwarfListWidget.h"

#include "DwarfUnderground/Component/DwarfManagementComponent.h"

void UManagedDwarfListWidget::SetTargetActor(AActor* Target)
{
	Super::SetTargetActor(Target);

	TargetDwarfManagementComponent = Target->GetComponentByClass<UDwarfManagementComponent>();
	if (!TargetDwarfManagementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HUD ManagedInfoWidget: Target에 DwarfManagementComponent가 존재하지 않음"));
		return;
	}

	//TargetDwarfManagementComponent->OnDwarfListChanged.AddDynamic(this, &UManagedDwarfListWidget::OnDwarfListChanged);
}

void UManagedDwarfListWidget::RerenderDwarves(const TArray<ADwarfUnit*>& DwarfList)
{
	//특정 이벤트 발생 시 리스트를 재렌더함
	//1. 전체 드워프 수만큼 반복
	for (auto* Dwarf : DwarfList)
	{
		RenderDwarfSlot(Dwarf);
	}
	int32 EmptySlotNum = TargetDwarfManagementComponent->MaxDwarfNum - DwarfList.Num();
	for (int32 i=0;i<EmptySlotNum;++i)
	{
		RenderEmptyDwarfSlot();
	}
}
