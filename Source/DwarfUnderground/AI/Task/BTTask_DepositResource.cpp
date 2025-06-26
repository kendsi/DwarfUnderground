// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DepositResource.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"
#include "DwarfUnderground/Resource/ResourceBase.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "AIController.h"

UBTTask_DepositResource::UBTTask_DepositResource()
{
    NodeName = TEXT("Deposit Resource");
}

EBTNodeResult::Type UBTTask_DepositResource::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    auto* BB = OwnerComp.GetBlackboardComponent();
    ADwarfUndergroundPlayerController* PC =Cast<ADwarfUndergroundPlayerController>(OwnerComp.GetWorld()->GetFirstPlayerController());
    auto* Dwarf = Cast<ADwarfUnit>(OwnerComp.GetAIOwner()->GetPawn());

    if (!BB || !PC || !Dwarf) return EBTNodeResult::Failed;

    bool bHasResource = BB->GetValueAsBool(HasResourceKey);

    if (!bHasResource)
        return EBTNodeResult::Failed;

    PC->IncreaseResource(Dwarf->GetResourceType(), Dwarf->GetResrouceAmount());
    Dwarf->SetResourceType(NAME_None);
    Dwarf->SetResourceAmount(0);

    // 블랙보드 초기화
    BB->SetValueAsBool(HasResourceKey, false);

    return EBTNodeResult::Succeeded;
}