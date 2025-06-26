// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_WorkBuilding.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "DwarfUnderground/BuildManagement/Buildings/BuildingBase.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "DwarfUnderground/Component/DwarfManagementComponent.h"

UBTTask_WorkBuilding::UBTTask_WorkBuilding()
{
    NodeName = TEXT("Work Building");
}

EBTNodeResult::Type UBTTask_WorkBuilding::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    auto* BB = OwnerComp.GetBlackboardComponent();
    auto* Dwarf = Cast<ADwarfUnit>(OwnerComp.GetAIOwner()->GetPawn());
    auto* Building = Cast<ABuildingBase>(BB->GetValueAsObject(WorkBuildingKey));

    if (!Dwarf || !Building)
        return EBTNodeResult::Failed;

    UDwarfManagementComponent* DMComp = Building->FindComponentByClass<UDwarfManagementComponent>();
    if (DMComp)
    {
        Dwarf->SetActorHiddenInGame(true);
        Dwarf->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Dwarf->SetVisibility(false);

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}